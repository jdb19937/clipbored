#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

#include <math.h>

#include "paste.hh"
#include "board.hh"

#define LOCK(x) {int ret = flock(x,LOCK_EX); assert(ret == 0); }
#define UNLOCK(x) flock(x,LOCK_UN)

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: pasteout board.dir [start_ts [end_ts]] > pastes\n");
    return 1;
  }

  long ts = time(NULL);
  if (argc > 2) {
    ts = atoi(argv[2]);
  }
  if (ts < 0) {
    ts += time(NULL);
  }
  long end_ts = 0;
  if (argc > 3) {
    end_ts = atoi(argv[3]);
  }
  if (end_ts < 0) {
    end_ts += time(NULL);
  }

  const char *dir = argv[1];
  Board b(dir);

  long off = b.find_off(ts);
  if (off >= 0) {
fprintf(stderr, "off=%ld\n", off);
    lseek(b.log_fd, off, SEEK_SET);
  } else {
    lseek(b.log_fd, 0, SEEK_END);
  }

  while (1) {
    Paste p;
LOCK(b.log_fd);
    if (!p.read(b.log_fd)) {
UNLOCK(b.log_fd);
fprintf(stderr, "hit end, sleep(1)\n");
      sleep(1);
      continue;
    }
UNLOCK(b.log_fd);
//fprintf(stderr, "p.seen=%d\n", p.seen);

    if (argc > 3) {
      int ret = lseek(b.log_fd, 0, SEEK_CUR);
      assert(ret != -1);
      if (p.seen > end_ts)
        break;
    }

    p.write(stdout);
  }

//  execl("/usr/bin/tail", "/usr/bin/tail", "-0f", logfn.c_str(), NULL);

  return 0;
}

