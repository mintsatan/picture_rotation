#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdint.h>

struct pixel
{
  uint8_t b, g, r;
};

struct image
{
  uint64_t width, height;
  struct pixel *data;
};

#endif
