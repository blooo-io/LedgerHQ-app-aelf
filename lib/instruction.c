#include "instruction.h"
#include "aelf/parser.h"
#include "aelf/transaction_summary.h"
#include "util.h"
#include <string.h>
#include "apdu.h"

// Function to compare two strings
static inline int is_equal_string(const SizedString* string1, const char* string2) {
    const char* array = string2;
    const char* str = string1->string;

    // Iterate through the string and compare with array
    while (*array != '\0' && *str != '\0') {
        if (*array != *str) {
            return 1;  // Not equal
        }
        array++;  // Move to the next character in array
        str++;    // Move to the next character in the string
    }

    return 0;
}

static inline bool pubkeys_equal(const Pubkey* pubkey1, const Pubkey* pubkey2) {
    return memcmp(pubkey1, pubkey2, PUBKEY_SIZE) == 0;
}

static int parseParams(Parser* parser, SystemTransferInfo* info) {
    uint8_t index = 0;
    while (index < FIELD_NUMBER_PARAMS) {
        uint64_t key;
        readVarInt(parser, &key);
        uint32_t field_number = key >> 3;
        switch (field_number) {
            case 1:
                // Address 'to'
                {
                    advance(parser, 2);
                    uint64_t length;
                    readVarInt(parser, &length);
                    BAIL_IF(parse_pubkey(parser, &info->dest));
                    index++;
                }
                break;
            case 2:
                // Ticker (String)
                {
                    readVarInt(parser, &info->ticker.length);
                    BAIL_IF(parse_sized_string(parser, &info->ticker));
                    if (is_equal_string(&info->ticker, TICKER_ELF)) {
                        THROW(ApduReplyAelfWrongTicker);
                    }
                    index++;
                }
                break;
            case 3:
                // Amount (Varint)
                {
                    BAIL_IF(readVarInt(parser, (uint64_t*) &info->amount););
                    index++;
                }
                break;
            case 4:
                // Memo (string)
                {
                    readVarInt(parser, &info->memo.length);
                    BAIL_IF(parse_sized_string(parser, &info->memo));
                    index++;
                }
                break;
            default:
                return 1;
        }
    }
    return 0;
}

int parse_system_transfer_instruction(Parser* parser, SystemTransferInfo* info) {
    while (parser->buffer_length) {
        uint64_t key;
        readVarInt(parser, &key);
        uint32_t field_number = key >> 3;
        switch (field_number) {
            case 1:
                // Address from
                {
                    advance(parser, 2);
                    uint64_t length;
                    readVarInt(parser, &length);
                    BAIL_IF(parse_pubkey(parser, &info->from));
                }
                break;
            case 2:
                // Address 'to'
                {
                    advance(parser, 2);
                    uint64_t length;
                    readVarInt(parser, &length);
                    BAIL_IF(parse_pubkey(parser, &info->to));

                    // TODO: Implement contract address validation for mainchain and sidechains on mainnet and testnet.
                    /* const Pubkey sm_address = SMART_CONTRACT_ADDRESS;
                    if (!pubkeys_equal(info->to, &sm_address)) {
                        THROW(ApduReplyAelfWrongSmartContractAddress);
                    } */
                }
                break;
            case 3:
                // ref_block_number (varint)
                { BAIL_IF(readVarInt(parser, (uint64_t*) &info->ref_block_number);); }
                break;
            case 4:
                // ref_block_prefix (string)
                {
                    readVarInt(parser, &info->ref_block_prefix.length);
                    BAIL_IF(parse_sized_string(parser, &info->ref_block_prefix));
                }
                break;
            case 5:
                // method_name (string)
                {
                    readVarInt(parser, &info->method_name.length);
                    BAIL_IF(parse_sized_string(parser, &info->method_name));
                    if (is_equal_string(&info->method_name, TRANSFER_METHOD_NAME)) {
                        THROW(ApduReplyAelfWrongMethodName);
                    }
                }
                break;
            case 6:
                // params (bytes)
                {
                    uint64_t length;
                    readVarInt(parser, &length);
                    parseParams(parser, info);
                }
                break;
            default:
                return 1;
        }
    }
    return 0;
}

int print_system_transfer_info(const SystemTransferInfo* info) {
    SummaryItem* item;

    item = transaction_summary_primary_item();
    summary_item_set_amount(item, "Transfer", info->amount);

    item = transaction_summary_general_item();
    summary_item_set_pubkey(item, "Recipient", info->dest);

    item = transaction_summary_general_item();
    summary_item_set_sized_string(item, "Memo", &info->memo);

    return 0;
}
