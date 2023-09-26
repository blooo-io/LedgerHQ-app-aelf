#include "apdu.h"
#include "getPubkey.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "aelf/printer.h"

static uint8_t G_publicKey[PUBKEY_LENGTH];
static char G_addressStr[BASE58_CHECK_PUBKEY_LENGTH];

void reset_getpubkey_globals(void) {
    MEMCLEAR(G_publicKey);
    MEMCLEAR(G_addressStr);
}

int find_address_length() {
    int i;
    for (i = 0; G_addressStr[i] != '\0' && i < BASE58_CHECK_PUBKEY_LENGTH - 1; ++i)
        ;
    return i;
}

static uint8_t set_result_get_pubkey() {
    uint32_t tx = 0;
    G_io_apdu_buffer[tx++] = PUBKEY_LENGTH;
    memmove(G_io_apdu_buffer + tx, G_publicKey, PUBKEY_LENGTH);
    tx += PUBKEY_LENGTH;

    int address_length = find_address_length();

    G_io_apdu_buffer[tx++] = address_length;
    memmove(G_io_apdu_buffer + tx, G_addressStr, address_length);
    tx += address_length;

    return tx;
}

//////////////////////////////////////////////////////////////////////

UX_STEP_NOCB(ux_display_public_flow_5_step,
             bnnn_paging,
             {
                 .title = "Address",
                 .text = G_addressStr,
             });
UX_STEP_CB(ux_display_public_flow_6_step,
           pb,
           sendResponse(set_result_get_pubkey(), true),
           {
               &C_icon_validate_14,
               "Approve",
           });
UX_STEP_CB(ux_display_public_flow_7_step,
           pb,
           sendResponse(0, false),
           {
               &C_icon_crossmark,
               "Reject",
           });

UX_FLOW(ux_display_public_flow,
        &ux_display_public_flow_5_step,
        &ux_display_public_flow_6_step,
        &ux_display_public_flow_7_step);

void handle_get_pubkey(volatile unsigned int *flags, volatile unsigned int *tx) {
    if (!flags || !tx || (G_command.instruction != InsGetPubkey) ||
        G_command.state != ApduStatePayloadComplete) {
        THROW(ApduReplySdkInvalidParameter);
    }

    get_public_key(G_publicKey, G_command.derivation_path, G_command.derivation_path_length);
    compute_address(G_publicKey, PUBKEY_LENGTH, G_addressStr, BASE58_CHECK_PUBKEY_LENGTH);

    if (G_command.non_confirm) {
        *tx = set_result_get_pubkey();
        THROW(ApduReplySuccess);
    } else {
        ux_flow_init(0, ux_display_public_flow, NULL);
        *flags |= IO_ASYNCH_REPLY;
    }
}
