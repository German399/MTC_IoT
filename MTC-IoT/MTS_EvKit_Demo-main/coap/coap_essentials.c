#include "coap_essentials.h"

/**
    @file coap_essentials.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

static uint8_t URI_copy[MAX_URI_LENGTH];
static uint8_t* URI_ptrs[MAX_URI_ENTRIES];

void CoAP_parse_message_header(uint8_t *hdr,CoAP_header_info_t *out)
{
    /* Notice: header is assumed to be in a network byte order, i.e. as received from UDP layer. */
    out->version      = (hdr[0] & 0xC0) >>  6;
    out->type         = (hdr[0] & 0x30) >>  4;
    out->token_length = (hdr[0] & 0x0F) >>  0;
    out->code_class   = (hdr[1] & 0xE0) >>  5;
    out->code_detail  = (hdr[1] & 0x1F) >>  0;
    out->message_ID   = (((uint16_t)hdr[2]) << 8) | (hdr[3]);
}

uint16_t CoAP_parse_option(uint8_t *buf,CoAP_option_info_t *out)
{
    uint16_t buf_index;
    uint16_t option_delta,option_length;

    buf_index = 0;

    option_delta = buf[buf_index] >> 4;
    option_length = buf[buf_index] & 0x0F;

    buf_index++;

    switch (option_delta)
    {
        case COAP_OPTION_DELTA_EXTRA1B:
            /* One extra byte follows the main header, which contains option delta minus 13 */
            option_delta = buf[buf_index] + 13;
            buf_index++;
        break;
        case COAP_OPTION_DELTA_EXTRA2B:
            /* Two extra bytes follow the main header (in a network order), which contain option delta minus 269 */
            option_delta = ((((uint16_t)(buf[buf_index]))<<8) | (buf[buf_index+1])) + 269;
            buf_index+=2;
        break;
        case COAP_OPTION_DELTA_DATA_MKR:
            /* This is probably a data marker */
            if (buf[0]==0xFF)
            {
                /* Data marker has a length of one byte */
                return 1;
            }
            else
            {
                /* This is a message format violation */
                return 0xFFFF;
            }
        break;
        default:
            /* Nothing needs to be done, option delta is specified directly */
        break;
    }

    switch (option_length)
    {
        case COAP_OPTION_LENGTH_EXTRA1B:
            /* One extra byte precedes data, which contains data length value minus 13 */
            option_length = buf[buf_index] + 13;
            buf_index++;
        break;
        case COAP_OPTION_LENGTH_EXTRA2B:
            /* Two extra bytes precede data, which contain data length value minus 269 */
            option_length = ((((uint16_t)(buf[buf_index]))<<8) | (buf[buf_index+1])) + 269;
            buf_index+=2;
        break;
        case COAP_OPTION_LENGTH_DATA_MKR:
            if (buf[0]==0xFF)
            {
                return 1;
            }
            else
            {
                return 0xFFFF;
            }
        break;
        default:
            /* Nothing needs to be done, option content length was specified directly */
        break;
    }

    out->option_id = out->option_id + option_delta;
    out->data_length = option_length;
    out->data_ptr = &(buf[buf_index]);

    return (buf_index + option_length);
}

CoAP_parsing_error_t CoAP_parse_message(uint8_t *buf,uint16_t buf_length,uint8_t **data_start)
{
    CoAP_header_info_t hdr_data;
    CoAP_option_info_t opt_data;
    uint16_t buf_ptr;
    uint16_t safety_counter;
    uint16_t total_option_length;

    CoAP_parse_message_header(buf,&hdr_data);

    if (hdr_data.version!=COAP_MSG_VERSION)
    {
        //Unsupported (and probably non-existent) protocol version
        return ERR_WRONG_PROTOCOL_VER;
    }

    if ((hdr_data.type!=COAP_TYPE_CONFIRMABLE) &&
        (hdr_data.type!=COAP_TYPE_NON_CONFIRMABLE) &&
        (hdr_data.type!=COAP_TYPE_ACK) &&
        (hdr_data.type!=COAP_TYPE_RESET))
    {
        //Unknown message type
        return ERR_UNKNOWN_MSG_TYPE;
    }

    if (hdr_data.token_length>8)
    {
        //Unallowed token length
        return ERR_WRONG_TKL;
    }

    //Find out where the options start
    buf_ptr = COAP_MSG_HDR_LENGTH + hdr_data.token_length;

    if ((buf_ptr<buf_length) && (buf[buf_ptr]!=0xFF)) /* This means that there is at least one option after the header/token */
    {
        //It is essential to initialize option ID to zero, for the options are encoded in incremental form
        opt_data.option_id=0;
        safety_counter=0;
        while ((buf[buf_ptr]!=0xFF) && (safety_counter<MAX_PARSED_OPTIONS) && (buf_ptr<buf_length))
        {
            total_option_length = CoAP_parse_option(&(buf[buf_ptr]),&opt_data);

            if (total_option_length == 0xFFFF)
            {
                return ERR_WRONG_OPTION;
            }

            buf_ptr += total_option_length;
            safety_counter++;
        }

        if (safety_counter>=MAX_PARSED_OPTIONS)
        {
            //Too much options
            return ERR_TOO_MUCH_OPTIONS;
        }
    }

    if (data_start!=NULL)
    {
        if ((hdr_data.code_class == COAP_CLASS_RESP_SUCCESS) && (hdr_data.code_detail == 5))
        {
            /* Returning data pointer only makes sense when the code is 2.05, "content" */
            (*data_start) = &(buf[buf_ptr+1]);
        }
        else
        {
            (*data_start) = NULL;
        }
    }

    return ERR_NONE;
}

uint16_t CoAP_assemble_request_extended(uint8_t request_method,uint8_t is_observe_request,uint8_t *msg_buffer,uint16_t buffer_length,uint8_t *url_string,uint8_t *data,uint16_t data_length)
{
    static uint8_t message_ID_counter=0;

    uint16_t uri_ptr_counter;
    uint16_t uri_entry_length;
    uint8_t option_id;
    uint16_t buffer_ptr;
    uint16_t k,j;

    /* Populating CoAP header; one-byte token assumed for a request. */
    msg_buffer[0] = (COAP_MSG_VERSION<<6) | (COAP_TYPE_NON_CONFIRMABLE<<4) | 1;
    msg_buffer[1] = (COAP_CLASS_REQUEST<<5) | (request_method & 0x1F);
    msg_buffer[2] = (message_ID_counter & 0xFF00) >> 8;
    msg_buffer[3] = (message_ID_counter & 0x00FF);

    /* Populating token field. */
    msg_buffer[4] = message_ID_counter & 0x00FF;

    message_ID_counter++;

    if (url_string[0]!='/')
    {
        return 0;
    }

    option_id = 0;
    buffer_ptr = COAP_MSG_HDR_LENGTH + 1;

    if ((is_observe_request != 0) && (request_method == COAP_METHOD_GET))
    {
        /* Add an OBSERVE option */
        option_id = COAP_OPTION_OBSERVE;
        msg_buffer[buffer_ptr] = (option_id << 4) | 1;
        buffer_ptr++;
        msg_buffer[buffer_ptr] = 0; //OBSERVE = 0 (register)
        buffer_ptr++;
    }

    strncpy(URI_copy,url_string,MAX_URI_LENGTH);

    /* Splitting URI into entries */
    k=0;
    uri_ptr_counter=0;
    while ((URI_copy[k]!=0) && (k<MAX_URI_LENGTH))
    {
        if (URI_copy[k]=='/')
        {
            if (uri_ptr_counter>=MAX_URI_ENTRIES)
            {
                return 0;
            }

            URI_ptrs[uri_ptr_counter] = &(URI_copy[k+1]);
            URI_copy[k]=0;
            uri_ptr_counter++;
        }
        k++;
    }

    for (k=0; k<uri_ptr_counter; k++)
    {
        if (buffer_ptr>=buffer_length)
        {
            return 0;
        }

        /*
            CoAP option identifiers are encoded in a weird delta-format,
            so the first option has to have the actual value, and all subsequent similar
            must have id of zero.
        */
        if (k==0)
        {
            option_id = COAP_OPTION_URI_PATH - option_id;
        }
        else
        {
            option_id = 0;
        }

        uri_entry_length = strlen(URI_ptrs[k]);

        /*
            CoAP weirdness does not end there. The option header is dynamic. If the length of a string
            is lower than 13 bytes, one has to fit length value into header byte. Otherwise a byte or two have to
            be appended, which contain string size minus 13 (and this prevents implementer from hardcoding
            this byte to be present and simply changing its value; let's thank authors of RFC7252 for this).
        */
        if (uri_entry_length<13)
        {
            msg_buffer[buffer_ptr] = (option_id << 4) | uri_entry_length;
            buffer_ptr++;
        }
        else
        {
            msg_buffer[buffer_ptr] = (option_id << 4) | 13;
            buffer_ptr++;
            msg_buffer[buffer_ptr] = uri_entry_length - 13;
            buffer_ptr++;
        }

        /*
            And now copy the value of an option.
        */
        for (j=0; j<uri_entry_length; j++)
        {
            msg_buffer[buffer_ptr] = URI_ptrs[k][j];
            buffer_ptr++;

            if (buffer_ptr>=buffer_length)
            {
                return 0;
            }
        }
    }

    /*
        Now we can copy the data, if present.
    */
    if ((data!=NULL) && (data_length!=0))
    {
        /*
            Placing the data start marker.
            Fun fact: there's no guarantee that the value of 0xFF can not be present among option data,
            so, when parsing, you can not skip options directly to data by searching for 0xFF. Haha.
        */
        msg_buffer[buffer_ptr] = 0xFF;
        buffer_ptr++;

        /*
            Copy data.
        */
        for (j=0; j<data_length; j++)
        {
            msg_buffer[buffer_ptr] = data[j];
            buffer_ptr++;

            if (buffer_ptr>=buffer_length)
            {
                return 0;
            }
        }
    }

    return buffer_ptr;
}
