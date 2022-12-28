#include <game.h>

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {
  AM_KEYS(KEYNAME)
};
#define SIDE 16
extern int rx,ry;
extern int w, h;
extern void draw_tile(int x, int y, int w, int h, uint32_t color);

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    if ((rx & 1) ^ (ry & 1)) {
        draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    else {
      draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0xff0000); // red
    }
    switch (event.keycode) {
      case 0x1f + 0x0d:
        if(ry * SIDE < h)
          ry += 1;
        draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0x000000); // black
        break;
      case 0x11 + 0x0d:
        if(ry >0)
          ry -= 1;
        draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0x000000); // black
        break;
      case 0x1e + 0x0d:
        if(rx >0)
        rx -= 1;
        draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0x000000); // black
        break;
      case 0x20 + 0x0d:
        if(rx * SIDE < w)
          rx += 1;
        draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0x000000); // black
        break;
      default :
        draw_tile(rx * SIDE, ry * SIDE, SIDE, SIDE, 0x000000); // black
        break;  
    }
    puts("\n");
  }
}
