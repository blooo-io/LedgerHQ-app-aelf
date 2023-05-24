#include "sol/parser.h"
#include "util.h"
#include <stdio.h>
#include "pb_encode.h"
#include "pb_decode.h"
#include "sol/pb.h"
#include "message_pb.h"


#define OFFCHAIN_MESSAGE_SIGNING_DOMAIN \
    "\xff"                              \
    "aelf offchain"

static int check_buffer_length(Parser* parser, size_t num) {
    return parser->buffer_length < num ? 1 : 0;
}

static void advance(Parser* parser, size_t num) {
    parser->buffer += num;
    parser->buffer_length -= num;
}

int parse_u8(Parser* parser, uint8_t* value) {
    BAIL_IF(check_buffer_length(parser, 1));
    *value = *parser->buffer;
    advance(parser, 1);
    return 0;
}

static int parse_u16(Parser* parser, uint16_t* value) {
    uint8_t lower, upper;
    BAIL_IF(parse_u8(parser, &lower));
    BAIL_IF(parse_u8(parser, &upper));
    *value = lower + ((uint16_t) upper << 8);
    return 0;
}

int parse_u32(Parser* parser, uint32_t* value) {
    uint16_t lower, upper;
    BAIL_IF(parse_u16(parser, &lower));
    BAIL_IF(parse_u16(parser, &upper));
    *value = lower + ((uint32_t) upper << 16);
    return 0;
}

int parse_u64(Parser* parser, uint64_t* value) {
    BAIL_IF(check_buffer_length(parser, 8));
    uint32_t lower, upper;
    BAIL_IF(parse_u32(parser, &lower));
    BAIL_IF(parse_u32(parser, &upper));
    *value = lower + ((uint64_t) upper << 32);
    return 0;
}

int parse_i64(Parser* parser, int64_t* value) {
    uint64_t* as_u64 = (uint64_t*) value;
    return parse_u64(parser, as_u64);
}

int parse_length(Parser* parser, size_t* value) {
    uint8_t value_u8;
    BAIL_IF(parse_u8(parser, &value_u8));
    *value = value_u8 & 0x7f;

    if (value_u8 & 0x80) {
        BAIL_IF(parse_u8(parser, &value_u8));
        *value = ((value_u8 & 0x7f) << 7) | *value;
        if (value_u8 & 0x80) {
            BAIL_IF(parse_u8(parser, &value_u8));
            *value = ((value_u8 & 0x7f) << 14) | *value;
        }
    }
    return 0;
}

int parse_option(Parser* parser, enum Option* value) {
    uint8_t maybe_option;
    BAIL_IF(parse_u8(parser, &maybe_option));
    switch (maybe_option) {
        case OptionNone:
        case OptionSome:
            *value = (enum Option) maybe_option;
            return 0;
        default:
            break;
    }
    return 1;
}

int parse_sized_string(Parser* parser, SizedString* string) {
    BAIL_IF(parse_u64(parser, &string->length));
    BAIL_IF(string->length > SIZE_MAX);
    size_t len = (size_t) string->length;
    BAIL_IF(check_buffer_length(parser, len));
    string->string = (const char*) parser->buffer;
    advance(parser, len);
    return 0;
}

int parse_pubkey(Parser* parser, const Pubkey** pubkey) {
    PRINTF("PUB1 %d", pubkey);
    BAIL_IF(check_buffer_length(parser, PUBKEY_SIZE));
    *pubkey = (const Pubkey*) parser->buffer;
    PRINTF("PUB2 %d", pubkey);
    advance(parser, PUBKEY_SIZE);
    return 0;
}

int parse_hash(Parser* parser, const Hash** hash) {
    BAIL_IF(check_buffer_length(parser, HASH_SIZE));
    *hash = (const Hash*) parser->buffer;
    advance(parser, HASH_SIZE);
    return 0;
}

int parse_version(Parser* parser, MessageHeader* header) {
    BAIL_IF(check_buffer_length(parser, 1));
    const uint8_t version = *parser->buffer;
    if (version & 0x80) {
        header->versioned = true;
        header->version = version & 0x7f;
        advance(parser, 1);
    } else {
        header->versioned = false;
        header->version = 0;
    }
    return 0;
}

// int parse_message_header(Parser* parser, MessageHeader* header) {

//     /* The first four bytes of the referenced block hash. */
//     pb_callback_t ref_block_prefix;
//     /* The name of a method in the smart contract at the To address. */
//     pb_callback_t method_name;
//     /* The parameters to pass to the smart contract method. */
//     BAIL_IF(pb_decode));
//     BAIL_IF(parse_pubkey(parser, &header->from));
//     BAIL_IF(parse_u8(parser, &has_to));
//     BAIL_IF(parse_pubkey(parser, &header->to));
//     BAIL_IF(parse_u64(parser, (uint64_t) &ref_block_number));
//     BAIL_IF(parse_u16(parser, (uint16_t) &ref_block_prefix));
//     BAIL_IF(parse_u16(parser, (uint16_t) &ref_block_prefix));
//     return 0;
// }

int parse_data(Parser* parser, const uint8_t** data, size_t* data_length) {
    BAIL_IF(parse_length(parser, data_length));
    BAIL_IF(check_buffer_length(parser, *data_length));
    *data = parser->buffer;
    advance(parser, *data_length);
    return 0;
}


#define BUFFER_SIZE 256

void tohex(unsigned char * in, size_t insz, char * out, size_t outsz)
{
    unsigned char * pin = in;
    const char * hex = "0123456789ABCDEF";
    char * pout = out;
    for(; pin < in+insz; pout +=2, pin++){
        pout[0] = hex[(*pin>>4) & 0xF];
        pout[1] = hex[ *pin     & 0xF];
        if (pout + 2 - out > (int) outsz){
            /* Better to truncate output string than overflow buffer */
            /* it would be still better to either return a status */
            /* or ensure the target buffer is large enough and it never happen */
            break;
        }
    }
    pout[-1] = 0;
}

bool read_address_field(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    pb_byte_t* buffer = *arg;
    pb_byte_t binBuffer[BUFFER_SIZE] = {0};
    size_t binToRead = stream->bytes_left;
    bool status = pb_read(stream, binBuffer, binToRead);
    if (!status)
        return status;
    tohex(binBuffer, binToRead, (char*) buffer, 2*binToRead);
    return true;
}

bool read_string_field(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    pb_byte_t* buffer = *arg;
    return pb_read(stream, buffer, stream->bytes_left);
}

bool read_transfer_input(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    bool status = pb_decode(stream, aelf_TransferInput_fields, *arg);
    return status;
}

int parse_transfer(Parser* parser) {
    // char txn_data[] = {
    //     0x0a, 0x22, 0x0a, 0x20, 0xcd, 0xef, 0xe7, 0x28,
    //     0x49, 0x31, 0x33, 0xf5, 0x26, 0xcb, 0x5e, 0x97,
    //     0xe2, 0xec, 0x33, 0x9c, 0xa2, 0x19, 0xbe, 0xf8,
    //     0x04, 0x27, 0xea, 0xf5, 0x3f, 0xf0, 0x00, 0x3c,
    //     0xad, 0x24, 0x1c, 0x7a, 0x12, 0x22, 0x0a, 0x20,
    //     0x27, 0x91, 0xe9, 0x92, 0xa5, 0x7f, 0x28, 0xe7,
    //     0x5a, 0x11, 0xf1, 0x3a, 0xf2, 0xc0, 0xae, 0xc8,
    //     0xb0, 0xeb, 0x35, 0xd2, 0xf0, 0x48, 0xd4, 0x2e,
    //     0xba, 0x89, 0x01, 0xc9, 0x2e, 0x03, 0x78, 0xdc,
    //     0x18, 0x8d, 0xcd, 0xa2, 0x4b, 0x22, 0x04, 0x5a,
    //     0x74, 0x47, 0x38, 0x2a, 0x08, 0x54, 0x72, 0x61,
    //     0x6e, 0x73, 0x66, 0x65, 0x72, 0x32, 0x34, 0x0a,
    //     0x22, 0x0a, 0x20, 0x4f, 0xf4, 0xe6, 0x3a, 0xd4,
    //     0xaa, 0x7e, 0xc9, 0x2e, 0x65, 0xba, 0x2d, 0x37,
    //     0xb2, 0xc5, 0x6b, 0x3f, 0x82, 0x39, 0x0b, 0xfc,
    //     0x25, 0xe6, 0x6c, 0xeb, 0xab, 0x68, 0x21, 0xf3,
    //     0xb0, 0x5c, 0x0b, 0x12, 0x03, 0x45, 0x4c, 0x46,
    //     0x18, 0x80, 0xad, 0xe2, 0x04, 0x22, 0x04, 0x74,
    //     0x65, 0x73, 0x74, 0x82, 0xf1, 0x04, 0x41, 0xf0,
    //     0x82, 0xe6, 0xda, 0x05, 0x8f, 0x7a, 0x3d, 0xf4,
    //     0x6a, 0xd6, 0x39, 0xe6, 0xaa, 0x7c, 0xa4, 0xf6,
    //     0xe3, 0x8c, 0x87, 0xbd, 0xc2, 0xbd, 0x96, 0x23,
    //     0x4f, 0xe1, 0x98, 0xb5, 0x30, 0xb4, 0xe0, 0x20,
    //     0xc1, 0xa3, 0xab, 0x04, 0xed, 0x92, 0xc2, 0x05,
    //     0x71, 0xce, 0x14, 0xa3, 0x37, 0x24, 0xb8, 0xfa,
    //     0xd3, 0x1d, 0x19, 0xfc, 0xe0, 0xb1, 0xea, 0xaf,
    //     0x3c, 0xf3, 0xa1, 0x4e, 0x60, 0xdf, 0x7d, 0x01};

    aelf_TransferInput transfer_input = aelf_TransferInput_init_zero;

    pb_byte_t symbolBuffer[BUFFER_SIZE] = {0};
    pb_byte_t memoBuffer[BUFFER_SIZE] = {0};
    pb_byte_t addressBuffer[BUFFER_SIZE] = {0};

    transfer_input.symbol.funcs.decode = read_string_field;
    transfer_input.memo.funcs.decode = read_string_field;
    transfer_input.to.value.funcs.decode = read_address_field;
    transfer_input.symbol.arg = &symbolBuffer;
    transfer_input.memo.arg = &memoBuffer;
    transfer_input.to.value.arg = &addressBuffer;

    aelf_Transaction txn = aelf_Transaction_init_zero;

    txn.params.funcs.decode = read_transfer_input;
    txn.params.arg = &transfer_input;

    pb_istream_t stream = pb_istream_from_buffer((const pb_byte_t *) parser->buffer, parser->buffer_length);

    bool status;
    status = pb_decode(&stream, aelf_Transaction_fields, &txn);
    // if (!status)
    // {
    //     printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    //     return 1;
    // }

    PRINTF("Address   : %s\n", addressBuffer);
    PRINTF("Symbol    : %s\n", symbolBuffer);
    PRINTF("Amount    : %llu\n", transfer_input.amount);
    PRINTF("Memo      : %s\n", memoBuffer);

    return 0;
}