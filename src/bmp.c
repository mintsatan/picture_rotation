#include "bmp.h"

#include "util.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

#define PRI_SPECIFIER(e) (_Generic((e), uint16_t          \
                                   : "%" PRIu16, uint32_t \
                                   : "%" PRIu32, default  \
                                   : "NOT IMPLEMENTED"))

#define PRINT_FIELD(t, name)                             \
  fprintf(f, "%-17s: ", #name);                          \
  fprintf(f, PRI_SPECIFIER(header->name), header->name); \
  fprintf(f, "\n");

void bmp_header_print(struct bmp_header const *header, FILE *f)
{
    FOR_BMP_HEADER(PRINT_FIELD)
}

static enum read_status read_header(FILE *f, struct bmp_header *p_header)
{
    return !fread(p_header, sizeof(struct bmp_header), 1, f);
}

static enum write_status write_header(FILE *f, struct bmp_header const *p_header)
{
    return !fwrite(p_header, sizeof(struct bmp_header), 1, f);
}

static enum read_status read_image(FILE *in, struct image *p_img)
{
    const uint8_t mod_4    = p_img->width * 3 % 4;
    const uint64_t padding = mod_4 ? 4 - mod_4 : 0;

    p_img->data =
            (struct pixel *)
                    malloc(sizeof(struct pixel) * p_img->width * p_img->height);

    for (size_t i = 0; i < p_img->height; i++) {
        fread(p_img->data + i * p_img->width, sizeof(struct pixel), p_img->width, in);
        fseek(in, padding, SEEK_CUR);
    }

    return READ_OK;
}

static enum write_status write_image(FILE *out, struct image const *p_img)
{
    const uint8_t mod_4            = p_img->width * 3 % 4;
    const uint64_t padding         = mod_4 ? 4 - mod_4 : 0;
    const struct pixel trash_pixel = {0};

    for (size_t i = 0; i < p_img->height; i++) {
        fwrite(p_img->data + i * p_img->width, sizeof(struct pixel), p_img->width, out);
        fwrite(&trash_pixel, sizeof(struct pixel), padding, out);
    }

    return WRITE_OK;
}

enum read_status read_bmp_from_file(
        FILE *file,
        struct image *p_img)
{
    struct bmp_header header;
    if (!file)
    {
        return READ_INVALID_SIGNATURE;
    }

    if (read_header(file, &header))
    {
        err("Can't read header\n");
        return READ_INVALID_HEADER;
    }

    p_img->width  = header.biWidth;
    p_img->height = header.biHeight;
    if (read_image(file, p_img))
    {
        err("Can't read image\n");
        return READ_INVALID_BITS;
    }

    return READ_OK;
}

enum write_status write_bmp_to_file(
        FILE *file,
        struct image *p_img)
{
    struct bmp_header header =
            {
                    .bfType      = 19778,
                    .bOffBits    = sizeof(struct bmp_header),
                    .biWidth     = p_img->width,
                    .biHeight    = p_img->height,
                    .biPlanes    = 1,
                    .biBitCount  = sizeof(struct pixel) * 8,
                    .biSizeImage = p_img->height * p_img->width * sizeof(struct pixel)
            };
    header.biSize    = header.bOffBits - 14;
    header.bfileSize = header.bOffBits + header.biSizeImage;

    if (!file)
    {
        return WRITE_ERROR;
    }

    if (write_header(file, &header))
    {
        err("Can't write header\n");
        return WRITE_ERROR;
    }

    if (write_image(file, p_img))
    {
        err("Can't write image\n");
        return WRITE_ERROR;
    }

    return WRITE_OK;
}