/*
*author  : wilson
*version : 1
*time    : 2015-03-14
*description:this is a class design for image text location
*
*data field:
*
*function field:
*imgTextLocat  : find text area and saved in vector
*
*/
#ifndef TEXT_REGION_EXTRACTER_H
#define	TEXT_REGION_EXTRACTER_H

#include <iostream>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "OCRResultType.h"

using namespace std;
using namespace cv;

#define D_NREG 10

struct TextBox
{
	unsigned int level;
	bool belonged;//is already belonged to some other box
	Point p0;//top left
	Point p2;//bottom right
};

class TextRegionExtracter
{
public:
	TextRegionExtracter();
	~TextRegionExtracter();
public:
	int extractTextRegion(const Mat &biImg, vector<Rect> &txtPos);
private:
	bool mySimiSize( Point a0 , Point a2 , Point b0 , Point b2 );
	bool myBigSize( Point a0 , Point a2 , Point b0 , Point b2 );
	bool myNearRect( Point a0 , Point a2 , Point b0 , Point b2 );
	void myCombineLines( vector<TextBox> &vi );

protected:
	/* data */
};

#endif