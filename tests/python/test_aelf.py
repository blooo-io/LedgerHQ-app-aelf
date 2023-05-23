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