#ifndef __BITBOARD_PASTE_HH__
#define __BITBOARD_PASTE_HH__ 1

#include <string>
#include <string.h>

#include <stdint.h>

struct Paste {
  uint32_t magic;
  uint32_t ts;

  uint16_t w, h;
  uint16_t x, y;

  uint32_t rgb_len;
  uint8_t *rgb;

  uint8_t name_len;
  char *name;

  uint8_t nonce_len;
  uint8_t *nonce;

  uint32_t seen;
  uint8_t hash[32];

  Paste() {
    magic = 0;
    ts = 0;
    seen = 0;
    w = h = x = y = 0;
    rgb = NULL;
    rgb_len = 0;
    nonce = NULL;
    nonce_len = 0;
    name = NULL;
    name_len = 0;
  }

  ~Paste() {
    if (rgb)
      delete[] rgb;
    if (nonce)
      delete[] nonce;
    if (name)
      delete[] name;
  }

  void set_xy(uint32_t _x, uint32_t _y) {
    x = _x;
    y = _y;
  }

  void set_rgb_wh(uint8_t *_rgb, uint16_t _w, uint16_t _h) {
    if (rgb)
      delete[] rgb;
    rgb = NULL;
    w = _w;
    h = _h;
    rgb_len = w * h * 3;
    if (rgb_len == 0)
      return;

    rgb = new uint8_t[rgb_len];
    memcpy(rgb, _rgb, rgb_len);
  }

  void set_name(const char *_name) {
    if (name)
      delete[] name;
    name_len = strlen(_name);
    name = new char[name_len + 1];
    strcpy(name, _name);
  }

  void set_nonce(uint8_t *_nonce, uint8_t _nonce_len) {
    if (nonce)
      delete[] nonce;
    nonce = NULL;
    nonce_len = _nonce_len;
    if (nonce_len == 0)
      return;
    nonce = new uint8_t[nonce_len];
    memcpy(nonce, _nonce, nonce_len);
  }

  bool verify();
  void generate(double min_value = 0, time_t _ts = time(NULL));
  void compute_hash();
  void compute_hash0(void*);
  void compute_hash1(void*);
  double value(double halflife);
  double hval();

  void write(FILE *fp);
  void write(int fd);
  void read(FILE *fp);
  bool read(int fd);
};

#endif
