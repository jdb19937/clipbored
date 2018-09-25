#include "board.hh"
#include "paste.hh"

#include <stdio.h>

#include <set>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: dump boardname\n");
    return 1;
  }

  std::string name = argv[1];
  Board board(name.c_str(), 1024, 1024);

  board.open_map();
  board.open_pastes();

  std::set<Paste*> ps;
  for (unsigned int y = 0; y < board.h; ++y) {
    for (unsigned int x = 0; x < board.w; ++x) {
      ps.insert(board.pastes + board.map[y * board.w + x]);
    }
  }

  for (auto psi = ps.begin(); psi != ps.end(); ++psi) {
    (*psi)->write_tsv(stdout);
  }

  return 0;
}
