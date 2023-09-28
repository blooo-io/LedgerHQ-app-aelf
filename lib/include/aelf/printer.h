#pragma once

#include "aelf/parser.h"
#include <stdint.h>
#include <stddef.h>

#define SUMMARY_LENGTH             7
#define TITLE_SIZE                 32
#define BASE58_PUBKEY_LENGTH       45
#define BASE58_CHECK_PUBKEY_LENGTH 51
#define BASE58_PUBKEY_SHORT        (SUMMARY_LENGTH + 2 + SUMMARY_LENGTH + 1)
#define MEMO_LENGTH                65  // 64 + \0

int print_token_amount(uint64_t amount,
                       const char *asset,
                       uint8_t decimals,
                       char *out,
                       size_t out_length);

int print_amount(uint64_t amount, char *out, size_t out_length);

int print_i64(int64_t i64, char *out, size_t out_length);

int print_u64(uint64_t u64, char *out, size_t out_length);

int print_sized_string(const SizedString *string, char *out, size_t out_length);

int print_string(const char *in, char *out, size_t out_length);

int print_pubkey(const uint8_t *in, char *out, size_t out_length);

int compute_address(const uint8_t *pubkey,
                    size_t pubkey_length,
                    char *address,
                    size_t address_length);

int print_summary(const char *in,
                  char *out,
                  size_t out_length,
                  size_t left_length,
                  size_t right_length);

int print_timestamp(int64_t, char *out, size_t out_length);

int encode_base58(const void *in, size_t length, char *out, size_t maxoutlen);

int encode_base58_check(const void *in, size_t length, char *out, size_t maxoutlen);