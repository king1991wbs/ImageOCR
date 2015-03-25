/*
*author  : wilson
*version : 1
*time    : 2015-03-14
*description:this is a class design for image text recognition
*
*data field:
*srcImg : input source image path for text recognition
*segText : place where you want to save the text recognized from image
*
*function field:
*recognize  : start recognition
*setData       : flexibly change or set srcImgFile,segTextFile,dstImgFile
*imgPreProcess : preprocess image, convert source image to binary image
*
*/
#ifndef TEXT_RECOGNIZER_H
#define TEXT_RECOGNIZER_H

#include <iostream>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "OCRUtility.h"
#include "TextRegionExtracter.h"
#include "OCRResultType.h"

using namespace std;
using namespace cv;

class  TextRecognizer
{
public:
	 TextRecognizer();
	~TextRecognizer();

public:
	bool init();
	bool recognize( const Mat & image );
	void getRecognizedResult( vector<OCRResultType> & rcgRst );
	
private:
	void imgPreProcess(const Mat &image, Mat & binaryImg );//convert input frame into binary image.

protected:
	OCRUtil *				m_pRecognizerUtil;
	TextRegionExtracter *	m_pTxtRgnExtracter;
	vector<OCRResultType> 	m_recogResult;
};

#endif