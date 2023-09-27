from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID
from ragger.utils import RAPDU

from .apps.aelf import AelfClient, ErrorType
from .apps.aelf_cmd_builder import verify_signature
from .apps.aelf_utils import FOREIGN_PUBLIC_KEY, FOREIGN_PUBLIC_KEY_2, CHAIN_PUBLIC_KEY, AMOUNT, AMOUNT_2, TICKER, REF_BLOCK_NUMBER, METHOD_NAME, ELF_PACKED_DERIVATION_PATH, ELF_PACKED_DERIVATION_PATH_2

from .utils import ROOT_SCREENSHOT_PATH

def test_aelf_get_public_key_ok(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)

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

    assert aelf.get_async_response().data == from_public_key

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

def test_aelf_long_memo_transfer_txn_ok(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)
    # Create instruction
    message: bytes = bytearray.fromhex("0a220a20d736f33c7c2a35a04603fd3e94d5395c29edd9ac159bed03e366c8fb700b7d1812220a202791e992a57f28e75a11f13af2c0aec8b0eb35d2f048d42eba8901c92e0378dc18b1e8fb552204b05922512a085472616e7366657232710a220a204ff4e63ad4aa7ec92e65ba2d37b2c56b3f82390bfc25e66cebab6821f3b05c0b1203454c461880d4dbd20f22404c6f72656d20497073756d2069732073696d706c792064756d6d792074657874206f6620746865207072696e74696e6720616e64207479706573657474696e67")

    with aelf.send_async_sign_transfer(ELF_PACKED_DERIVATION_PATH_2, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)
    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)

def test_aelf_simple_transfer_43_elf_txn_ok(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH_2)
    # Create instruction
    txHex43Elf: str = "0a220a20d736f33c7c2a35a04603fd3e94d5395c29edd9ac159bed03e366c8fb700b7d1812220a202791e992a57f28e75a11f13af2c0aec8b0eb35d2f048d42eba8901c92e0378dc18aaccfb5522049d78113e2a085472616e73666572323c0a220a204ff4e63ad4aa7ec92e65ba2d37b2c56b3f82390bfc25e66cebab6821f3b05c0b1203454c46188096b38210220b612074657374206d656d6f"
    message: bytes = bytearray.fromhex(txHex43Elf)
    print(list(message))
    with aelf.send_async_sign_transfer(ELF_PACKED_DERIVATION_PATH_2, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)
    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)