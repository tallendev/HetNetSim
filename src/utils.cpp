/**
 * File containing simple utility functions.
 *
 * Author: Tyler Allen
 * Author: Matthew Leeds
 * Version: 06/24/2014
 */

#include "utils.h"


/**
 * This function initializes a two-dimensional array dynamically. To prevent
 * memory leaks, call the delete function when you are finished with it.
 *
 * Param: x - the length of the array.
 * Param: y - the width of the array.
 *
 * return - a pointer to the newly initialized 2d array.
 */
double** arrayInit2d(unsigned long long x, unsigned long long y)
{
    double** array = new double*[x];
    for (unsigned long long i = 0; i < x; i++)
    {
        array[i] = new double[y]();
    }
    return array;
}


/**
 * Deletes a previously allocated 2d array. In order to appropriately 
 * deallocate in the rows, it is necessary to have the length of the outer 
 * array.
 *
 * Param: array - the array to deallocate.
 * Param: x - the number of inner rows to deallocate.
 */
void arrayDel2d(double** array, unsigned long long x)
{
    for (unsigned long long i = 0; i < x; i++)
    {
        delete [] array[i];
    }
    delete [] array;
}
