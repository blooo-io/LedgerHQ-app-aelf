#include "instruction.h"
#include "sol/parser.h"
#include "sol/message.h"
#include "sol/print_config.h"
#include "util.h"
#include <string.h>

// change this if you want to be able to add succesive tx
#define MAX_INSTRUCTIONS 1

int process_message_body(const uint8_t* message_body, int message_body_length, int ins_code) {
    size_t instruction_count = 0;
    InstructionInfo instruction_info[MAX_INSTRUCTIONS];
    explicit_bzero(instruction_info, sizeof(InstructionInfo) * MAX_INSTRUCTIONS);
    size_t display_instruction_count = 0;
    InstructionInfo* display_instruction_info[MAX_INSTRUCTIONS];

    // init parser body
    Parser parser = {message_body, message_body_length};
    Instruction instruction;
    InstructionInfo* info = &instruction_info[instruction_count];

    switch (ins_code) {
        case 3:  // TRANSFER
            parse_system_transfer_instruction(&parser, &instruction, &info->transfer);
            break;
    };

    display_instruction_info[display_instruction_count++] = info;
    // Ensure we've consumed the entire message body
    BAIL_IF(!parser_is_empty(&parser));

    switch (ins_code) {
        case 3:  // TRANSFER
            return print_system_transfer_info(&display_instruction_info[0]->transfer);
    };
    return 1;
}