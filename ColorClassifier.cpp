#include "ColorClassifier.h"
#include "VideoSnapCut.h"

ColorClassifier::ColorClassifier(void)
{
	mForegroundGMM = 0;
	mBackgroundGMM = 0;	
}

ColorClassifier::~ColorClassifier(void)
{
	FreeGMM();
}

void ColorClassifier::FreeGMM()
{
	if(mForegroundGMM)
	{
		delete mForegroundGMM;
		mForegroundGMM = 0;
	}
	if(mBackgroundGMM)
	{
		delete mBackgroundGMM;
		mBackgroundGMM = 0;
	}

}
int ColorClassifier::Build()
{
	DataImage<PixelClassifier>& dataImagePixelClassifier = *mDataImagePixelClassifier;

	vector<Color> foregroundColors;
	vector<Color> backgroundColors;

	int maskSize2 = VideoSnapCut::mMaskSize2;

	float boundingBoxMin[2];
	float boundingBoxMax[2];
	boundingBoxMin[0] = max(mBoundingBoxCenter[0]-maskSize2, 0);
	boundingBoxMin[1] = min(mBoundingBoxCenter[1]-maskSize2, mImageF.rows);
	boundingBoxMax[0] = max(mBoundingBoxCenter[0]+maskSize2,0);
	boundingBoxMax[1] = min(mBoundingBoxCenter[1]+maskSize2, mImageF.cols);

	for(uint y=(uint)boundingBoxMin[1]; y<=(uint)boundingBoxMax[1]; y++)
	{
		for(uint x=(uint)boundingBoxMin[0]; x<=(uint)boundingBoxMax[0]; x++)
		{
			Vec3f intensity = mImageF.at<Vec3f>(y, x);
			Color c(intensity.val[2],intensity.val[1],intensity.val[0]);
			if(mDistanceTransform.at<float>(y,x)>5)	
			{
				if(mCombinedMask.at<uchar>(y,x))	
				{
					foregroundColors.push_back(c);
				}
				else
				{
					backgroundColors.push_back(c);
				}
			}
		}
	}

	if( (int)backgroundColors.size() < VideoSnapCut::mK || (int)foregroundColors.size() < VideoSnapCut::mK )
	{
		return 0;
	}

	FreeGMM();

	uint i, j, nrows;
	double** data ;
	nrows = foregroundColors.size();
	data = (double**)malloc(nrows*sizeof(double*));
	for (i = 0; i < nrows; i++) data[i] = (double*)malloc(3*sizeof(double));

	//	copy the data from the color array to a temp array 
	//	and assin each sample a random cluster id
	
	for (j = 0; j < nrows; j++)
	{
		data[j][0] = foregroundColors[j].r;
		data[j][1] = foregroundColors[j].g;
		data[j][2] = foregroundColors[j].b;			
	}

	mForegroundGMM = new GMM(VideoSnapCut::mK);
	mForegroundGMM->Build(data,nrows);

	for (i = 0; i < nrows; i++) free(data[i]);
	free(data);

	nrows = backgroundColors.size();
	data = (double**)malloc(nrows*sizeof(double*));
	for (i = 0; i < nrows; i++) data[i] = (double*)malloc(3*sizeof(double));

	//	copy the data from the color array to a temp array 
	//	and assin each sample a random cluster id
	
	for (j = 0; j < nrows; j++)
	{
		data[j][0] = backgroundColors[j].r;
		data[j][1] = backgroundColors[j].g;
		data[j][2] = backgroundColors[j].b;			
	}

	mBackgroundGMM = new GMM(VideoSnapCut::mK);
	mBackgroundGMM->Build(data,nrows);

	for (i = 0; i < nrows; i++) free(data[i]);
	free(data);
	

	//	calculate fc

	float Wc_x_Sum = 0;
	float Wk_Sum = 0;

	for(uint y=(uint)boundingBoxMin[1]; y<=(uint)boundingBoxMax[1]; y++)
	{
		for(uint x=(uint)boundingBoxMin[0]; x<=(uint)boundingBoxMax[0]; x++)
		{
			float D_x = mDistanceTransform.at<float>(y,x);//dataImageDistTransform(x,y);
			if(D_x > 5)
			{
				Vec3f intensity = mImageF.at<Vec3f>(y, x);
				Color c(intensity.val[2],intensity.val[1],intensity.val[0]);
				float Pc_xF = mForegroundGMM->p(c);
				float Pc_xB = mBackgroundGMM->p(c);
				float Pc_x = Pc_xF / (Pc_xF + Pc_xB);
			
				
				float D_x2 = D_x * D_x;
				float dimgaC = (float)VideoSnapCut::mMaskSize2;
				float dimgaC2 = dimgaC*dimgaC;

				float L_x = 0;
				if(mCombinedMask.at<uchar>(y,x) == 1)
				{
					L_x = 1;
				}
				float Wc_x = exp(-D_x2/dimgaC2);
				Wk_Sum += fabs(1-Pc_x)*Wc_x;
				Wc_x_Sum += Wc_x;
			}//if(D_x > 5)
		}//for(uint x=(uint)boundingBoxMin[0]; x<=(uint)boundingBoxMax[0]; x++)
	}//for(uint y=(uint)boundingBoxMin[1]; y<=(uint)boundingBoxMax[1]; y++)

	mF_C = 1 - Wk_Sum/Wc_x_Sum;

	for(int y=(int)boundingBoxMin[1]; y<=(int)boundingBoxMax[1]; y++)
	{
		for(int x=(int)boundingBoxMin[0]; x<=(int)boundingBoxMax[0]; x++)
		{
			int& n = dataImagePixelClassifier(x,y).mNumClassifiers;
			const int maxN = VideoSnapCut::mMaxColorClassifiers;
			if(n < maxN)
			{
				dataImagePixelClassifier(x,y).mColorClassifier[n] = this;	
				n++;

			}
		}//for(int x=boundingBoxMin[0]; x<=boundingBoxMax[0]; x++)
	}//for(int y=boundingBoxMin[1]; y<=boundingBoxMax[1]; y++)

	return 1;

}