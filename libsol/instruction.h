#pragma once

#include "sol/parser.h"
#include "sol/print_config.h"

typedef struct SystemTransferInfo {
    const Pubkey* from;
    const Pubkey* to;
    uint64_t ref_block_number;
    uint8_t* ref_block_prefix;
    uint8_t* method_name;
    const Pubkey* dest;
    uint8_t* ticker;
    uint64_t amount;
    uint8_t* memo;
    uint8_t* signature;
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

// This symbol is defined by the link script to be at the start of the stack
// area.
extern unsigned long _stack;

#define STACK_CANARY (*((volatile uint32_t*) &_stack))

void init_canary();

void check_canary();

int parse_system_transfer_instruction(Parser* parser,
                                      Instruction* instruction,
                                      SystemTransferInfo* info);

int parse_system_get_tx_result_instruction(Parser* parser,
                                           Instruction* instruction,
                                           SystemGetTxResultInfo* info);

int print_system_transfer_info(const SystemTransferInfo* info);