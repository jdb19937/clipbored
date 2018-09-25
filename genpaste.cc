#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <math.h>

#include <string>

#include "paste.hh"
#include "board.hh"

static std::string read_word(FILE *fp) {
  std::string x;

  while (1) {
    int c = getc(fp);
    if (c == -1)
      return x;
    if (isspace(c))
      break;
    char cc = c;
    x += std::string(&cc, 1);
  }

#if 0
  while (1) {
    int c = getc(fp);
    if (c == -1)
      return x;
    if (!isspace(c)) {
      ungetc(c, fp);
      return x;
    }
  }
#endif

  return x;
}

int main(int argc, char **argv) {
  double target = 10.0;

  if (argc < 4) {
    fprintf(stderr, "Usage: genpaste board x y [target] < file.ppm\n");
    return 1;
  }

  const char *boardname = argv[1];
  unsigned int x = strtoul(argv[2], NULL, 0);
  unsigned int y = strtoul(argv[3], NULL, 0);
  if (argc >= 5) {
    target = strtod(argv[4], NULL);
    assert(target >= 0);
  }

  assert(read_word(stdin) == "P6");
  std::string wstr = read_word(stdin);
  std::string hstr = read_word(stdin);
  assert(read_word(stdin) == "255");

  uint32_t w = strtoul(wstr.c_str(), NULL, 10);
  uint32_t h = strtoul(hstr.c_str(), NULL, 10);

  uint8_t *rgb = new uint8_t[w * h * 3];
  int ret = fread(rgb, 1, w * h * 3, stdin);
  assert(ret == w * h * 3);

  Paste p;
  p.set_name(boardname);
  p.set_rgb_wh(rgb, w, h);
  p.set_xy(x, y);

  p.generate(target);
  p.write(stdout);
}

