from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID
from ragger.utils import RAPDU

from .apps.aelf import AelfClient, ErrorType
from .apps.aelf_cmd_builder import verify_signature
from .apps.aelf_utils import pubkey_to_address, FOREIGN_PUBLIC_KEY, FOREIGN_PUBLIC_KEY_2, CHAIN_PUBLIC_KEY, AMOUNT, AMOUNT_2, TICKER, REF_BLOCK_NUMBER, METHOD_NAME, ELF_PACKED_DERIVATION_PATH, ELF_PACKED_DERIVATION_PATH_2

from .utils import ROOT_SCREENSHOT_PATH

def test_aelf_get_public_key_ok(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)
    address = pubkey_to_address(from_public_key)

    with aelf.send_public_key_with_confirm(ELF_PACKED_DERIVATION_PATH_2):
        if backend.firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                        [NavInsID.BOTH_CLICK],
                                                        "Approve",
                                                        ROOT_SCREENSHOT_PATH,
                                                        test_name)
        else:
            instructions = [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS
            ]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                            test_name,
                                            instructions)

    assert aelf.get_async_response().data == b'\x41' + from_public_key + len(address).to_bytes(1, 'big') + address

def test_aelf_simple_transfer_txn_ok(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)
    # Create instruction
    message: bytes = bytearray.fromhex("0a220a20cdefe728493133f526cb5e97e2ec339ca219bef80427eaf53ff0003cad241c7a12220a202791e992a57f28e75a11f13af2c0aec8b0eb35d2f048d42eba8901c92e0378dc188dcda24b22045a7447382a085472616e7366657232340a220a204ff4e63ad4aa7ec92e65ba2d37b2c56b3f82390bfc25e66cebab6821f3b05c0b1203454c461880ade204220474657374")
    print(list(message))
    with aelf.send_async_sign_transfer(ELF_PACKED_DERIVATION_PATH_2, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)
    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)
