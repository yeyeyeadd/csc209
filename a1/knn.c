#include <stdio.h>
#include <math.h>    // Need this for sqrt()
#include <stdlib.h>
#include <string.h>

#include "knn.h"

/* Print the image to standard output in the pgmformat.  
 * (Use diff -w to compare the printed output to the original image)
 */
void print_image(unsigned char *image) {
    printf("P2\n %d %d\n 255\n", WIDTH, HEIGHT);
    for (int i = 0; i < NUM_PIXELS; i++) {
        printf("%d ", image[i]);
        if ((i + 1) % WIDTH == 0) {
            printf("\n");
        }
    }
}

/* Return the label from an image filename.
 * The image filenames all have the same format:
 *    <image number>-<label>.pgm
 * The label in the file name is the digit that the image represents
 */
unsigned char get_label(char *filename) {
    // Find the dash in the filename
    char *dash_char = strstr(filename, "-");
    // Convert the number after the dash into a integer
    return (char) atoi(dash_char + 1);
}

/* ******************************************************************
 * Complete the remaining functions below
 * ******************************************************************/


/* Read a pgm image from filename, storing its pixels
 * in the array img.
 * It is recommended to use fscanf to read the values from the file. First, 
 * consume the header information.  You should declare two temporary variables
 * to hold the width and height fields. This allows you to use the format string
 * "P2 %d %d 255 "
 * 
 * To read in each pixel value, use the format string "%hhu " which will store
 * the number it reads in an an unsigned character.
 * (Note that the img array is a 1D array of length WIDTH*HEIGHT.)
 */
void load_image(char *filename, unsigned char *img) {
    // Open corresponding image file, read in header (which we will discard)
    FILE *f2 = fopen(filename, "r");
    if (f2 == NULL) {
        perror("fopen");
        exit(1);
    }
	int width;
    int height;
    
    if (fscanf(f2, "P2 %d %d 255", &width, &height) != 2){
        printf("file error");
    }
    int i = 0;
    unsigned char value;
    while (fscanf(f2, "%hhu", &value) == 1) {
        img[i] = value;
        i++;
    }

    fclose(f2);
}


/**
 * Load a full dataset into a 2D array called dataset.
 *
 * The image files to load are given in filename where
 * each image file name is on a separate line.
 * 
 * For each image i:
 *  - read the pixels into row i (using load_image)
 *  - set the image label in labels[i] (using get_label)
 * 
 * Return number of images read.
 */
int load_dataset(char *filename, 
                 unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                 unsigned char *labels) {
    // We expect the file to hold up to MAX_SIZE number of file names
    FILE *f1 = fopen(filename, "r");
    if (f1 == NULL) {
        perror("fopen");
        exit(1);
    }
    int i = 0;
    while (!feof(f1)){
        char nameOf_file[100];
        if (fscanf(f1, "%s", nameOf_file) == 1){;
            load_image(nameOf_file, dataset[i]);
            labels[i] = get_label(nameOf_file);
            i++;
        }    
    }
    fclose(f1);
    return i;
}

/** 
 * Return the euclidean distance between the image pixels in the image
 * a and b.  (See handout for the euclidean distance function)
 */
double distance(unsigned char *a, unsigned char *b) {

    int size1 = NUM_PIXELS;
    
    double diff = 0;
    for (int i = 0; i < size1; i++){
        diff += (a[i] - b[i]) *(a[i] - b[i]);
    }
    diff = sqrt(diff);
    return diff;
}

/** Return the most frequent int in arr.
        In the case of a tie, return the smallest value digit. 
*/ 
int max_count_num(unsigned char * arr, int len){   
        
    int i, j;
    int m; 
    int max[len];

	for (i = 0; i < len; i++) {
		max[i] = 0;
	}

	for (i = 0; i < len; i++) {
		for (j = i; j < len; j++) {
			if (arr[i] == arr[j]) {
				max[i]++;
			}
		}
	}
	m = 0;
	for (i = 0; i < len; i++) {
		if (max[m] < max[i]) {
			m = i;
		}   
        else if ((max[m] = max[i]) & (arr[m] > arr[i])){
            m = i;
        } 
        
    }
    return arr[m];
}


/**
 * Return the most frequent label of the K most similar images to "input"
 * in the dataset
 * Parameters:
 *  - input - an array of NUM_PIXELS unsigned chars containing the image to test
 *  - K - an int that determines how many training images are in the 
 *        K-most-similar set
 *  - dataset - a 2D array containing the set of training images.
 *  - labels - the array of labels that correspond to the training dataset
 *  - training_size - the number of images that are actually stored in dataset
 * 
 * Steps
 *   (1) Find the K images in dataset that have the smallest distance to input
 *         When evaluating an image to decide whether it belongs in the set of 
 *         K closest images, it will only replace an image in the set if its
 *         distance to the test image is strictly less than all of the images in 
 *         the current K closest images.
 *   (2) Count the frequencies of the labels in the K images
 *   (3) Return the most frequent label of these K images
 *         In the case of a tie, return the smallest value digit.
 */ 

int knn_predict(unsigned char *input, int K,
                unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                unsigned char *labels,
                int training_size) {

    double sample[K];
    unsigned char labelsOf_sample[K];
    
    int size = training_size;
    double diff;
    
    for (int i = 0; i < size; i++) {
        diff = distance(input, dataset[i]);
        if (i < K){
            sample[i] = diff;
            labelsOf_sample[i] = labels[i];
        }
        else{
            double max = diff;
            int max_index = -1;
            for(int j = 0; j < K; j++){
                if (sample[j] > max){
                    max = sample[j];
                    max_index = j;
                }
            }     
            if (max_index != -1) {
                sample[max_index] = diff;
                labelsOf_sample[max_index] = labels[i];
            }
        }
    } 
    int return_value = max_count_num(labelsOf_sample, K);
    return return_value;
}

