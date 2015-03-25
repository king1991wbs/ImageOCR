#include "OCRUtility.h"

OCRUtil::OCRUtil():m_pOCR(NULL)
{

}

OCRUtil::~OCRUtil()
{
	m_pOCR->End();
}

bool OCRUtil::init()
{
	m_pOCR = new tesseract::TessBaseAPI();
	if( m_pOCR->Init( NULL , "chi_sim" , tesseract::OEM_DEFAULT ) )
	{
		// Error : Could not initialize tesseract
		cout << "Error : Could not initialize tesseract." << endl;
		return 1;
	}

	return 0;
}

//not nessary to be binary image,the key point is to give a correct text region
int OCRUtil::recognize( const Mat &biImg, const vector<Rect> &txtPos )
{
	Mat tarImg;
	for(int i = 0; i < txtPos.size(); ++i)
	{
		biImg(txtPos[i]).copyTo(tarImg);
		OCRResultType ort;
		//initialize OCR engine source image
		m_pOCR->SetImage( tarImg.ptr(),
				tarImg.cols,
				tarImg.rows, 
				tarImg.channels(),
				tarImg.cols*tarImg.channels() );

		//get ocr text
		char *outText =  m_pOCR->GetUTF8Text();
		/*
		if(outText)
		{
			ort.m_textPos = txtPos[i];
			ort.m_rcgRst.assign(outText);
			m_recogRst.push_back(ort);
		}
		*/
		ort.m_textPos = txtPos[i];
		ort.m_rcgRst.assign(outText);
		ort.m_flag = outText ? RECOGNIZING_SUCCEED : RECOGNIZING_FAILED;
		m_recogRst.push_back(ort);
	}

	return RECOGNIZING_SUCCEED;
}

void OCRUtil::getRecogedResult( vector<OCRResultType>& rcgRst )
{
	rcgRst = m_recogRst;
}