/*
*author  : wilson
*version : 1
*time    : 2015-03-14
*description:this is a class design for image text recognition
*
*data field:
*
*function field:
*imgTextRecg   : run tesseract to recognize text 
*
*/
#ifndef OCR_UTILITY_H
#define OCR_UTILITY_H

#include <iostream>
#include <vector>

#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#include "OCRResultType.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

class OCRUtil
{
public:
	OCRUtil();
	~OCRUtil();
public:
	bool init();
	int recognize( const Mat &biImg, const vector<Rect> &txtPos );
	void getRecogedResult( vector<OCRResultType>& rcgRst );
	/* data */
protected:
	tesseract::TessBaseAPI*		m_pOCR;
	vector<OCRResultType>		m_recogRst;
};

#endif
