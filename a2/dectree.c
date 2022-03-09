/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC209H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Mustafa Quraish, Bianca Schroeder, Karen Reid
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2021 Karen Reid
 */

#include "dectree.h"

/**
 * Load the binary file, filename into a Dataset and return a pointer to 
 * the Dataset. The binary file format is as follows:
 *
 *     -   4 bytes : `N`: Number of images / labels in the file
 *     -   1 byte  : Image 1 label
 *     - NUM_PIXELS bytes : Image 1 data (WIDTHxWIDTH)
 *          ...
 *     -   1 byte  : Image N label
 *     - NUM_PIXELS bytes : Image N data (WIDTHxWIDTH)
 *
 * You can set the `sx` and `sy` values for all the images to WIDTH. 
 * Use the NUM_PIXELS and WIDTH constants defined in dectree.h
 */
Dataset *load_dataset(const char *filename) {
    // TODO: Allocate data, read image data / labels, return
    FILE *f1 = fopen(filename, "r");
    if (f1 == NULL) {
        perror("fopen");
        exit(1);
    }
    int readed = 0;
    int numberOfImage;
    if (fread(&numberOfImage, sizeof(int), 1, f1) == 1){
        Dataset* dataset = (Dataset*)malloc(sizeof(Dataset));
        dataset->num_items = numberOfImage;
        dataset->images = malloc(sizeof(Image) * numberOfImage);
        dataset->labels = malloc(sizeof(char) * numberOfImage);
        
        for (int i = 0; i < numberOfImage; i++){
            readed += fread(&((dataset->labels)[i]), sizeof(char), 1, f1);
            dataset->images[i].data = malloc(sizeof(char) * NUM_PIXELS);
            dataset->images[i].sx = WIDTH;
            dataset->images[i].sy = WIDTH;
            readed += fread((dataset->images)[i].data, sizeof(unsigned char), NUM_PIXELS, f1);
        }
        return dataset;
    }    
    else{
        perror("wrong file");
        exit(1);
    }
    fclose(f1);
    return NULL;
}

/**
 * Compute and return the Gini impurity of M images at a given pixel
 * The M images to analyze are identified by the indices array. The M
 * elements of the indices array are indices into data.
 * This is the objective function that you will use to identify the best 
 * pixel on which to split the dataset when building the decision tree.
 *
 * Note that the gini_impurity implemented here can evaluate to NAN 
 * (Not A Number) and will return that value. Your implementation of the 
 * decision trees should ensure that a pixel whose gini_impurity evaluates 
 * to NAN is not used to split the data.  (see find_best_split)
 * 
 * DO NOT CHANGE THIS FUNCTION; It is already implemented for you.
 */
double gini_impurity(Dataset *data, int M, int *indices, int pixel) {
    int a_freq[10] = {0}, a_count = 0;
    int b_freq[10] = {0}, b_count = 0;

    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];

        // The pixels are always either 0 or 255, but using < 128 for generality.
        if (data->images[img_idx].data[pixel] < 128) {
            a_freq[data->labels[img_idx]]++;
            a_count++;
        } else {
            b_freq[data->labels[img_idx]]++;
            b_count++;
        }
    }

    double a_gini = 0, b_gini = 0;
    for (int i = 0; i < 10; i++) {
        double a_i = ((double)a_freq[i]) / ((double)a_count);
        double b_i = ((double)b_freq[i]) / ((double)b_count);
        a_gini += a_i * (1 - a_i);
        b_gini += b_i * (1 - b_i);
    }

    // Weighted average of gini impurity of children
    return (a_gini * a_count + b_gini * b_count) / M;
}

/**
 * Given a subset of M images and the array of their corresponding indices, 
 * find and use the last two parameters (label and freq) to store the most
 * frequent label in the set and its frequency.
 *
 * - The most frequent label (between 0 and 9) will be stored in `*label`
 * - The frequency of this label within the subset will be stored in `*freq`
 * 
 * If multiple labels have the same maximal frequency, return the smallest one.
 */
void get_most_frequent(Dataset *data, int M, int *indices, int *label, int *freq) {
    // TODO: Set the correct values and return
    int i, j;
    int m; 
    int max[M];
    for (i = 0; i < M; i++) {
		max[i] = 0;
	}

    for (i = 0; i < M; i++) {
		for (j = i; j < M; j++) {
			if (data->labels[indices[i]] == data->labels[indices[j]]) {
				max[i]++;
			}
		}
	}
    m = 0;
	for (i = 0; i < M; i++) {
		if (max[m] < max[i]) {
			m = i;
		}   
        else if ((max[m] == max[i]) && (data->labels[indices[m]] > data->labels[indices[i]])){
            m = i;
        }  
    }
    *label = data->labels[indices[m]];
    *freq = max[m];
    return;
}

/**
 * Given a subset of M images as defined by their indices, find and return
 * the best pixel to split the data. The best pixel is the one which
 * has the minimum Gini impurity as computed by `gini_impurity()` and 
 * is not NAN. (See handout for more information)
 * 
 * The return value will be a number between 0-783 (inclusive), representing
 *  the pixel the M images should be split based on.
 * 
 * If multiple pixels have the same minimal Gini impurity, return the smallest.
 */
int find_best_split(Dataset *data, int M, int *indices) {
    int splitAt = 0;
    double giniValue = 100;
    for (int i = 0; i < NUM_PIXELS; i++){
        double result = gini_impurity(data, M, indices, i);
        if ((!isnan(result)) && (giniValue > result)){
            giniValue = result;
            splitAt = i;
        }
    }
    return splitAt;
}

/**
 * Create the Decision tree. In each recursive call, we consider the subset of the
 * dataset that correspond to the new node. To represent the subset, we pass 
 * an array of indices of these images in the subset of the dataset, along with 
 * its length M. Be careful to allocate this indices array for any recursive 
 * calls made, and free it when you no longer need the array. In this function,
 * you need to:
 *
 *    - Compute ratio of most frequent image in indices, do not split if the
 *      ration is greater than THRESHOLD_RATIO
 *    - Find the best pixel to split on using `find_best_split`
 *    - Split the data based on whether pixel is less than 128, allocate 
 *      arrays of indices of training images and populate them with the 
 *      subset of indices from M that correspond to which side of the split
 *      they are on
 *    - Allocate a new node, set the correct values and return
 *       - If it is a leaf node set `classification`, and both children = NULL.
 *       - Otherwise, set `pixel` and `left`/`right` nodes 
 *         (using build_subtree recursively). 
 */
DTNode *build_subtree(Dataset *data, int M, int *indices) {
    // TODO: Construct and return the tree
    int label;
    int freq;
    get_most_frequent(data, M, indices, &label, &freq);
    if ((((double)freq/M) > THRESHOLD_RATIO)){
        DTNode * node = malloc(sizeof(DTNode));
        node->classification = label;
        node->pixel = -1;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    else{
        int splitAt;
        splitAt = find_best_split(data, M, indices);
        int indexOfleft = 0;
        int indexOfright = 0;
        for (int i = 0; i < M; i++){
            if (data->images[indices[i]].data[splitAt] >= 128){
                indexOfright += 1;
            }
            else {
                indexOfleft += 1;
            }
        }
        int *left_array = malloc(sizeof(int) * indexOfleft);
        int *right_array = malloc(sizeof(int) * indexOfright);
        indexOfleft = 0;
        indexOfright = 0;
        for (int j = 0; j < M; j++){
            if (data->images[indices[j]].data[splitAt] >= 128){
                right_array[indexOfright] = indices[j];
                indexOfright += 1;
            }
            else {
                left_array[indexOfleft] = indices[j];
                indexOfleft += 1;
            }
        }
        DTNode* node = malloc(sizeof(DTNode));
        node->pixel = splitAt;
        node->classification = -1;
        node->left = build_subtree(data, indexOfleft, left_array);
        node->right = build_subtree(data, indexOfright, right_array);
        
        free(left_array);
        free(right_array);
        return node;
    }
}

/**
 * This is the function exposed to the user. All you should do here is set
 * up the `indices` array correctly for the entire dataset and call 
 * `build_subtree()` with the correct parameters.
 */
DTNode *build_dec_tree(Dataset *data) {
    // TODO: Set up `indices` array, call `build_subtree` and return the tree.
    // HINT: Make sure you free any data that is not needed anymore
    int M = data->num_items;
    int indices[M];
    for (int i = 0; i < M; i++){
        indices[i] = i;
    }
    DTNode *tree = build_subtree(data, M, indices);
    return tree;
}

/**
 * Given a decision tree and an image to classify, return the predicted label.
 */
int dec_tree_classify(DTNode *root, Image *img) {
    // TODO: Return the correct label
    int label;
    if (root->classification == -1){
        if(img->data[root->pixel] >= 128){
            label = dec_tree_classify(root->right, img);
        }
        else{
            label = dec_tree_classify(root->left, img);
        }
    }
    else {
        label = root->classification;
    }
    return label;
}

/**
 * This function frees the Decision tree.
 */
void free_dec_tree(DTNode *node) {
    // TODO: Free the decision tree
    if(node->left != NULL){
        free_dec_tree(node->left);
    }
    if(node->right != NULL){
        free_dec_tree(node->right);
    }
    free(node);
    return;
}

/**
 * Free all the allocated memory for the dataset
 */
void free_dataset(Dataset *data) {
    // TODO: Free dataset (Same as A1)
    for(int i = 0; i < (data->num_items); i++){
    free((data->images[i]).data);
    }
    free(data->images);
    free(data->labels);
    free(data);
    return;
}
