/* sample.c -- handling 128x128, 32x32, 8x8 characters
 * copyright (c) 2003 Peter van Rossum
 * $Id: sample.c,v 1.2 2003/10/19 04:25:13 petervr Exp $ */

#include "ocr.h"

/****************************************
 * 8x8
 ****************************************/

void
rescale_intensity8x8 (float intensity[8][8])
{
  float min = 100000000.0;
  float max = 0.0;
  int x, y;

  /* find minimal and maximal intensity */
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      if (intensity[x][y] < min) {
        min = intensity[x][y];
      }
      if (intensity[x][y] >= max) {
        max = intensity[x][y];
      }
    }
  }

  if (max <= 0.0) {
    /* the character is completely empty */
    return;
  }

  /* rescale so minimum is 0.0 and maximum is 1.0 */
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      intensity[x][y] = (intensity[x][y] - min) / (max - min);
    }
  }
}

/****************************************
 * 32x32
 ****************************************/

void
preprocess_char32x32 (char c[32][32], int reshape, float intensity[8][8])
{
  int pixels;
  int x, y, i, j;

  /* reshape not implemented for 32x32 */
  reshape = reshape;

  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      pixels = 0;
      for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
          pixels += c[4 * x + i][4 * y + j];
        }
      }
      intensity[x][y] = pixels;
    }
  }

  rescale_intensity8x8 (intensity);
}

/****************************************
 * 128x128
 ****************************************/

int
find_left128x128 (char c[128][128])
{
  int x, y;

  for (x = 0; x < 128; x++) {
    for (y = 0; y < 128; y++) {
      if (c[x][y] != 0) {
        return x;
      }
    }
  }
  return x;
}

int
find_right128x128 (char c[128][128])
{
  int x, y;

  for (x = 127; x >= 0; x--) {
    for (y = 0; y < 128; y++) {
      if (c[x][y] != 0) {
        return x;
      }
    }
  }
  return x;
}

int
find_top128x128 (char c[128][128])
{
  int x, y;

  for (y = 0; y < 128; y++) {
    for (x = 0; x < 128; x++) {
      if (c[x][y] != 0) {
        return y;
      }
    }
  }
  return y;
}

int
find_bottom128x128 (char c[128][128])
{
  int x, y;

  for (y = 128; y >= 0; y--) {
    for (x = 0; x < 128; x++) {
      if (c[x][y] != 0) {
        return y;
      }
    }
  }
  return y;
}

void
preprocess_char128x128 (char c[128][128], int reshape, float intensity[8][8])
{
  int left, right, top, bottom, width, height, size;
  int pixels;
  int x, y, i, j;

  if (reshape) {

    /* find left,right,top,bottom-most non-zero column,row */
    left = find_left128x128 (c);

    if (left == 128) {
      /* the character is completely empty */
      for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
          intensity[x][y] = 0.0;
        }
      }
      return;
    }

    right = find_right128x128 (c);
    top = find_top128x128 (c);
    bottom = find_bottom128x128 (c);

    /* center character in square whose sides are multiple of 8 */
    width = right - left;       /* 1 less than the width, actually */
    height = bottom - top;      /* 1 less than the height, actually */
    if (width < height) {
      size = height - (height % 8) + 8;
    } else {
      size = width - (width % 8) + 8;
    }
    left = left - (size - width) / 2;
    if (left < 0) {
      left = 0;
    }
    right = left + size;
    top = top - (size - height) / 2;
    if (top < 0) {
      top = 0;
    }
    bottom = top + size;
  } else {
    left = 0;
    right = 127;
    top = 0;
    bottom = 127;
    size = 128;
  }
  size /= 8;

  /* count number of pixels in each of the 8*8 size*size blocks */
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      pixels = 0;
      for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
          pixels += c[left + x * size + i][top + y * size + j];
        }
      }
      intensity[x][y] = pixels;
    }
  }

  rescale_intensity8x8 (intensity);
}
