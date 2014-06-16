/**
 * Concrete implementation of LP solution methods.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 */

#include "LPSolution.h"

/**
 * Constructor for LPSolution class.
 *
 * TODO: implement me
 */
LPSolution::LPSolution()
{
    
}

int LPSolution::CheckErrorCode()
{
    return errorCode;
}

void LPSolution::SetErrorCode(int code)
{
    errorCode = code;
}
