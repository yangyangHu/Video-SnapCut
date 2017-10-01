

#include "Color.h"


// Helper function, finds distance between two pixels
Real Distance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	return sqrt((Real)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

Real ColorDistance2( const Color& c1, const Color& c2 )
{
	return ((c1.r-c2.r)*(c1.r-c2.r)+(c1.g-c2.g)*(c1.g-c2.g)+(c1.b-c2.b)*(c1.b-c2.b));
}





