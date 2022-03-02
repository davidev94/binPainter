/*
 * binPainter.c - Davide Valisa
 */

/* -------------------------------------------------------------------------- */

#include "ctype.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

/* -------------------------------------------------------------------------- */

#define RGB565(r, g, b)                                                        \
  (((r & 0xf8) << 8) + ((g & 0xfc) << 3) + ((b & 0xf8) >> 3))

#define BLACK 0x0000          /*   0,   0,   0 */
#define NAVY 0x000F           /*   0,   0, 128 */
#define DARKGREEN 0x03E0      /*   0, 128,   0 */
#define DARKCYAN 0x03EF       /*   0, 128, 128 */
#define MAROON 0x7800         /* 128,   0,   0 */
#define PURPLE 0x780F         /* 128,   0, 128 */
#define OLIVE 0x7BE0          /* 128, 128,   0 */
#define LIGHTGREY 0xC618      /* 192, 192, 192 */
#define DARKGREY 0x7BEF       /* 128, 128, 128 */
#define VERYDARKGREY 0x41E7
#define BLUE 0x001F           /*   0,   0, 255 */
#define GREEN 0x07E0          /*   0, 255,   0 */
#define CYAN 0x07FF           /*   0, 255, 255 */
#define RED 0xF800            /* 255,   0,   0 */
#define MAGENTA 0xF81F        /* 255,   0, 255 */
#define YELLOW 0xFFE0         /* 255, 255,   0 */
#define WHITE 0xFFFF          /* 255, 255, 255 */
#define ORANGE 0xFDA0         /* 255, 180,   0 */
#define DARKORANGE 0xC980     /* 204,  51,   0 */
#define GREENYELLOW 0xB7E0    /* 180, 255,   0 */
#define PINK 0xFC9F

/* -------------------------------------------------------------------------- */

FILE *write_ptr;

void writeOne(uint8_t data, uint8_t times);
void writeTwo(uint16_t data, uint8_t times);
void writeFour(uint32_t data, uint8_t times);

int help();

uint32_t colorMap = 0;
uint32_t scale = 1;
uint32_t width = 256;
uint8_t print = 0;

/* -------------------------------------------------------------------------- */

int main(int argc, char **argv) {
  int c;

  while ((c = getopt(argc, argv, "c:s:w:ph")) != -1)
    switch (c) {
    case 'c':
      colorMap = strtol(optarg, NULL, 0);
      break;
    case 's':
      scale = strtol(optarg, NULL, 0);
      break;
    case 'w':
      width = strtol(optarg, NULL, 0);
      break;
    case 'p':
      print = 0xFF;
      break;
    case 'h':
      return help();
      break;
    case '?':
      if (optopt == 'c' || optopt == 's' || optopt == 'w')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort();
    }

  if (optind == argc) {
    printf("Too few arguments!\n");
    return help();
  }
  if (argc > optind + 1) {
    printf("Too many arguments!\n");
    return help();
  }

  char *fnIN;
  fnIN = (char *)malloc(strlen(argv[optind]) * sizeof(char) + 1);
  memcpy(fnIN, argv[optind], strlen(argv[optind]) + 1);

  if (width % 2 > 0) {
    printf("Width must be a multiple of 2\n");
    return 1;
  }

  if (colorMap > 2) {
    printf("Color map must be 0 <= colorMap <= 2\n");
    return 1;
  }

  FILE *fileptr;
  unsigned char *bufferIN;
  unsigned long lenIN;

  fileptr = fopen(fnIN, "rb");
  fseek(fileptr, 0, SEEK_END);
  lenIN = ftell(fileptr);
  rewind(fileptr);
  bufferIN = (unsigned char *)malloc(lenIN * sizeof(unsigned char));
  fread(bufferIN, lenIN, 1, fileptr);
  fclose(fileptr);

  write_ptr = fopen(strcat(fnIN, ".bmp"), "wb");

  uint32_t height = (lenIN + (lenIN % width)) / width;
  printf("%d x %f byte\n", width, 1.0 * lenIN / width);
  width = width * scale;
  height = height * scale;
  printf("%d x %d pixel\n", width, height);

  /* File header, 14 bytes: Windows 3.1x, 95, NT, ... etc. */
  writeOne('B', 1);
  writeOne('M', 1);
  writeFour(14 + 40 + 12 + width * height * 2, 1); // File size in bytes317
  writeFour(0, 1);
  writeFour(14 + 40 + 12, 1); // Offset to image data from start

  /* Image header, 40 bytes: BITMAPINFOHEADER */
  writeFour(40, 1);     // Header size
  writeFour(width, 1);  // Image width
  writeFour(height, 1); // Image height
  writeTwo(1, 1);       // Planes
  writeTwo(16, 1);      // Bits per pixel
  writeFour(3, 1); // Compression (BI_BITFIELDS) -> follow colour bitmask
  writeFour(0, 1); // Image size (0 for uncompressed)
  writeFour(0, 1); // Preferred X resolution (ignore)
  writeFour(0, 1); // Preferred Y resolution (ignore)
  writeFour(0, 1); // Colour map entries (ignore)
  writeFour(0, 1); // Important colours (ignore)

  /* Colour masks: 12 bytes */
  writeFour(0b1111100000000000, 1); // Red
  writeFour(0b0000011111100000, 1); // Green
  writeFour(0b0000000000011111, 1); // Blue

  /* Image data: width * height * 2 bytes */
  int32_t x = 0, y = 0;
  // printf("\nx: %d/0x%x", x, x);
  // printf("\ny: %d/0x%x", y, y);
  // printf("\n");
  // printf("\ncolorMap: %d",colorMap);
  uint32_t count = 0;
  uint8_t data = 0;
  for (y = height - 1; y >= 0; y--) {
    if (print > 0)
      printf("\n");
    for (x = 0; x < width; x++) {
      if ((int)(x / scale) + ((int)(width / scale) * (int)(y / scale)) <
          lenIN) {
        data = bufferIN[(int)(x / scale) +
                        ((int)(width / scale) * (int)(y / scale))];
        if (print > 0)
          printf("%02x", data);

        switch (colorMap) {
        case 1:
          writeTwo(RGB565(data / 2, 255 - data / 2, data), 1);
          break;
        case 2:
          writeTwo(RGB565(data, data / 2, 255 - data), 1);
          break;
        default:
          if (data == 0) {
            writeTwo(YELLOW, 1);
            break;
          }
          if (data == 0xFF) {
            writeTwo(BLUE, 1);
            break;
          }
          if (data >= 127 && data < 0xFF) {
            writeTwo(RED, 1);
            break;
          }
          if (data < 127 && data >= 32) {
            writeTwo(GREEN, 1);
            break;
          }
          writeTwo(ORANGE, 1);
          break;
        }
        count++;
      } else {
        writeTwo(RGB565(128, 128, 128),
                 1);
        count++;
      }
    }
  }
  if (print > 0)
    printf("\n\n");
  printf("%d pixel written\n", count);
  fclose(write_ptr);
  return 0;
}

/* -------------------------------------------------------------------------- */

void writeOne(uint8_t data, uint8_t times) {
  while (times > 0) {
    fwrite(&data, 1, 1, write_ptr);
    times--;
  }
}
void writeTwo(uint16_t data, uint8_t times) {
  while (times > 0) {
    fwrite(&data, 2, 1, write_ptr);
    times--;
  }
}
void writeFour(uint32_t data, uint8_t times) {
  while (times > 0) {
    fwrite(&data, 4, 1, write_ptr);
    times--;
  }
}

int help() {
  printf("usage: binPainter [option] filename\n");

  printf("option:\n");
  printf("   -c arg     ");
  printf("choose color map, default arg=0\n");
  printf("              ");
  printf("can be 0,1,2\n");

  printf("   -w arg     ");
  printf("choose bitmap width, default arg=256\n");

  printf("   -s arg     ");
  printf("choose image scale, default arg=1\n");

  printf("   -p         ");
  printf("printf pixel data in hex \n");

  printf("   -h         ");
  printf("print this help\n");

  printf("\n");

  printf("example: binPainter -w 128 -c 1 testFile.bin\n");
  printf("\n");

  return 1;
}

/* -------------------------------------------------------------------------- */
