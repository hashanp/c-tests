#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "image.h"
#include "dragon.h"

/* x, y: coordinates of turtle */
static long x, y;

/* When drawing a pixel to the image, x and y must be divided by this value.
 * This enables both the dragon curve and twin dragon to rendered without
 * clipping.
 */
static long scale;

/* drawn_pixels: the length of the path travelled. */
static long drawn_pixels;

/* direction: the current direction of the turtle. */
static vector_t direction;

/* Returns a vector that describes the initial direction of the turtle. Each
 * iteration corresponds to a 45 degree rotation of the turtle anti-clockwise.  */
vector_t starting_direction(int total_iterations)
{
  int k = total_iterations % 8;
  switch (k) {
    case 0: return (vector_t) { 1, 0 };
    case 1: return (vector_t) { 1, 1 };
    case 2: return (vector_t) { 0, 1 };
    case 3: return (vector_t) { -1, 1 };
    case 4: return (vector_t) { -1, 0 };
    case 5: return (vector_t) { -1, -1 };
    case 6: return (vector_t) { 0, -1 };
    case 7: return (vector_t) { 1, -1 };
    default: assert(false);
  }
}

/* Draws a pixel to dst at location (x, y). The pixel intensity is chosen as a
 * function of image size and the number of pixels drawn.
 *
 * The gray level changes over entire size of path; the pixel value oscillates
 * along the path to give some contrast between segments close to each other
 * spatially.
 */
void draw_greyscale(image_t *dst,long x, long y)
{
  int k = (LEVEL * drawn_pixels) / (dst->height * dst->height);
  int m;
  switch (k) {
    case 0: m = 100; break;
    case 1: m = 120; break;
    case 2: m = 150; break;
    case 3: m = 180; break;
    case 4: m = 200; break;
    default: m = 255; 
  }
  //printf("%li %li %i\n", x, y, m);
  set_pixel(dst, x, y, m);
}

void anticlockwise() {
  if (direction.dx == 0) {
    direction.dx = direction.dy == 1 ? -1 : 1; 
  } else if (direction.dx == -1) {
    if (direction.dy == -1) {
      direction.dx = 0;
    } else {
      direction.dy--;
    }
  } else {
    if (direction.dy == 1) {
      direction.dx = 0;
    } else {
      direction.dy++;
    }
  }
}

void clockwise() {
  if (direction.dx == 0) {
    direction.dx = direction.dy == 1 ? 1 : -1; 
  } else if (direction.dx == -1) {
    if (direction.dy == 1) {
      direction.dx = 0;
    } else {
      direction.dy++;
    }
  } else {
    if (direction.dy == -1) {
      direction.dx = 0;
    } else {
      direction.dy--;
    }
  }
}

/* Iterates though the characters of str, recusively calling string_iteration()
 * until rules have been applied iterations times, or no rule is applicable.
 * and update the image.
 */

void string_iteration(image_t *dst, const char *str, int iterations)
{
  if (iterations == -1) {
    return;
  }
  for(const char *i = str; *i != '\0'; i++) {
    switch (*i) {
      case '+':
        clockwise();
        clockwise();
        break;
      case '-':
        anticlockwise();
        anticlockwise();
        break;
      case 'F':
        drawn_pixels++;
        draw_greyscale(dst, x / scale, y / scale);
        x += direction.dx;
        y += direction.dy;
        break;
      case 'X':
        string_iteration(dst, "X+YF", iterations - 1);
        break;
      case 'Y':
        string_iteration(dst, "FX-Y", iterations - 1);       
        break;
      default:
        assert(false);
    }
  }
}

/* Creates an image of requested size, calls starting_direction() to compute
 * initial turtle direction then calls string_iteration() to construct the
 * image. The constructed image is saved to a file in the output directory.
 */
void dragon(long size, int total_iterations)
{
  direction = starting_direction(total_iterations);
  int height = size;
  int width = size * 1.5;
  x = size;
  y = size;
  scale = 2;
  image_t *img; 
  init_image(&img, width, height, 1, 255);
  string_iteration(img, "FX+FX+", total_iterations);
  image_write("twindragon.pgm", img, PGM_FORMAT);
  image_free(img);
}

/* The main function. When called with an argument, this should be considered
 * the number of iterations to execute. Otherwise, it is assumed to be 9. Image
 * size is computed from the number of iterations then dragon() is used to
 * generate the dragon image. */
int main(int argc, char **argv)
{
  int i = 9;
  if (argc == 2) {
    i = atoi(argv[1]);
  }
  dragon(pow(2, i), 2 * i);
	return EXIT_SUCCESS;
}
