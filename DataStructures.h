#pragma once

#include "Global.h"

#include <vector>
using namespace std;

#define Epsilon 0.0001f

#define INF 1000000000

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

typedef unsigned int uint;

typedef unsigned char uchar;

// User supplied Trimap values
enum TrimapValue { TrimapUnknown, TrimapForeground, TrimapBackground };

// Grabcut derived hard segementation values
enum SegmentationValue { SegmentationForeground, SegmentationBackground };

struct PointI
{
	PointI(int _x=0, int _y=0):x(_x),y(_y){}
	int x;
	int y;
};

struct PointF
{
	PointF(float _x=0, float _y=0):x(_x),y(_y){}
	float x;
	float y;
};

struct Scribble
{
	vector<PointI> mPoints;
};

struct PolygonF
{
	vector<PointF> mPoints;
};

// Storage for N-link weights, each pixel stores links to only four of its 8-neighborhood neighbors.
// This avoids duplication of links, while still allowing for relatively easy lookup.
struct NLinks
{
	Real DownLeft;
	Real Down;
	Real DownRight;
	Real Right;
};