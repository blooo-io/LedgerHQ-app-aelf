#pragma once

#include "aelf/parser.h"
#include "aelf/print_config.h"

#define TICKER_ELF           "ELF"
#define TRANSFER_METHOD_NAME "Transfer"
#define SMART_CONTRACT_ADDRESS                                                                  \
    {                                                                                           \
        {                                                                                       \
            0x27, 0x91, 0xe9, 0x92, 0xa5, 0x7f, 0x28, 0xe7, 0x5a, 0x11, 0xf1, 0x3a, 0xf2, 0xc0, \
                0xae, 0xc8, 0xb0, 0xeb, 0x35, 0xd2, 0xf0, 0x48, 0xd4, 0x2e, 0xba, 0x89, 0x01,   \
                0xc9, 0x2e, 0x03, 0x78, 0xdc                                                    \
        }                                                                                       \
    }

typedef struct SystemTransferInfo {
    const Pubkey* from;
    const Pubkey* to;
    uint32_t ref_block_number;
    SizedString ref_block_prefix;
    SizedString method_name;
    const Pubkey* dest;
    SizedString ticker;
    uint64_t amount;
    SizedString memo;
} SystemTransferInfo;

typedef struct InstructionInfo {
    union {
        SystemTransferInfo transfer;
    };
} InstructionInfo;

int parse_system_transfer_instruction(Parser* parser, SystemTransferInfo* info);

int print_system_transfer_info(const SystemTransferInfo* info);