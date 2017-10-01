/*
 * GrabCut implementation source code Copyright(c) 2005-2006 Justin Talbot
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */

#ifndef GMM_H
#define GMM_H

#include "Color.h"
#include "GaussianFitter.h"

#include <vector>
using namespace std;

//#include "cv.h"


class GMM
{
public:

	// Initialize GMM with number of gaussians desired.
	GMM(unsigned int K);
	~GMM();

	unsigned int K() const { return mK; }

	// Returns the probability density of color c in this GMM
	Real p(Color c);

	// Returns the probability density of color c in just Gaussian k
	Real p(unsigned int i, Color c);

	int Build(double** data, uint nrows);

private:

	unsigned int mK;		// number of gaussians
	GaussianPDF* mGaussians;	// an array of K gaussians

	//friend void buildGMMs(GMM& backgroundGMM, GMM& foregroundGMM, Image<unsigned int>& components, const Image<Color>& image, const Image<SegmentationValue>& hardSegmentation);
	//friend void learnGMMs(GMM& backgroundGMM, GMM& foregroundGMM, Image<unsigned int>& components, const Image<Color>& image, const Image<SegmentationValue>& hardSegmentation);
};

//// Build the initial GMMs using the Orchard and Bouman color clustering algorithm
//void buildGMMs(GMM& backgroundGMM, GMM& foregroundGMM, Image<unsigned int>& components, const Image<Color>& image, const Image<SegmentationValue>& hardSegmentation);
//
//// Iteratively learn GMMs using GrabCut updating algorithm
//void learnGMMs(GMM& backgroundGMM, GMM& foregroundGMM, Image<unsigned int>& components, const Image<Color>& image, const Image<SegmentationValue>& hardSegmentation);




#endif //GMM_H