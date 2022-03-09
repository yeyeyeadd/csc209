#include <stdio.h>
#include <stdlib.h>

/* Return a pointer to an array of two dynamically allocated arrays of ints.
   The first array contains the elements of the input array s that are
   at even indices.  The second array contains the elements of the input
   array s that are at odd indices.

   Do not allocate any more memory than necessary.
*/
int **split_array(const int *s, int length) {
    int size1;
    int size2;

    if(length % 2 == 0){
      size1 = length/2;
      size2 = length/2;
    }
    else {
      size1 = (length + 1)/2;
      size2 = (length - 1)/2;
    }

    int *f_arr = malloc(sizeof(int) * size1);
    int *s_arr = malloc(sizeof(int) * size2);

    int **return_arr = malloc(sizeof(int *) * 2);

    int f_index = 0;
    int s_index = 0;

    for (int i = 0; i < length; i++) {
    if ( i%2 == 0 )  {
      f_arr[f_index] = s[i];
      f_index += 1; 
    }
    else {
      s_arr[s_index] = s[i];
      s_index += 1;
    }   
  }
  return_arr[0] = f_arr;
  return_arr[1] =  s_arr;
  return return_arr;


}

/* Return a pointer to an array of ints with size elements.
   - strs is an array of strings where each element is the string
     representation of an integer.
   - size is the size of the array
 */

int *build_array(char **strs, int size) {
  int *new_arr = malloc(size * sizeof(int)); 
  for (int i = 0; i < size; i++) {
    int s = strtol(strs[i], NULL, 10);
    new_arr[i] = s; 
  }
  return new_arr;

}


int main(int argc, char **argv) {
    /* Replace the comments in the next two lines with the appropriate
       arguments.  Do not add any additional lines of code to the main
       function or make other changes.
     */
    int size = argc - 1;
    char **array = argv + 1;
    int *full_array = build_array(array, size);
    int **result = split_array(full_array, size);

    printf("Original array:\n");
    for (int i = 0; i < argc - 1; i++) {
        printf("%d ", full_array[i]);
    }
    printf("\n");

    printf("result[0]:\n");
    for (int i = 0; i < argc / 2; i++) {
        printf("%d ", result[0][i]);
    }
    printf("\n");

    printf("result[1]:\n");
    for (int i = 0; i < (argc - 1) / 2; i++) {
        printf("%d ", result[1][i]);
    }
    printf("\n");
    free(full_array);
    free(result[0]);
    free(result[1]);
    free(result);
    return 0;
}
