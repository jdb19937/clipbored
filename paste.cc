#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>

#include <math.h>

#include "paste.hh"

#include "sha256.h"

static void shup(SHA256_CTX &ctx, uint8_t x) {
  sha256_update(&ctx, (uint8_t *)&x, 1);
}
static void shup(SHA256_CTX &ctx, uint16_t x) {
  uint16_t nx = htons(x);
  sha256_update(&ctx, (uint8_t *)&nx, 2);
}
static void shup(SHA256_CTX &ctx, uint32_t x) {
  uint32_t nx = htonl(x);
  sha256_update(&ctx, (uint8_t *)&nx, 4);
}

void Paste::generate(double min_value, time_t _ts) {
  SHA256_CTX ctx0;

  ts = _ts;
  compute_hash0(&ctx0);

  uint8_t zero[16] = {0};
  set_nonce(zero, 16);

  do {
    for (int i = 0; i < nonce_len; ++i)
      nonce[i] = rand() % 256;

    SHA256_CTX ctx;
    memcpy(&ctx, &ctx0, sizeof(ctx));
    compute_hash1(&ctx);
  } while (hval() < min_value);
}

void Paste::compute_hash0(void *p) {
  SHA256_CTX &ctx(*(SHA256_CTX *)p);

  sha256_init(&ctx);
  shup(ctx, magic);
  shup(ctx, ts);
  shup(ctx, w); shup(ctx, h);
  shup(ctx, x); shup(ctx, y);

  shup(ctx, rgb_len);
  sha256_update(&ctx, (uint8_t *)rgb, rgb_len);

  shup(ctx, name_len);
  sha256_update(&ctx, (uint8_t *)name, name_len);
}

void Paste::compute_hash1(void *p) {
  uint8_t vhash[32];
  SHA256_CTX &ctx(*(SHA256_CTX *)p);
  shup(ctx, nonce_len);
  sha256_update(&ctx, (uint8_t *)nonce, nonce_len);

  sha256_final(&ctx, vhash);
  memcpy(hash, vhash, 32);
}

void Paste::compute_hash() {
  SHA256_CTX ctx;
  compute_hash0(&ctx);
  compute_hash1(&ctx);
}


static char hex[] = "0123456789ABCDEF";

double Paste::value(double halflife) {
  int i = 0;

  time_t now = time(NULL);
  if (ts > now)
    return 0;
  double age = (now - ts) / halflife;

  double v = hval();
  v *= pow(2, -age);

  return v;
}

double Paste::hval() {
  int i = 0;

  if (w == 0 || h == 0) {
    return 0;
  }

  char buf[256];
  strcpy(buf, "0x0.");
  char *p = buf + 4;
  while (i < 32) {
    *p++ = hex[hash[i] >> 4];
    *p++ = hex[hash[i] & 0xF];
    ++i;
  }
  *p = 0;

  double iv = strtod(buf, NULL);
  if (iv == 0)
    return 0;

  double v = 1.0 / iv;
  v /= (w * h);

  return v;
}


static std::string to_hex(uint8_t *hash, unsigned int n) {
  char *p0 = new char[2 * n + 1];
  char *p = p0;

  for (unsigned int i = 0; i < n; ++i) {
    *p++ = hex[hash[i] >> 4];
    *p++ = hex[hash[i] & 0xF];
  }
  *p = 0;
  std::string s(p0);
  delete[] p0;
  return s;
}

static void out(FILE *fp, const uint8_t &x) {
  int ret = fwrite(&x, sizeof(x), 1, fp);
  assert(ret == 1);
}
static void out(int fd, const uint8_t &x) {
  int ret = ::write(fd, &x, 1);
  assert(ret == 1);
}
static void out(FILE *fp, const uint16_t &x) {
  uint16_t nx = htons(x);
  int ret = fwrite(&nx, sizeof(nx), 1, fp);
  assert(ret == 1);
}
static void out(int fd, const uint16_t &x) {
  uint16_t nx = htons(x);
  int ret = ::write(fd, &nx, 2);
  assert(ret == 2);
}
static void out(FILE *fp, const uint32_t &x) {
  uint32_t nx = htonl(x);
  int ret = fwrite(&nx, sizeof(nx), 1, fp);
  assert(ret == 1);
}
static void out(int fd, const uint32_t &x) {
  uint32_t nx = htonl(x);
  int ret = write(fd, &nx, 4);
  assert(ret == 4);
}

static void in(FILE *fp, uint8_t *x) {
  uint8_t nx = 0;
  int ret = fread(&nx, sizeof(nx), 1, fp);
  assert(ret == 1);
  *x = nx;
}
static void in(int fd, uint8_t *x) {
  uint8_t nx = 0;
  int ret = ::read(fd, &nx, sizeof(nx));
  assert(ret == 1);
  *x = nx;
}


static void in(FILE *fp, uint16_t *x) {
  uint16_t nx = 0;
  int ret = fread(&nx, sizeof(nx), 1, fp);
  assert(ret == 1);
  *x = ntohs(nx);
}
static void in(int fd, uint16_t *x) {
  uint16_t nx = 0;
  int ret = ::read(fd, &nx, sizeof(nx));
  assert(ret == 2);
  *x = ntohs(nx);
}
static void in(FILE *fp, uint32_t *x) {
  uint32_t nx = 0;
  int ret = fread(&nx, sizeof(nx), 1, fp);
  assert(ret == 1);
  *x = ntohl(nx);
}
static void in(int fd, uint32_t *x) {
  uint32_t nx = 0;
  int ret = ::read(fd, &nx, sizeof(nx));
  assert(ret == 4);
  *x = ntohl(nx);
}

void Paste::write(FILE *fp) {
  int ret;

  out(fp, magic);
  out(fp, ts);

  out(fp, w); out(fp, h);
  out(fp, x); out(fp, y);

  out(fp, rgb_len);
  ret = fwrite(rgb, 1, rgb_len, fp);
  assert(ret == rgb_len);

  out(fp, name_len);
  ret = fwrite(name, 1, name_len, fp);
  assert(ret == name_len);

  out(fp, nonce_len);
  ret = fwrite(nonce, 1, nonce_len, fp);
  assert(ret == nonce_len);

  out(fp, seen);
}

void Paste::write(int fd) {
  int ret;

  out(fd, magic);
  out(fd, ts);

  out(fd, w); out(fd, h);
  out(fd, x); out(fd, y);

  out(fd, rgb_len);
  ret = ::write(fd, rgb, rgb_len);
  assert(ret == rgb_len);

  out(fd, name_len);
  ret = ::write(fd, name, name_len);
  assert(ret == name_len);

  out(fd, nonce_len);
  ret = ::write(fd, nonce, nonce_len);
  assert(ret == nonce_len);

  out(fd, seen);
}

bool Paste::read(FILE *fp) {
  int ret;

  do {
    ret = ::fread(&magic, 4, 1, fp);
    if (ret < 1) {
      return 0;
    }
    assert(ret == 1);
    magic = ntohl(magic);
  } while (magic == 0xFFFFFFFF);

  in(fp, &ts);

  in(fp, &w); in(fp, &h);
  in(fp, &x); in(fp, &y);

  in(fp, &rgb_len);
  assert(rgb_len == w * h * 3);
  rgb = new uint8_t[rgb_len];
  ret = fread(rgb, 1, rgb_len, fp);
  assert(ret == rgb_len);

  in(fp, &name_len);
  name = NULL;
  if (name_len > 0) {
    name = new char[name_len + 1];
    ret = fread((char *)name, 1, name_len, fp);
    assert(ret == name_len);
    name[name_len] = 0;
  }

  in(fp, &nonce_len);
  nonce = NULL;
  if (nonce_len > 0) {
    nonce = new uint8_t[nonce_len];
    ret = fread(nonce, 1, nonce_len, fp);
    assert(ret == nonce_len);
  }

  in(fp, &seen);
}

bool Paste::read(int fd) {
  int ret;

  do {
    ret = ::read(fd, &magic, 4);
    if (ret < 1) {
      return 0;
    }
    assert(ret == 4);
    magic = ntohl(magic);
  } while (magic == 0xFFFFFFFF);

  in(fd, &ts);

  in(fd, &w); in(fd, &h);
  in(fd, &x); in(fd, &y);

  in(fd, &rgb_len);
  assert(rgb_len == w * h * 3);
  rgb = new uint8_t[rgb_len];
  ret = ::read(fd, rgb, rgb_len);
  assert(ret == rgb_len);

  in(fd, &name_len);
  name = NULL;
  if (name_len > 0) {
    name = new char[name_len + 1];
    ret = ::read(fd, (char *)name, name_len);
    assert(ret == name_len);
    name[name_len] = 0;
  }

  in(fd, &nonce_len);
  nonce = NULL;
  if (nonce_len > 0) {
    nonce = new uint8_t[nonce_len];
    ret = ::read(fd, nonce, nonce_len);
    assert(ret == nonce_len);
  }

  in(fd, &seen);

  return true;
};
