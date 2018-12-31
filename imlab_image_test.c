// test all functionalities of the library
#include <stdio.h>
#include <stdint.h>
#include "imcore.h"
#include "prcore.h"
#include "iocore.h"
#include <time.h>

int main()
{
    int i,j,c;

    double fill_inf[3] ={1,2,3.3};

    matrix_t *img   = imread("../data/bb.bmp");

    matrix_t *grayimg = matrix_create(uint8_t, rows(img), cols(img), 1);
    matrix_t *bwimg = matrix_create(uint8_t, rows(img), cols(img), 1);
    matrix_t *label = matrix_create(uint32_t, rows(img), cols(img), 1);

    // grayscale
    rgb2gray(img, grayimg);

    // binarize
    imthreshold(grayimg, 128, bwimg);

    uint32_t conn = 0;
    bwlabel(bwimg, label, &conn);
    printf("Label: %d\n", conn);
    label2rgb(label, conn, img);

    // test imwrite
    imwrite(img, "bbrgb.bmp");

    return 0;
}
