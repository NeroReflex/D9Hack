#include "sha.h"

void sha_init(u32 mode)
{
    while(*REG_SHACNT & 1);
    *REG_SHACNT = mode | SHA_CNT_OUTPUT_ENDIAN | SHA_NORMAL_ROUND;
}

void sha_update(const void* src, u32 size)
{
    const u32* src32 = (const u32*)src;

    while(size >= 0x40) {
        while(*REG_SHACNT & 1);
        for(u32 i = 0; i < 4; i++) {
            *REG_SHAINFIFO = *src32++;
            *REG_SHAINFIFO = *src32++;
            *REG_SHAINFIFO = *src32++;
            *REG_SHAINFIFO = *src32++;
        }
        size -= 0x40;
    }
    while(*REG_SHACNT & 1);
    memcpy((void*)REG_SHAINFIFO, src32, size);
}

void sha_get(void* res) {
    *REG_SHACNT = (*REG_SHACNT & ~SHA_NORMAL_ROUND) | SHA_FINAL_ROUND;
    while(*REG_SHACNT & SHA_FINAL_ROUND);
    while(*REG_SHACNT & 1);
    memcpy(res, (void*)REG_SHAHASH, (256 / 8));
}

void sha_quick(void* res, const void* src, u32 size, u32 mode) {
    sha_init(mode);
    sha_update(src, size);
    sha_get(res);
}

u8 hex2text(u8 nibble) {
  nibble = nibble & 0x0F;

  if (nibble == 0x00) return '0';
  if (nibble == 0x01) return '1';
  if (nibble == 0x02) return '2';
  if (nibble == 0x03) return '3';
  if (nibble == 0x04) return '4';
  if (nibble == 0x05) return '5';
  if (nibble == 0x06) return '6';
  if (nibble == 0x07) return '7';
  if (nibble == 0x08) return '8';
  if (nibble == 0x09) return '9';
  if (nibble == 0x0a) return 'a';
  if (nibble == 0x0b) return 'b';
  if (nibble == 0x0c) return 'c';
  if (nibble == 0x0d) return 'd';
  if (nibble == 0x0e) return 'e';
  if (nibble == 0x0f) return 'f';
}

u8 text2hex(u8 ch) {
  if (ch == '0') return 0x00;
  if (ch == '1') return 0x01;
  if (ch == '2') return 0x02;
  if (ch == '3') return 0x03;
  if (ch == '4') return 0x04;
  if (ch == '5') return 0x05;
  if (ch == '6') return 0x06;
  if (ch == '7') return 0x07;
  if (ch == '8') return 0x08;
  if (ch == '9') return 0x09;
  if (ch == 'a') return 0x0a;
  if (ch == 'b') return 0x0b;
  if (ch == 'c') return 0x0c;
  if (ch == 'd') return 0x0d;
  if (ch == 'e') return 0x0e;
  if (ch == 'f') return 0x0f;
}

void sha_hex2text(u8* hash, u8 hashLength, u8* out) {
  for (u8 i = 0; i < hashLength; i++) {
    out[i*2] = hex2text((hash[i] >> 4) & 0x0F);
    out[(i*2)+1] = hex2text(hash[i] & 0x0F);
  }
}

void sha_text2sha(u8* hash, u8 hashLength, u8* in) {
  for (u8 i = 0; i < hashLength; i++)
    hash[i] = text2hex(in[i * 2]) | (text2hex(in[(i * 2)+1]) << 4);
}
