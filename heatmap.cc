#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <math.h>

#include "paste.hh"
#include "board.hh"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: heatmap board.dir > heatmap.ppm\n");
    return 1;
  }

  const char *dir = argv[1];
  Board b(dir);

  setbuf(stdout, NULL);
  printf("P6\n%u %u\n255\n", b.w, b.h);

  for (int y = 0; y < b.h; ++y) {
    for (int x = 0; x < b.w; ++x) {
      uint8_t red = 0;
      double v = b.map[y * b.w + x].value(b.halflife);
      if (v > 1) {
        double lv = 5 * log(v) / log(2);
        if (lv < 0)
          lv = 0;
        if (lv > 255)
          lv = 255;
        red = (uint8_t)lv;
      }
      uint8_t buf[3] = {red, 0, 0};
      int ret = fwrite(buf, 1, 3, stdout);
      assert(ret == 3);
    }
  }

  return 0;
}

