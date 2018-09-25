#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <math.h>

#include "paste.hh"
#include "board.hh"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: pastein board.dir < pastes\n");
    return 1;
  }

  const char *dir = argv[1];
  Board b(dir);

  while (1) {
    Paste p;

    int c = getc(stdin);
    if (c == -1)
      break;
    ungetc(c, stdin);

    p.read(stdin);
    if (strcmp(p.name, b.name.c_str())) {
      fprintf(stderr, "wrong boardname %s (should be %s)\n", p.name, b.name.c_str());
      continue;
    }

    p.compute_hash();

    unsigned int count = b.paste(&p);
fprintf(stderr, "updated %u pixels\n", count);
  }

  return 0;
}

