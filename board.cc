#include "board.hh"
#include "paste.hh"

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

void Board::create(const char *dir, const char *name, uint32_t w, uint32_t h, double halflife) {
  int ret = mkdir(dir, 0700);
  assert(ret == 0);

  assert(!strchr(name, '\t'));

  std::string fn = std::string(dir) + "/config.tsv";
  FILE *fp = fopen(fn.c_str(), "w");
  fprintf(fp, "%s\t%u\t%u\t%lf\n", name, w, h, halflife);
  fclose(fp);
}

static std::string read_colval(FILE *fp) {
  std::string x;

  while (1) {
    int c = getc(fp);
    if (c == -1 || c == '\n' || c == '\t')
      return x;
    char cc = c;
    x += std::string(&cc, 1);
  }
}

void Board::open(const char *dir) {
  std::string config_fn = std::string(dir) + "/config.tsv";
  FILE *config_fp = fopen(config_fn.c_str(), "r");
  assert(config_fp);
  name = read_colval(config_fp);
  std::string wstr = read_colval(config_fp);
  w = strtoul(wstr.c_str(), NULL, 0);
  std::string hstr = read_colval(config_fp);
  h = strtoul(hstr.c_str(), NULL, 0);
  std::string halflifestr = read_colval(config_fp);
  halflife = strtoul(halflifestr.c_str(), NULL, 0);

  std::string ppm_fn = std::string(dir) + "/render.ppm";
fprintf(stderr, "opening %s\n", ppm_fn.c_str());
  ppm_fd = ::open(ppm_fn.c_str(), O_RDWR | O_CREAT, 0777);
  assert(ppm_fd != -1);

  char hdr[256];
  sprintf(hdr, "P6\n%u %u\n255\n", w, h);
  int hdr_len = strlen(hdr);
  int ret = write(ppm_fd, hdr, hdr_len);
  assert(ret == hdr_len);

  assert(w % 2 == 0);
  assert(h % 2 == 0);

  size_t length = hdr_len + (w * h * 3);
  assert(0 == ftruncate(ppm_fd, length));
  length = (length + 4095) & ~4095;
  ppm = (uint8_t *)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, ppm_fd, 0);
  assert(ppm != NULL);
  assert(ppm != MAP_FAILED);

  rgb = ppm + hdr_len;


  //map = (uint32_t *)((uint8_t *)rgb + w * h * 3);
  //log = (uint8_t *)((uint8_t *)map + w * h * 4);
  std::string map_fn = std::string(dir) + "/map.dat";
fprintf(stderr, "opening %s\n", map_fn.c_str());
  map_fd = ::open(map_fn.c_str(), O_RDWR | O_CREAT, 0777);
  assert(map_fd != -1);

  length = (w * h * sizeof(Cell));
  assert(0 == ftruncate(map_fd, length));
  length = (length + 4095) & ~4095;
  map = (Cell *)mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, 0);
  assert(map != NULL);
  assert(map != MAP_FAILED);

  std::string log_fn = std::string(dir) + "/log.dat";
fprintf(stderr, "opening %s\n", log_fn.c_str());
  log_fd = ::open(log_fn.c_str(), O_RDWR | O_CREAT, 0777);
  assert(log_fd != -1);

  ret = lseek(log_fd, 0, SEEK_END);
  if (ret == 0) {
    ret = lseek(log_fd, 0, SEEK_SET);
    assert(ret == 0);

    Paste p;
    p.set_name(name.c_str());
    p.set_rgb_wh(NULL, 0, 0);
    p.set_xy(0, 0);
    p.generate(0.0, 0);

    p.write(log_fd);
  }

  std::string seenoff_fn = std::string(dir) + "/seenoff.dat";
  seenoff_fd = ::open(seenoff_fn.c_str(), O_RDWR | O_CREAT, 0777);
  assert(seenoff_fd != -1);
  ret = lseek(seenoff_fd, 0, SEEK_END);
  assert(ret != -1);
  seenoff_len = ret;
  assert(seenoff_len % sizeof(SeenOff) == 0);

  ret = lseek(seenoff_fd, 0, SEEK_SET);
  assert(ret == 0);

  seenoff_len = (seenoff_len + 4095) & ~4095;
  if (seenoff_len > 0) {
    seenoff = (SeenOff *)mmap(NULL, seenoff_len, PROT_READ | PROT_WRITE, MAP_SHARED, seenoff_fd, 0);
    assert(seenoff != MAP_FAILED && seenoff != NULL);
  } else {
    seenoff = NULL;
  }

  ret = lseek(seenoff_fd, 0, SEEK_END);
  assert(ret != -1);
}

long Board::find_off(uint32_t ts) {
  int ret = ::lseek(seenoff_fd, 0, SEEK_END);
  assert(ret >= 0);
  assert(ret % sizeof(SeenOff) == 0);
  int n = (ret / sizeof(SeenOff));

  int a = 0, b = n;

  while (a < b) {
    int mid = a + (b-a)/2;

    if (ts <= seenoff[mid].seen)
      b = mid;
    else
      a = mid + 1;
  }
  assert(a == b);
  assert(0 <= a);
  assert(a <= n);

  if (a == n) {
    return -1;
  }

  return seenoff[a].off;
}

unsigned int Board::count(Paste *p) {
  double pv = p->value(halflife);
  unsigned int count = 0;

  for (uint32_t x = p->x; x < p->x + p->w; ++x) {
    for (uint32_t y = p->y; y < p->y + p->h; ++y) {
      double qv = map[y * w + x].value(halflife);
      if (pv > qv)
        ++count;
    }
  }

  return count;
}

unsigned int Board::paste(Paste *p) {
  double phv = p->hval();
  double pv = p->value(halflife);
  unsigned int count = 0;

  size_t poff = lseek(log_fd, 0, SEEK_END);
  assert(poff > 0);

  for (uint32_t y = p->y; y < p->y + p->h; ++y) {
    for (uint32_t x = p->x; x < p->x + p->w; ++x) {
      double qv = map[y * w + x].value(halflife);

      if (pv > qv) {
//        double qhv = map[y * w + x].hval;
//fprintf(stderr, "pv=%lf (%u,%lf) qv=%lf (%u,%lf)\n", pv, p->ts, phv, qv, map[y*w+x].ts, qhv);

        uint32_t px = x - p->x;
        uint32_t py = y - p->y;
        uint8_t r = p->rgb[py * p->w * 3 + px * 3 + 0];
        uint8_t g = p->rgb[py * p->w * 3 + px * 3 + 1];
        uint8_t b = p->rgb[py * p->w * 3 + px * 3 + 2];

        if (r || g || b) {

        map[y * w + x].off = poff;
        map[y * w + x].ts = p->ts;
        map[y * w + x].hval = phv;

        rgb[y * w * 3 + x * 3 + 0] = p->rgb[py * p->w * 3 + px * 3 + 0];
        rgb[y * w * 3 + x * 3 + 1] = p->rgb[py * p->w * 3 + px * 3 + 1];
        rgb[y * w * 3 + x * 3 + 2] = p->rgb[py * p->w * 3 + px * 3 + 2];

        ++count;
        }
      }
    }
  }

  fprintf(stderr, "count=%u\n", count);
  if (count) {
    if (p->seen == 0) {
      p->seen = time(NULL);
    }
    p->write(log_fd);
    int ret = ::write(seenoff_fd, &p->seen, 4);
    assert(ret == 4);
    ret = ::write(seenoff_fd, &poff, 4);
    assert(ret == 4);
  }

  return count;
}
  
  

