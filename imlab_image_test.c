// test connected component labeling algorithm of the library
#include <stdio.h>
#include <stdint.h>
#include "imcore.h"
#include "iocore.h"

int main()
{
    // read image
    matrix_t *img   = imread("../data/bb.bmp");

    // allocate gray, black-white and labeled image
    matrix_t *grayimg = matrix_create(uint8_t, rows(img), cols(img), 1);
    matrix_t *bwimg = matrix_create(uint8_t, rows(img), cols(img), 1);
    matrix_t *label = matrix_create(uint32_t, rows(img), cols(img), 1);

    // convert input to grayscale
    rgb2gray(img, grayimg);

    // binarize the grayscale image
    imthreshold(grayimg, 128, bwimg);

    // find connected components
    uint32_t conn = 0;
    bwlabel(bwimg, label, &conn);

    // print the number of labels
    printf("Label: %d\n", conn);

    // assign different color for eack connected component
    label2rgb(label, conn, img);

    // test output
    imwrite(img, "../data/bbrgb.bmp");

    return 0;
}
