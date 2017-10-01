/*
 * GrabCut implementation source code Copyright(c) 2005-2006 Justin Talbot
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */

#include "GMM.h"
#include "cluster.h"

GMM::GMM(unsigned int K) : mK(K)
{
	mGaussians = new GaussianPDF[mK];
}

GMM::~GMM()
{
	if (mGaussians)
		delete [] mGaussians;
}

Real GMM::p(Color c)
{
	Real result = 0;

	if (mGaussians)
	{
		for (unsigned int i=0; i < mK; i++)
		{
			result += mGaussians[i].pi * p(i, c);
		}
	}

	return result;
}

Real GMM::p(unsigned int i, Color c)
{
	Real result = 0;

	if( mGaussians[i].pi > 0 )
	{
		if (mGaussians[i].determinant > 0)
		{
			Real r = c.r - mGaussians[i].mu.r;
			Real g = c.g - mGaussians[i].mu.g;
			Real b = c.b - mGaussians[i].mu.b;
			
			Real d = r * (r*mGaussians[i].inverse[0][0] + g*mGaussians[i].inverse[1][0] + b*mGaussians[i].inverse[2][0]) +
					g * (r*mGaussians[i].inverse[0][1] + g*mGaussians[i].inverse[1][1] + b*mGaussians[i].inverse[2][1]) +
					b * (r*mGaussians[i].inverse[0][2] + g*mGaussians[i].inverse[1][2] + b*mGaussians[i].inverse[2][2]);

			result = (Real)(1.0/(sqrt(mGaussians[i].determinant)) * exp(-0.5*d));
		}
	}

	return result;
}

//vector<Color> ColorData
//uint nrows = ColorData.size();
//double** data = (double**)malloc(nrows*sizeof(double*));
//for (i = 0; i < nrows; i++) data[i] = (double*)malloc(3*sizeof(double));
//	copy the data from the color array to a temp array 
//	and assin each sample a random cluster id
//for (j = 0; j < nrows; j++)
//{
//	data[j][0] = ColorData[j].r;
//	data[j][1] = ColorData[j].g;
//	data[j][2] = ColorData[j].b;			
//}
//for (i = 0; i < nrows; i++) free(data[i]);
//free(data);
	
int GMM::Build(double** data, uint nrows)
{
	uint i,j;
	int* clusterid = (int*)malloc(nrows*sizeof(int));
	
	//	run	k-means clustering
	const int ncols = 3;		
	const int nclusters = mK;
	const int transpose = 0;
	const char dist = 'e';
	const char method = 'a';
	int npass = 1;
	int ifound = 0;
	double error;
	double* weight = (double*)malloc(ncols*sizeof(double));
	int** mask = (int**)malloc(nrows*sizeof(int*));
	for (i = 0; i < nrows; i++)
	{
		mask[i] = (int*)malloc(ncols*sizeof(int));
		for (j = 0; j < ncols; j++) mask[i][j] = 1;
	}

	for (i = 0; i < ncols; i++) weight[i] = 1.0;
	
	kcluster(nclusters,nrows,ncols,data,mask,weight,transpose,npass,method,dist,clusterid, &error, &ifound);
	

	////	for debuging 
	//int* count;
	//count = (int*)malloc(nclusters*sizeof(int));
	//for (j = 0; j < nclusters; j++) count[j] = 0;
	//count[clusterid[j]]++;
	//free(count);

	//	build the GMM 
	GaussianFitter* gaussianFitter = new GaussianFitter[mK];

	for (i = 0; i < nrows; i++) gaussianFitter[clusterid[j]].add(Color((float)data[i][0],(float)data[i][1],(float)data[i][2]));

	for (i = 0; i < mK; i++) gaussianFitter[i].finalize(mGaussians[i], nrows);

	delete [] gaussianFitter;

	for (i = 0; i < nrows; i++) free(mask[i]);
	
	free(mask);	

	free(weight);
	
	free(clusterid);
	
	return 1;
}
