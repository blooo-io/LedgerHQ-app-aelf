from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID
from ragger.utils import RAPDU

from .apps.aelf import AelfClient, ErrorType
from .apps.aelf_cmd_builder import SystemInstructionTransfer, MessageTransfer, verify_signature
from .apps.aelf_utils import FOREIGN_PUBLIC_KEY, AMOUNT, TICKER, ELF_PACKED_DERIVATION_PATH

from .utils import ROOT_SCREENSHOT_PATH

def test_aelf_simple_transfer_ok_1(backend, navigator, test_name):
    aelf = AelfClient(backend)
    from_public_key = aelf.get_public_key(ELF_PACKED_DERIVATION_PATH)
    # Create instruction
    # instruction: SystemInstructionTransfer = SystemInstructionTransfer(FOREIGN_PUBLIC_KEY, TICKER, AMOUNT)
    # message: bytes = MessageTransfer(instruction).serialize()
    message = bytearray.fromhex("e00301006b010a220a20cdefe728493133f526cb5e97e2ec339ca219bef80427eaf53ff0003cad241c7a12220a202791e992a57f28e75a11f13af2c0aec8b0eb35d2f048d42eba8901c92e0378dc188dcda24b22045a7447382a085472616e7366657232340a220a204ff4e63ad4aa7ec92e65ba2d37b2c56b3f82390bfc25e66cebab6821f3b05c0b1203454c461880ade20422047465737482f10441f082e6da058f7a3df46ad639e6aa7ca4f6e38c87bdc2bd96234fe198b530b4e020c1a3ab04ed92c20571ce14a33724b8fad31d19fce0b1eaaf3cf3a14e60df7d01")
    with aelf.send_async_sign_transfer(ELF_PACKED_DERIVATION_PATH, message):
        navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                  [NavInsID.BOTH_CLICK],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)
    assert False
    signature: bytes = aelf.get_async_response().data

    verify_signature(from_public_key, message, signature)