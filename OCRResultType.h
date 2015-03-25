#ifndef OCR_RESULT_TYPE_H
#define OCR_RESULT_TYPE_H

#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

enum ReconizingStatusType{
	RECOGNIZING_SUCCEED	= 0,
	RECOGNIZING_FAILED	= 1
	//add flag here.
};

struct OCRResultType{
	Rect					m_textPos;
	string					m_rcgRst;
	ReconizingStatusType	m_flag;////
};

#endif