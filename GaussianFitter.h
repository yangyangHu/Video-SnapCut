#ifndef __GAUSSIABNFITTER__H__ 
#define __GAUSSIABNFITTER__H__ 

#include "Color.h"
#include "Global.h"

struct GaussianPDF
{
	Color mu;					// mean of the gaussian
	Real covariance[3][3];		// covariance matrix of the gaussian
	Real determinant;			// determinant of the covariance matrix
	Real inverse[3][3];			// inverse of the covariance matrix
	Real pi;					// weighting of this gaussian in the GMM.
};


// Helper class that fits a single Gaussian to color samples
class GaussianFitter
{
public:
	GaussianFitter();
	
	// Add a color sample
	void add(Color c);
	
	// Build the gaussian out of all the added color samples
	void finalize(GaussianPDF& g, unsigned int totalCount) const;
	
private:

	Color s;			// sum of r,g, and b
	Real  p[3][3];		// matrix of products (i.e. r*r, r*g, r*b), some values are duplicated.

	unsigned int count;	// count of color samples added to the gaussian
};

#endif