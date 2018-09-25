#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <math.h>

#include "paste.hh"
#include "board.hh"

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: newbored <dirname> <boardname> <width> <height> <halflife>\n");
    return 1;
  }

  const char *dir = argv[1];
  const char *boardname = argv[2];
  unsigned int w = 1024;
  if (argc >= 4)
    w = strtoul(argv[3], NULL, 0);
  unsigned int h = 1024;
  if (argc >= 5)
    h = strtoul(argv[4], NULL, 0);
  double halflife = 86400;
  if (argc >= 6)
    halflife = strtod(argv[5], NULL);

  Board::create(dir, boardname, w, h, halflife);
  Board b(dir);
  assert(!strcmp(b.name.c_str(), boardname));

  std::string html_fn = std::string(dir) + "/index.html";
  FILE *html_fp = fopen(html_fn.c_str(), "w");
  assert(html_fp);

  const char *html = 
"<html> <head> <title>%s</title> <script>\n"
"window.setInterval(function() {\n"
"  var render = document.getElementById('render');\n"
"  render.src = 'render.png?rand=' + Math.random();\n"
"  var heatmap = document.getElementById('heatmap');\n"
"  heatmap.src = 'heatmap.png?rand=' + Math.random();\n"
"  }, 5000);\n"
"</script></head><body>\n"
"<img id=render src='render.png'>\n"
"<br/><br/>\n"
"<img id=heatmap src='heatmap.png'>\n"
"</body> </html>\n"
;

  fprintf(html_fp, html, b.name.c_str());
  fclose(html_fp);

  return 0;
}

