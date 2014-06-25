/**
 * File containing simple utility functions.
 *
 * Author: Tyler Allen
 * Author: Matthew Leeds
 * Version: 06/24/2014
 */

#include "utils.h"

double** arrayInit2d(unsigned long long x, unsigned long long y)
{
    double** array = new double*[x];
    for (unsigned long long i = 0; i < x; i++)
    {
        array[i] = new double[y]();
    }
    return array;
}

void arrayDel2d(double** array, unsigned long long x)
{
    for (unsigned long long i = 0; i < x; i++)
    {
        delete [] array[i];
    }
    delete [] array;
}
