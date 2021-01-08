#include "rotate.h"

#include <stddef.h>
#include <malloc.h>

struct image rotate_copy_left(struct image image)
{
  const uint64_t w = image.width;
  const uint64_t h = image.height;

  struct image new_image = {.height = w, .width = h};

  new_image.data = (struct pixel *)malloc(sizeof(struct pixel) * h * w);
  for (size_t i = 0; i < h; i++)
  {
    for (size_t j = 0; j < w; j++)
    {
      new_image.data[j * h + i] = image.data[(i + 1) * w - j - 1];
    }
  }
  return new_image;
}

struct image rotate_copy_right(struct image image)
{
  const uint64_t w = image.width;
  const uint64_t h = image.height;

  struct image new_image;
  new_image.height = w;
  new_image.width  = h;

  new_image.data = (struct pixel *)malloc(sizeof(struct pixel) * h * w);
  for (size_t i = 0; i < h; i++)
  {
    for (size_t j = 0; j < w; j++)
    {
      new_image.data[j * h + i] = image.data[(h - i - 1) * w + j];
    }
  }
  return new_image;
}