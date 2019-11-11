// test connected component labeling algorithm of the library
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "imcore.h"
#include "iocore.h"


// find the minimum label assigned to this value by tracing the connected labels
uint32_t find_root(uint32_t *list, uint32_t value)
{
    while (value != list[value])
    {
        value = list[value];
    }
    return value;
}

// sort 4 element array
void sort_neighbours(uint32_t neigh[4])
{
    uint32_t temp = 0;
    uint32_t i = 0;

    while (i < 3)
    {
        if (neigh[i] > neigh[i + 1])
        {
            temp = neigh[i];
            neigh[i] = neigh[i + 1];
            neigh[i + 1] = temp;
            i = 0;
        }
        else
        {
            i++;
        }
    }
}

// connected component labeling algorithm as presented in cescript blog (buggy)
void connected_component_label(matrix_t *input, matrix_t *output, uint32_t *numCC)
{
    // set the number of connected componenets to zero
    numCC[0] = 0;

    int cond = channels(input) == 1 && is_8u(input) && is_32u(output);
    
    // if the condition is not met, return
    if(!cond)
    {
        printf("input must be uint8 array with single channel and output must be uint32 array!\n");
        return;
    }

    // get the pointer to the data
    uint8_t *in_data = data(uint8_t, input);
    uint32_t *out_data = data(uint32_t, output);

    // resize the output label matrix
    matrix_resize(output, rows(input), cols(input), 1);

    // allocate the list array
    uint32_t *list = calloc(volume(input) / 4, sizeof(uint32_t)); //en fazla kaç ayrık nesne olabilir

    // label number for the next component, 0 background, 1 foreground
    uint32_t current_label = 2;

    // neighbours of the current pixel (r,c)
    uint32_t neigh[4];

    // make sure that the input is in 0-1 range
    uint32_t idx = 0;
    for(idx = 0; idx < volume(input); idx++)
    {
        out_data[idx] = in_data[idx] > 0 ? 1 : 0;
    }
    
    uint32_t r = 0, c = 0;
    for (int r = 1; r < rows(input) - 1; r++)
    {
        for (int c = 1; c < cols(input) - 1; c++)
        {
            // if the current pixel is foreground
            if (at(uint32_t, output, r,c) != 0)
            {
                // get the four neighbours of the current pixel
                neigh[0] = at(uint32_t, output, r, c - 1);
                neigh[1] = at(uint32_t, output, r - 1, c);
                neigh[2] = at(uint32_t, output, r - 1, c - 1);
                neigh[3] = at(uint32_t, output, r - 1, c + 1);

                // check all the neighbours, if all of them are unlabeled
                if (neigh[0] == 0 && neigh[1] == 0 && neigh[2] == 0 && neigh[3] == 0)
                {
                    // set the current label of the output to the next label
                    at(uint32_t, output, r, c) = current_label;

                    // update the connection list
                    list[current_label] = current_label;

                    // go to the next label
                    current_label++;
                }
                // if one of the neighbour is labeled, find the root of the label
                else
                {
                    // sort the labels of the neighbours
                    sort_neighbours(neigh);

                    // compare the labels of the neighbours 
                    if (neigh[0] != 0 && neigh[1] != neigh[0])
                    {
                        list[neigh[1]] = find_root(list, neigh[0]);
                    }
                    if (neigh[1] != 0 && neigh[2] != neigh[1])
                    {
                        list[neigh[2]] = find_root(list, neigh[1]);
                    }
                    if (neigh[2] != 0 && neigh[3] != neigh[2])
                    {
                        list[neigh[3]] = find_root(list, neigh[2]);
                    }

                    // starting from the smallest label, pick the first one different from the background
                    uint32_t l = 0;
                    for (int l = 0; l < 4; l++)
                    {
                        // pick the first one different from the background
                        if (neigh[l] > 0)
                        {
                            at(uint32_t, output, r, c) = neigh[l];
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Create Consecutive Label
    uint32_t *final_list = calloc(current_label, sizeof(uint32_t)); 

    // go over all the labels one more time and make them consecutive
    uint32_t l = 0;
    for (l = 2; l < current_label; l++)
    {
        if (l == find_root(list, l))
        {
            // new component found
            numCC[0]++; 
            final_list[l] = numCC[0];
        }
    }

    // re assign the labels that are assigned previously
    for (idx = 0; idx < volume(output); idx++)
    {
        out_data[idx] = final_list[find_root(list, out_data[idx])];
    }

    // clean unused arrays
    free(list);
    free(final_list);
}

int main()
{
    // read image
    matrix_t *img = imread("..//data//complex.bmp");

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
    //bwlabel(bwimg, label, &conn);
    connected_component_label(bwimg, label, &conn);

    // print the number of labels
    printf("Label: %d\n", conn);

    // assign different color for eack connected component
    label2rgb(label, conn, img);

    // test output
    imwrite(img, "..//data//complex_ccl.bmp");

    return 0;
}
