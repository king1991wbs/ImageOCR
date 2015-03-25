#include "TextRecognizer.h"

TextRecognizer::TextRecognizer():m_pRecognizerUtil(NULL),m_pTxtRgnExtracter(NULL)
{

}

TextRecognizer::~TextRecognizer()
{
	delete m_pRecognizerUtil;
	delete m_pTxtRgnExtracter;
}

bool TextRecognizer::init()
{
	//initialize member object
	m_pRecognizerUtil = new OCRUtil();
	if( m_pRecognizerUtil->init() )
	{
		cout << "initialize OCRUtil failed!" << endl;
		return 1;
	}

	m_pTxtRgnExtracter = new TextRegionExtracter();
	//m_pTxtRgnExtracter->init();

	return 0;
}

bool TextRecognizer::recognize( const Mat & image )
{
	Mat binaryImg(image.size(), CV_8UC1);//use for ocr engine
	vector<Rect> txtPos;//record text coordinate int image
	bool rStatus;

	imgPreProcess(image, binaryImg);
	Mat tmpBiImg = binaryImg.clone();
	
	rStatus = m_pTxtRgnExtracter->extractTextRegion(binaryImg, txtPos);

	if(rStatus == RECOGNIZING_FAILED) return RECOGNIZING_FAILED;
	//rStatus = m_pRecognizerUtil->recognize(binaryImg, txtPos);
	rStatus = m_pRecognizerUtil->recognize(tmpBiImg, txtPos);


	if(rStatus == RECOGNIZING_FAILED) return RECOGNIZING_FAILED;

	return RECOGNIZING_SUCCEED;
}

void TextRecognizer::getRecognizedResult( vector<OCRResultType> & rcgRst )
{
	m_pRecognizerUtil->getRecogedResult(rcgRst);
}

void TextRecognizer::imgPreProcess(const Mat &image, Mat & binaryImg )
{
	Mat grayImg(image.size(), CV_8UC1);

	cvtColor(image, grayImg, CV_RGB2GRAY);

	//GaussianBlur(grayImg, grayImg, Size(3,3), 0, 0);
	blur( grayImg, grayImg, Size(3,3) );

	imshow("grayImg", grayImg);
	waitKey(0);

	threshold(grayImg, binaryImg, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	//imshow("binaryImg", binaryImg);
	//waitKey(0);
	//adaptiveThreshold(src_gray, biImg, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 3, 0);
}