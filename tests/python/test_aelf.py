from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID
from ragger.utils import RAPDU

from .apps.aelf import AelfClient, ErrorType
from .apps.aelf_cmd_builder import SystemInstructionTransfer, SystemInstructionGetTxResult, MessageTransfer, MessageTxResult, verify_signature
from .apps.aelf_utils import FOREIGN_PUBLIC_KEY, FOREIGN_PUBLIC_KEY_2, CHAIN_PUBLIC_KEY, AMOUNT, AMOUNT_2, TICKER, REF_BLOCK_NUMBER, METHOD_NAME, ELF_PACKED_DERIVATION_PATH, ELF_PACKED_DERIVATION_PATH_2

from .utils import ROOT_SCREENSHOT_PATH

def test_aelf_simple_transfer_ok_1(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH)
    # Create instruction
    instruction: SystemInstructionTransfer = SystemInstructionTransfer(FOREIGN_PUBLIC_KEY, TICKER, AMOUNT)
    message: bytes = MessageTransfer(instruction).serialize()

    with aelf.send_async_sign_transfer(ELF_PACKED_DERIVATION_PATH, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)
    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)


def test_aelf_get_tx_result_ok(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)

    # Create instruction
    instruction: SystemInstructionGetTxResult = SystemInstructionGetTxResult(from_public_key, CHAIN_PUBLIC_KEY, REF_BLOCK_NUMBER, METHOD_NAME, FOREIGN_PUBLIC_KEY_2, TICKER, AMOUNT_2)
    message: bytes = MessageTxResult(instruction).serialize()

    with aelf.send_async_get_tx_result(ELF_PACKED_DERIVATION_PATH_2, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)

    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)


def test_aelf_simple_transfer_txn_ok_2(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)
    # Create instruction
    message: bytes = bytearray.fromhex("0a220a20cdefe728493133f526cb5e97e2ec339ca219bef80427eaf53ff0003cad241c7a12220a202791e992a57f28e75a11f13af2c0aec8b0eb35d2f048d42eba8901c92e0378dc188dcda24b22045a7447382a085472616e7366657232340a220a204ff4e63ad4aa7ec92e65ba2d37b2c56b3f82390bfc25e66cebab6821f3b05c0b1203454c461880ade204220474657374")

    with aelf.send_async_sign_transfer(ELF_PACKED_DERIVATION_PATH_2, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ELF_PACKED_DERIVATION_PATH_2,
                                                  test_name)
    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)
