#ifndef __BITBOARD_BOARD_HH__
#define __BITBOARD_BOARD_HH__ 1

#include <string>

#include <stdint.h>
#include <math.h>

struct Cell {
  uint32_t off;
  uint32_t ts;
  double hval;

  double value(double halflife) {
    if (ts == 0)
      return 0;

    time_t now = time(NULL);
    double age = (now - ts) / halflife;
    return hval * pow(2.0, -age);
  }
};

struct SeenOff {
  uint32_t seen; 
  uint32_t off;
};

struct Board {
  std::string name;
  uint16_t w, h;
  double halflife;

  int ppm_fd;
  uint8_t *ppm, *rgb;

  int map_fd;
  Cell *map;

  int log_fd;

  int seenoff_fd;
  uint32_t seenoff_len;
  SeenOff *seenoff;

  Board(const char *dir) {
    ppm_fd = map_fd = log_fd = -1;
    seenoff_fd = -1;
    seenoff = NULL;
    ppm = rgb = NULL;
    map = NULL;

    open(dir);
  }

  static void create(const char *dir, const char *_name, uint32_t _w = 1024, uint32_t _h = 1024, double _halflife = 86400.0);

  unsigned int count(class Paste *p);
  unsigned int paste(Paste *p);
  Paste *get_paste(unsigned int x, unsigned int y);
  long find_off(uint32_t ts);

  void open(const char *dir);
};

#endif
