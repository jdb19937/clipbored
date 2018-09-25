#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <math.h>

#include <set>

#include "paste.hh"
#include "board.hh"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: recompress board.dir > pastes\n");
    return 1;
  }

  const char *dir = argv[1];
  Board b(dir);

  std::set<uint32_t> visible;
  for (int y = 0; y < b.h; ++y) {
    for (int x = 0; x < b.w; ++x) {
      uint32_t off = b.map[y * b.w + x].off;
      if (off)
        visible.insert(off);
    }
  }

  for (auto i = visible.begin(); i != visible.end(); ++i) {
 //  fprintf(stderr, "off=%u\n", *i);
    int ret = lseek(b.log_fd, *i, SEEK_SET);
    assert(ret == *i);
    Paste p;
    p.read(b.log_fd);
    p.write(stdout);
  }

  return 0;
}

