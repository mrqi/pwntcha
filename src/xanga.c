/*
 * xanga.c: decode Xanga captchas
 * $Id$
 *
 * Copyright: (c) 2005 Sam Hocevar <sam@zoy.org>
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the Do What The Fuck You Want To
 *   Public License as published by Banlu Kemiyatorn. See
 *   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "config.h"
#include "common.h"

static void fill_white_holes(struct image *img);

/* Our macros */
#define FACTOR 1
#define FONTNAME "font_xanga.png" // use with FACTOR = 1
//#define FONTNAME "font.png" // use with FACTOR = 2
//#define FONTNAME "font_dilated.png" // use with FACTOR = 2
static struct image *font = NULL;

/* Global stuff */
struct { int xmin, ymin, xmax, ymax; } objlist[100];
int objects, first, last;
char *result;

/* Main function */
char *decode_xanga(struct image *img)
{
    struct image *tmp;

    /* Initialise local data */
    objects = 0;
    first = -1;
    last = -1;

    /* Xanga captchas have 7 characters */
    result = malloc(8 * sizeof(char));
    strcpy(result, "       ");

    tmp = image_dup(img);
image_save(tmp, "xanga1.bmp");
    /* Clean image a bit */
//    filter_equalize(tmp, 200);
    filter_contrast(tmp);
    //filter_detect_lines(tmp);
image_save(tmp, "xanga2.bmp");
    fill_white_holes(tmp);
//    filter_fill_holes(tmp);
image_save(tmp, "xanga3.bmp");
    //filter_detect_lines(tmp);
//    filter_median(tmp);
//image_save(tmp, "xanga4.bmp");
    filter_equalize(tmp, 128);
image_save(tmp, "xanga4.bmp");
return NULL;

    /* Detect small objects to guess image orientation */
    filter_median(tmp);
    filter_equalize(tmp, 200);

    /* Invert rotation and find glyphs */
    rotate(tmp);
    filter_median(tmp);

    /* Clean up our mess */
    image_free(tmp);

    /* aaaaaaa means decoding failed */
    if(!strcmp(result, "aaaaaaa"))
        result[0] = '\0';

    return result;
}

/* The following functions are local */

static void fill_white_holes(struct image *img)
{
    struct image *tmp;
    int x, y, i;
    int r, g, b;

    tmp = image_new(img->width, img->height);

    for(y = 0; y < img->height; y++)
        for(x = 0; x < img->width; x++)
        {
            getpixel(img, x, y, &r, &g, &b);
            setpixel(tmp, x, y, r, g, b);
        }

    for(y = 1; y < img->height - 1; y++)
        for(x = 1; x < img->width - 1; x++)
        {
            int count = 0;
            getpixel(img, x, y, &r, &g, &b);
            if(r <= 16)
                continue;
            getpixel(img, x + 1, y, &r, &g, &b);
            count += r;
            getpixel(img, x - 1, y, &r, &g, &b);
            count += r;
            getpixel(img, x, y + 1, &r, &g, &b);
            count += r;
            getpixel(img, x, y - 1, &r, &g, &b);
            count += r;
            if(count > 600)
                continue;
            setpixel(tmp, x, y, count / 5, count / 5, count / 5);
        }

    image_swap(tmp, img);
    image_free(tmp);
}
