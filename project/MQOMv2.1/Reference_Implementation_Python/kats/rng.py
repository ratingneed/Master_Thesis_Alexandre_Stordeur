import hashlib
import math
import binascii
# pip install pycryptodomex
# pip install pycryptodome
from Crypto.Cipher import AES

AES256_CTR_DRBG_struct = { 'Key' : bytearray([0] * 32), 'V' : bytearray([0] * 16), 'reseed_counter' : 0 }

def AES256_CTR_DRBG_Update(provided_data=None):
    temp = bytearray([0] * 48)
    Key = AES256_CTR_DRBG_struct['Key']
    V = AES256_CTR_DRBG_struct['V']
    cipher = AES.new(Key, AES.MODE_ECB)
    for i in range(3):
        for j in range(15, -1, -1):
            if V[j] == 0xff:
                V[j] = 0x00
            else:
                V[j] += 1
                break
        temp[16*i:16*(i+1)] = cipher.encrypt(V)
    if provided_data is not None:
        for i in range(48):
            temp[i] ^= provided_data[i]
    AES256_CTR_DRBG_struct['Key'] = temp[:32]
    AES256_CTR_DRBG_struct['V'] = temp[32:]


def randombytes_init(entropy_input, personalization_string=None, security_strength=None):
    seed_material = entropy_input[:48]
    if personalization_string is not None:
        for i in range(48):
            seed_material[i] = seed_material[i] ^ personalization_string[i]
    AES256_CTR_DRBG_struct['Key'] = bytearray([0] * 32)
    AES256_CTR_DRBG_struct['V'] = bytearray([0] * 16)
    AES256_CTR_DRBG_Update(seed_material)
    AES256_CTR_DRBG_struct['reseed_counter'] = 1

def randombytes(xlen):
    x = bytearray([0] * xlen)
    block = bytearray([0] * 16)
    i = 0
    cipher = AES.new(AES256_CTR_DRBG_struct['Key'], AES.MODE_ECB)
    while xlen > 0:
        for j in range(15, -1, -1):
            if AES256_CTR_DRBG_struct['V'][j] == 0xff:
                AES256_CTR_DRBG_struct['V'][j] = 0x00
            else:
                AES256_CTR_DRBG_struct['V'][j] += 1
                break
        block = cipher.encrypt(AES256_CTR_DRBG_struct['V'])
        if xlen > 15:
            x[i:i+16] = block
            i += 16
            xlen -= 16
        else:
            x[i:i+xlen] = block[:xlen]
            xlen = 0
    AES256_CTR_DRBG_Update()
    AES256_CTR_DRBG_struct['reseed_counter'] += 1
    return bytes(x)
