ocr : testOCR.o TextRecognizer.o TextRegionExtracter.o OCRUtility.o
	g++ -g -std=c++11 -o ocr testOCR.o TextRecognizer.o TextRegionExtracter.o OCRUtility.o `pkg-config --cflags --libs opencv tesseract`
testOCR.o : testOCR.h testOCR.cpp
	g++ -g -std=c++11 -c testOCR.cpp `pkg-config --cflags --libs opencv tesseract`
TextRecognizer.o : TextRecognizer.h  TextRecognizer.cpp
	g++ -g -std=c++11 -c TextRecognizer.cpp `pkg-config --cflags --libs opencv tesseract`
TextRegionExtracter.o : TextRegionExtracter.h TextRegionExtracter.cpp
	g++ -g -std=c++11 -c TextRegionExtracter.cpp `pkg-config --cflags --libs opencv tesseract`
OCRUtility.o : OCRUtility.h OCRUtility.cpp
	g++ -g -std=c++11 -c OCRUtility.cpp `pkg-config --cflags --libs opencv tesseract`
clean :
	rm ocr testOCR.o TextRecognizer.o TextRegionExtracter.o OCRUtility.o