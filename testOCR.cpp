#include "testOCR.h"

int main(int argc, char ** argv)
{
	Mat src = imread(argv[1]);

	TextRecognizer tr;
	tr.init();
	tr.recognize(src);
	vector<OCRResultType> rstTxt;
	tr.getRecognizedResult(rstTxt);
	for( auto iter = rstTxt.begin(); iter != rstTxt.end(); ++iter )
	{
		cout << iter->m_rcgRst << endl;
	}

	return 0;
}
