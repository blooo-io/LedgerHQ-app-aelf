#pragma once

#include "sol/parser.h"
#include "sol/print_config.h"

typedef struct SystemTransferInfo {
    const Pubkey* from;
    const Pubkey* to;
    uint64_t amount;
} SystemTransferInfo;

typedef struct SystemGetTxResultInfo {
    const Pubkey* from;
    const Pubkey* chain;
    uint64_t ref_block_number;
    const Pubkey* to;
    uint64_t amount;
    SizedString method_name;
} SystemGetTxResultInfo;

typedef struct InstructionInfo {
    union {
        SystemTransferInfo transfer;
        SystemGetTxResultInfo getTxResult;
    };
} InstructionInfo;

int parse_system_transfer_instruction(Parser* parser,
                                      Instruction* instruction,
                                      SystemTransferInfo* info);

int print_system_transfer_info(const SystemTransferInfo* info);