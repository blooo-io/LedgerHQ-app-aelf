from typing import List
from enum import IntEnum
import base58
from nacl.signing import VerifyKey

# Fake blockhash so this example doesn't need a network connection. It should be queried from the cluster in normal use.
FAKE_RECENT_BLOCKHASH = "11111111111111111111111111111111"


def verify_signature(from_public_key: bytes, message: bytes, signature: bytes):
    assert len(signature) == 64, "signature size incorrect"
    verify_key = VerifyKey(from_public_key)
    verify_key.verify(message, signature)


class SystemInstruction(IntEnum):
    CreateAccount           = 0x00
    Assign                  = 0x01
    Transfer                = 0x02
    # GetTxResult             = 0x03

# Only support Transfer instruction for now
# TODO add other instructions if the need arises
class Instruction:
    data: bytes
    to_pubkey: bytes
    ticker: bytes
    def serialize(self) -> bytes:
        serialized: bytes = self.to_pubkey
        serialized += len(self.ticker).to_bytes(1, byteorder='little')
        serialized += self.ticker
        serialized += self.data
        return serialized

class SystemInstructionTransfer(Instruction):
    def __init__(self, to_pubkey: bytes, ticker: bytes, amount: bytes):
        self.data =  (amount).to_bytes(8, byteorder='little')
        self.to_pubkey = to_pubkey
        self.ticker = ticker
class MessageTransfer:
    recent_blockhash: bytes
    instruction: Instruction

    def __init__(self, instruction: Instruction):
        self.recent_blockhash = base58.b58decode(FAKE_RECENT_BLOCKHASH)
        self.instruction = instruction

    def serialize(self) -> bytes:
        serialized: bytes = self.recent_blockhash
        serialized += self.instruction.serialize()
        return serialized