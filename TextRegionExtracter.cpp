#include "TextRegionExtracter.h"

TextRegionExtracter::TextRegionExtracter()
{

}

TextRegionExtracter::~TextRegionExtracter()
{

}

int TextRegionExtracter::extractTextRegion(const Mat &biImg, vector<Rect> &txtPos)
{
	// Prepare to find contours
	vector<vector<Point> > contours;
	
	int mode = CV_RETR_EXTERNAL;
	int method = CV_CHAIN_APPROX_SIMPLE;

	namedWindow("biImg", WINDOW_NORMAL);
	imshow("biImg", biImg);
	waitKey(0);
	// Find the contours

	findContours( biImg, contours, mode, method, Point(0,0) );

	Mat drawing = Mat::zeros(biImg.size(), CV_8UC3);
	RNG rng(12345);
	vector<Vec4i> hierarchy;
	for( int i = 0; i < contours.size(); ++i )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
	}
	/*
	vector<Vec4i> lines;
	HoughLinesP(biImg, lines, 1, CV_PI/180, 20, 10, 2 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
	  Vec4i l = lines[i];
	  line( drawing, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,0), 1, CV_AA);
	}*/
	namedWindow("drawing", WINDOW_NORMAL);
	imshow("drawing", drawing);
	waitKey(0);

	vector<TextBox> rects;
	rects.reserve(100);
	unsigned int levelCounter = 1;

	for( int i = 0; i < contours.size(); ++i )
	{
		Rect BoundingBox = boundingRect( contours[i] );
		if( !isTextArea( BoundingBox, contours[i]) )//not a text area ,skip
			continue;

		TextBox Box;
		Box.p0 = Point( BoundingBox.x , BoundingBox.y );
		Box.p2 = Point( Box.p0.x + BoundingBox.width , Box.p0.y + BoundingBox.height );
		Box.width = BoundingBox.width;
		Box.height = BoundingBox.height;
		Box.level = 0;

		if( rects.size() == 0 )
		{
			rects.push_back(Box);
			continue;
		}

		for( int n = 0 ; n < rects.size(); n++ )
		{
			if( isConnected(Box, rects[n]) )
			{
				setLevel( Box, rects[n], levelCounter );
			}
		}

		rects.push_back(Box);
	}

	// Contour scan finished
	// Determine the text area
	vector<TextBox> myCandi;
	for( int k=1 ; k<levelCounter ; k++ )
	{
		Point tmpP0 = Point(0,0);
		Point tmpP2 = Point(0,0);
		bool flag_find_k = false;

		for( int n=0 ; n<rects.size() ; n++ )
		{
			if( rects[n].level == k )
			{
				if( flag_find_k == false )
				{
					tmpP0.x = rects[n].p0.x;
					tmpP0.y = rects[n].p0.y;
					tmpP2.x = rects[n].p2.x;
					tmpP2.y = rects[n].p2.y;
					flag_find_k = true;
					continue;
				}
				else
				{
					if( rects[n].p0.x < tmpP0.x )
						tmpP0.x = rects[n].p0.x;
					if( rects[n].p0.y < tmpP0.y )
						tmpP0.y = rects[n].p0.y;
					if( rects[n].p2.x > tmpP2.x )
						tmpP2.x = rects[n].p2.x;
					if( rects[n].p2.y > tmpP2.y )
						tmpP2.y = rects[n].p2.y;
				}
			}

		}

		TextBox tmpBox;
		tmpBox.level = 0;
		tmpBox.p0.x = tmpP0.x;
		tmpBox.p0.y = tmpP0.y;
		tmpBox.p2.x = tmpP2.x;
		tmpBox.p2.y = tmpP2.y;

		myCandi.push_back(tmpBox);

	}
	//

	// Final steps to get candidates
	combineLines( myCandi );
	//vector<myt_Cand> Candidates = myGetCandi( myCandi , src );
	for( auto iter = myCandi.begin(); iter!= myCandi.end(); ++iter)
	{
		rectangle( drawing, iter->p0, iter->p2, Scalar( 0,255,0 ), 3 );

		txtPos.push_back( Rect( iter->p0, iter->p2 ) );
	}
	namedWindow("drawi", WINDOW_NORMAL);
	imshow("drawi", drawing);
	waitKey(0);

	return 0;
}

bool TextRegionExtracter::isTextArea( const Rect & bBox, const vector<Point> & contour)
{
	Rect BoundingBox = boundingRect( contour );
	if( BoundingBox.width>10 && BoundingBox.height>10 )
	{
		// Check the contour area with its bounding box area
		double ContArea = contourArea( contour );
		float AreaRatio = ContArea / ( BoundingBox.width * BoundingBox.height );
		if( AreaRatio < 0.9 && AreaRatio > 0.1 )
		{
			return true;
		}
	}
	return false;
}

void TextRegionExtracter::setLevel( TextBox & BoxL, TextBox & BoxR, unsigned int & levelCounter )
{
	if( BoxR.level == 0 && BoxL.level == 0 )
	{
		BoxR.level = levelCounter;
		BoxL.level = levelCounter;
		levelCounter++;
	}
	else if( BoxR.level != 0 && BoxL.level == 0 )
	{
		BoxL.level = BoxR.level;
	}
	else if( BoxR.level == 0 && BoxL.level != 0 )
	{
		BoxR.level = BoxL.level;
	}
	else if( BoxR.level != 0 && BoxL.level != 0 )
	{
		//save in pairS
	}
}
bool TextRegionExtracter::isConnected( const TextBox & BoxL, const TextBox &  BoxR )
{
	Point Bp0 = BoxL.p0;
	Point Bp2 = BoxL.p2;
	if( !nearRect( Bp0, Bp2, BoxR.p0 , BoxR.p2 ) )
		return false;
		
	if( !(abs(BoxL.p0.y - BoxR.p2.y) < max(BoxL.height, BoxR.height) || 
		abs(BoxL.p2.y - BoxR.p0.y) < max(BoxL.height, BoxR.height)) )
		return false;
	if( !(abs(BoxL.p0.x - BoxR.p2.x) < max(BoxL.height, BoxR.height) || 
		abs(BoxL.p2.x - BoxR.p0.x) < max(BoxL.height, BoxR.height)) )
		return false;
	return true;
}
/*
bool TextRegionExtracter::isConnected(const TextBox & BoxL, const TextBox &  BoxR)
{
	Point Bp0 = BoxL.p0;
	Point Bp2 = BoxL.p2;
	if( !nearRect( Bp0, Bp2, BoxR.p0 , BoxR.p2 ) )
		return false;
		

	CvPoint Ap0 = BoxR.p0;
	CvPoint Ap2 = BoxR.p2;
	// Area I:
	if( Bp0.x < Ap0.x && Bp0.y < Ap0.y )
	{
		if( Bp2.x <= Ap0.x || (Bp2.x <= Ap2.x && Bp2.y <= Ap0.y) )							// 1247
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				//Combine
				return true;
			}
		}
		else if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y <= Ap2.y && Bp2.y >= Ap0.y )		// 5
		{
			if( Bp2.x <= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
			else if( Bp2.x >= (Ap0.x+Ap2.x)/2 && bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				return true;
			}
			else if( (Ap0.x <= (Bp0.x+Bp2.x)/2 || Ap0.y <= (Bp0.y+Bp2.y)/2) && bigSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// B belongs to A
				return true;
			}
		}
		else if( Bp2.x >= Ap2.x && Bp2.y >= Ap0.y && Bp2.y <= Ap2.y )						// 6
		{
			if( Bp2.y >= (Ap0.y+Ap2.y)/2 && bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				return true;
			}
		}
		else if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )						// 8
		{
			if( Bp2.x <= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
			else if( Bp2.x >= (Ap0.x+Ap2.x)/2 && bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				return true;
			}
		}
		else if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )										// 9
		{
			if( bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				return true;
			}
		}
	}

	// Area II:
	else if( Bp0.x >= Ap0.x && Bp0.x < Bp2.x && Bp0.y < Ap0.y )
	{
		if( Bp2.x >= Ap2.x && Bp2.y <= Ap0.y )											// 3
		{
			if( Bp0.x >= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
		else if( Bp2.x >= Ap2.x && Bp2.y > Ap0.y )										// 69
		{
			if( Bp0.x >= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
			else if( Bp0.x <= (Ap0.x+Ap2.x)/2 && Bp2.y >= (Ap0.y+Ap2.y)/2 && bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				cout << "A --> B" << endl;
				return true;
			}
		}
		else if( Bp2.x >= Ap0.x && Bp2.y >= Ap0.y && Bp2.y <= Ap2.y )						// 56
		{
			if( Ap0.y <= (Bp0.y+Bp2.y)/2 )
				if( (Bp2.x <= Ap2.x || (Bp2.x >= Ap2.x && Ap2.x >= (Bp0.x+Bp2.x)/2)) && bigSize(Ap0,Ap2,Bp0,Bp2) )
				{
					// B belongs to A
					return true;
				}
		}
	}
	// Area III:
	else if( Bp0.x >= Ap2.x && Bp0.y < Ap0.y )
	{
		// BP2 inside area 369 + similar size ---> combine
		if( Bp2.x >= Ap2.x )																// 369
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
	}
	// Area IV:
	else if( Bp0.x <= Ap0.x && Bp0.y >= Ap0.y && Bp0.y <= Ap2.y )
	{
		if( Bp2.x <= Ap0.x && Bp2.y >= Ap0.y )											// 47
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
		else if( Bp2.y >= Ap0.y && Bp2.x >= Ap0.x && Bp2.x <= Ap2.x )						// 58
		{
			if( Bp2.x <= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
			else if( Ap0.x <= (Bp0.x+Bp2.x)/2 && Ap0.y >= (Bp0.y+Bp2.y)/2 && bigSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// B belongs to A
				return true;
			}
		}
		else if( Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )										// 89
		{
			if( Bp0.y <= (Ap0.y+Ap2.y)/2 && bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				return true;
			}
		}
	}

	// Area V:
	else if( Bp0.x >= Ap0.x && Bp0.x < Ap2.x && Bp0.y >= Ap0.y && Bp0.y < Ap0.y )
	{
		if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y <= Ap2.y && Bp2.y >= Ap0.y )			// 5
		{
			if( bigSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// B belongs to A
				return true;
			}
		}
		else if( Bp2.x >= Ap2.x && Bp2.y >= Ap0.y && Bp2.y <= Ap2.y )						// 6
		{
			if( Bp0.x >= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
			else if( Ap2.x >= (Bp0.x+Bp2.x)/2 && bigSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// B belongs to A
				return true;
			}
		}
		else if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )						// 8
		{
			if( Ap2.y >= (Bp0.y+Bp2.y)/2 && bigSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// B belongs to A
				return true;
			}
		}
		else if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )										// 9
		{
			if( Bp0.x >= (Ap0.x+Ap2.x)/2 && simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
			else if( Bp0.x <= (Ap0.x+Ap2.x)/2 && Bp0.y <= (Ap0.y+Ap2.y)/2 && bigSize(Bp0,Bp2,Ap0,Ap2) )
			{
				// A belongs to B
				return true;
			}
			else if( Ap2.x >= (Bp0.x+Bp2.x)/2 && Ap2.y >= (Bp0.y+Bp2.y)/2 && bigSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// B belongs to A
				return true;
			}
		}
	}

	// Area VI:
	else if( Bp0.x >= Ap2.x && Bp0.y >= Ap0.y && Bp0.y < Ap2.y )
	{
		// BP2 inside area 69 + similar size ---> combine
		if( Bp2.x >= Ap2.x && Bp2.y >= Ap0.y ) 											// 69
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
	}

	// Area VII:
	else if( Bp0.x < Ap0.x && Bp0.y >= Ap0.y )
	{
		// BP2 inside area 78 + similar size ---> combine
		if( Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )												// 78
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
	}

	// Area VIII:
	else if( Bp0.x >= Ap0.x && Bp0.x < Ap2.x && Bp0.y >= Ap2.y )
	{
		// BP2 inside area 9 (right) + similar size ---> combine
		if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
	}

	// Area IX:
	else if( Bp0.x >= Ap2.x && Bp0.y >= Ap2.y )
	{
		// BP2 inside area 9 + similar size ---> combine
		if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )
		{
			if( simiSize(Ap0,Ap2,Bp0,Bp2) )
			{
				// Combine
				return true;
			}
		}
	}
}
*/
bool TextRegionExtracter::simiSize( Point a0 , Point a2 , Point b0 , Point b2 )
{
	int a_width = a2.x - a0.x;
	int a_height = a2.y - a0.y;
	int b_width = b2.x - b0.x;
	int b_height = b2.y - b0.y;

	// Start from width
	if( (a_width <= b_width && (b_width - a_width < a_width/2)) )
	{
		if( b_width - a_width < a_width/4 )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
		else if( a_height <= b_height && (b_height - a_height < a_height/2) || (b_height < a_height && (a_height - b_height < b_height/2)) )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
	}
	else if( a_width > b_width && (a_width - b_width < b_width/2) )
	{
		if( a_width - b_width < b_width/4 )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
		else if( a_height <= b_height && (b_height - a_height < a_height/2) || (b_height < a_height && (a_height - b_height < b_height/2)) )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
	}

	// or start from height
	if( (a_height <= b_height && (b_height - a_height < a_height/2)) )
	{
		if( b_height - a_height < a_height/4 )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
		else if( a_width <= b_width && (b_width - a_width < a_width/2) || (b_width < a_width && (a_width - b_width < b_width/2)) )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
	}
	else if( (a_height > b_height && (b_height - a_height < a_height/2)) )
	{
		if( a_height - b_height < b_height/4 )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
		else if( a_width <= b_width && (b_width - a_width < a_width/2) || (b_width < a_width && (a_width - b_width < b_width/2)) )
		{
			cout << "		simiSize: true" << endl;
			return true;
		}
	}


	return false;
}

// Box A much bigger size than B ??
bool TextRegionExtracter::bigSize( Point a0 , Point a2 , Point b0 , Point b2 )
{
	int a_width = a2.x - a0.x;
	int a_height = a2.y - a0.y;
	int b_width = b2.x - b0.x;
	int b_height = b2.y - b0.y;

	if( a_width > b_width && a_height > b_height )
		if( (a_width - b_width >= b_width) || (a_height - b_height >= b_height) )
		{
			cout << "bigSize: " << a0.x << "," << a0.y << "-" << a2.x << "," << a2.y << "Bigger" << endl;
			return true;
		}
	return false;
}

bool TextRegionExtracter::nearRect( Point a0 , Point a2 , Point b0 , Point b2 )
{
	int r1 = cvRound( ( a2.y - a0.y ) / 2 );
	int r2 = cvRound( ( b2.y - b0.y ) / 2 );

	// Adaptive neighbor distance
	int r = (r1 > r2) ? r2 : r1;
	if( r <= D_NREG ) r = D_NREG;

	//if( a0.x > b2.x + r || a2.x + r < b0.x || a0.y > b2.y + r || a2.y + r < b0.y )
	if( a0.y > b2.y + r || a2.y + r < b0.y )
		return false;
	else return true;
}

void TextRegionExtracter::combineLines( vector<TextBox> &vi )
{
	for( int i=0 ; i<vi.size() ; i++ )
	{
		int h1 = vi[i].p2.y - vi[i].p0.y;

		for( int j=i+1 ; j<vi.size() ; )
		{
			if( j == i )
			{
				j++;
				continue;
			}
			else
			{
				int h2 = vi[j].p2.y - vi[j].p0.y;
				// At same line
				if( (vi[i].p0.y > vi[j].p2.y) || (vi[i].p2.y < vi[j].p0.y) )
				{
					j++;
					continue;
				}
				else
				{
					// and have same size - count as a line
					if( abs(h1-h2) < h1/4 || abs(h1-h2) < h2/4 )
					{
						cout << "i=" << i << " j=" << j << " Combine (" << vi[i].p0.x << "/" << vi[i].p0.y << "-" << vi[i].p2.x << "/" << vi[i].p2.y << "--" << vi[j].p0.x << "/" << vi[j].p0.y
							<< "-" << vi[j].p2.x << "/" << vi[j].p2.y << ") into (";

						// Combine the candidate areas
						if( vi[i].p0.x > vi[j].p0.x ) vi[i].p0.x = vi[j].p0.x;
						if( vi[i].p2.x < vi[j].p2.x ) vi[i].p2.x = vi[j].p2.x;
						if( vi[i].p0.y > vi[j].p0.y ) vi[i].p0.y = vi[j].p0.y;
						if( vi[i].p2.y < vi[j].p2.y ) vi[i].p2.y = vi[j].p2.y;

						cout << vi[i].p0.x << "," << vi[i].p0.y << "-" << vi[i].p2.x << "," << vi[i].p2.y << ")" << endl;
						vi.erase( vi.begin() + j );
						// Ready for new loop
						if( j < i ) i--;
						j = 0;
						continue;
						// Mark the changed areas
						// vi[i].mark = h1;
						// vi[i].mark2 = true;
					}
					else j++;
				}
			}
		}
	}
}