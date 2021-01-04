#include <stdio.h>

#include "bmp.h"
#include "util.h"
#include "image.h"
#include "rotate.h"
#include <stdio.h>
#include <string.h>

void usage()
{
  fprintf(stderr, "Usage: make run ARGS=\"BMP_FILE_NAME DIRECTION\"\n");
  fprintf(stderr, "DIRECTION can be left or right, default: left\n");
}

enum rotate_direction {
  LEFT_ROTATE = 0,
  RIGHT_ROTATE
};

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    usage();
    err("Not enough arguments \n");
  }
  else if (argc > 3)
  {
    usage();
    err("Too many arguments \n");
  }

  FILE * input = fopen(argv[1], "rb");
  if (!input)
  {
    err("Can't open file\n");
    return -1;
  }

  enum rotate_direction direction;
  if (argc == 3) {
    if (!strcmp(argv[2], "right")) {
      direction = RIGHT_ROTATE;
    } else {
      direction = LEFT_ROTATE;
    }
  } else {
    direction = LEFT_ROTATE;
  }

  struct bmp_header header = {0};
  struct image img = {0};

  if (!read_bmp_from_file(input, &header, &img))
  {
    struct image new_img = direction == LEFT_ROTATE ?
                            rotate_copy_left(img) :
                            rotate_copy_right(img);

    FILE * output = fopen(argv[1], "wb");
    if (!output)
    {
      fclose(input);
      err("Can't create rotated image file\n");
      free(img->data);
      return -1;
    }

    header.biWidth  = new_img.width;
    header.biHeight = new_img.height;
    if (write_bmp_to_file(output, &header, &new_img))
    {
      fclose(input);
      fclose(output);
      err("Can't write in file\n");
      free(img->data);
      return -1;
    }

    fclose(output);
    free(img->data);
    free(new_img->data);
  }
  else
  {
    err("Failed to open BMP file or reading header or picture.\n");
  }
  fclose(input);
  return 0;
}
