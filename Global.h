/*
 * GrabCut implementation source code Copyright(c) 2005-2006 Justin Talbot
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

typedef unsigned char uchar;
typedef unsigned int uint;

// Choose accuracy of computations (double doesn't seem to work for me yet.)
//#define USE_DOUBLE

#ifdef USE_DOUBLE
	#define Real double
#else
	#define Real float
#endif




#endif
