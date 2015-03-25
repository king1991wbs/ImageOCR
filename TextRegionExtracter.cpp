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

	// Find the contours
	findContours( biImg, contours, mode, method, Point(0,0) );

	vector<TextBox> rects;
	rects.reserve(100);
	unsigned int levelCounter = 1;

	for( int i = 0; i < contours.size(); ++i )
	{
		Rect BoundingBox = boundingRect( contours[i] );
		if( BoundingBox.width>10 && BoundingBox.height>10
			&& BoundingBox.width<( biImg.cols*0.8 ) && BoundingBox.height<( biImg.rows*0.8 ) )
		{
			// Check the contour area with its bounding box area
			double ContArea = contourArea( contours[i] );
			float AreaRatio = ContArea / ( BoundingBox.width * BoundingBox.height );
			if( AreaRatio < 0.9 && AreaRatio > 0.1 )
			{
				if( rects.size() == 0 )
				{
					TextBox Box;
					Box.p0 = Point( BoundingBox.x , BoundingBox.y );
					Box.p2 = Point( Box.p0.x + BoundingBox.width , Box.p0.y + BoundingBox.height );
					Box.level = 0;
					Box.belonged = false;
					rects.push_back(Box);
				}
				else
				{
					Point Bp0 = Point( BoundingBox.x , BoundingBox.y );
					Point Bp2 = Point( Bp0.x + BoundingBox.width , Bp0.y + BoundingBox.height );
					// Point Bp1 = Point( Bp0.x + BoundingBox.width , Bp0.y );
					// Point Bp3 = Point( Bp0.x , Bp0.y + BoundingBox.height );
					TextBox Box;
					Box.p0 = Bp0;
					Box.p2 = Bp2;
					Box.level = 0;
					Box.belonged = false;
					bool flag_push = false;
					// unsigned short flag_proc = 0;

					for( int n=1 ; n<rects.size() ; n++ )
					{
						unsigned short flag_proc = 0;
						if( !(Bp0.x == rects[n].p0.x && Bp0.y == rects[n].p0.y && Bp2.x == rects[n].p2.x && Bp2.y == rects[n].p2.y) )
						{
							// Detect the near boxes
							if( myNearRect( Bp0 , Bp2 , rects[n].p0 , rects[n].p2 ) )
							{
								// Start to define situation and to operate

								Point Ap0 = rects[n].p0;
								Point Ap2 = rects[n].p2;
								
								// Area I:
								if( Bp0.x < Ap0.x && Bp0.y < Ap0.y )
								{
									if( Bp2.x <= Ap0.x || (Bp2.x <= Ap2.x && Bp2.y <= Ap0.y) )							// 1247
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											//Combine
											flag_proc = 1;
										}
									}
									else if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y <= Ap2.y && Bp2.y >= Ap0.y )		// 5
									{
										if( Bp2.x <= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
										else if( Bp2.x >= (Ap0.x+Ap2.x)/2 && myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											flag_proc = 2;
										}
										else if( (Ap0.x <= (Bp0.x+Bp2.x)/2 || Ap0.y <= (Bp0.y+Bp2.y)/2) && myBigSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// B belongs to A
											flag_proc = 3;
										}
									}
									else if( Bp2.x >= Ap2.x && Bp2.y >= Ap0.y && Bp2.y <= Ap2.y )						// 6
									{
										if( Bp2.y >= (Ap0.y+Ap2.y)/2 && myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											flag_proc = 2;
										}
									}
									else if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )						// 8
									{
										if( Bp2.x <= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
										else if( Bp2.x >= (Ap0.x+Ap2.x)/2 && myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											flag_proc = 2;
										}
									}
									else if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )										// 9
									{
										if( myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											flag_proc = 2;
										}
									}
								}

								// Area II:
								else if( Bp0.x >= Ap0.x && Bp0.x < Bp2.x && Bp0.y < Ap0.y )
								{
									if( Bp2.x >= Ap2.x && Bp2.y <= Ap0.y )											// 3
									{
										if( Bp0.x >= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
									else if( Bp2.x >= Ap2.x && Bp2.y > Ap0.y )										// 69
									{
										if( Bp0.x >= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
										else if( Bp0.x <= (Ap0.x+Ap2.x)/2 && Bp2.y >= (Ap0.y+Ap2.y)/2 && myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											cout << "A --> B" << endl;
											flag_proc = 2;
										}
									}
									else if( Bp2.x >= Ap0.x && Bp2.y >= Ap0.y && Bp2.y <= Ap2.y )						// 56
									{
										if( Ap0.y <= (Bp0.y+Bp2.y)/2 )
											if( (Bp2.x <= Ap2.x || (Bp2.x >= Ap2.x && Ap2.x >= (Bp0.x+Bp2.x)/2)) && myBigSize(Ap0,Ap2,Bp0,Bp2) )
											{
												// B belongs to A
												flag_proc = 3;
											}
									}
								}
								// Area III:
								else if( Bp0.x >= Ap2.x && Bp0.y < Ap0.y )
								{
									// BP2 inside area 369 + similar size ---> combine
									if( Bp2.x >= Ap2.x )																// 369
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
								}
								// Area IV:
								else if( Bp0.x <= Ap0.x && Bp0.y >= Ap0.y && Bp0.y <= Ap2.y )
								{
									if( Bp2.x <= Ap0.x && Bp2.y >= Ap0.y )											// 47
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
									else if( Bp2.y >= Ap0.y && Bp2.x >= Ap0.x && Bp2.x <= Ap2.x )						// 58
									{
										if( Bp2.x <= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
										else if( Ap0.x <= (Bp0.x+Bp2.x)/2 && Ap0.y >= (Bp0.y+Bp2.y)/2 && myBigSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// B belongs to A
											flag_proc = 3;
										}
									}
									else if( Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )										// 89
									{
										if( Bp0.y <= (Ap0.y+Ap2.y)/2 && myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											flag_proc = 2;
										}
									}
								}

								// Area V:
								else if( Bp0.x >= Ap0.x && Bp0.x < Ap2.x && Bp0.y >= Ap0.y && Bp0.y < Ap0.y )
								{
									if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y <= Ap2.y && Bp2.y >= Ap0.y )			// 5
									{
										if( myBigSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// B belongs to A
											flag_proc = 3;
										}
									}
									else if( Bp2.x >= Ap2.x && Bp2.y >= Ap0.y && Bp2.y <= Ap2.y )						// 6
									{
										if( Bp0.x >= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
										else if( Ap2.x >= (Bp0.x+Bp2.x)/2 && myBigSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// B belongs to A
											flag_proc = 3;
										}
									}
									else if( Bp2.x <= Ap2.x && Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )						// 8
									{
										if( Ap2.y >= (Bp0.y+Bp2.y)/2 && myBigSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// B belongs to A
											flag_proc = 3;
										}
									}
									else if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )										// 9
									{
										if( Bp0.x >= (Ap0.x+Ap2.x)/2 && mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
										else if( Bp0.x <= (Ap0.x+Ap2.x)/2 && Bp0.y <= (Ap0.y+Ap2.y)/2 && myBigSize(Bp0,Bp2,Ap0,Ap2) )
										{
											// A belongs to B
											flag_proc = 2;
										}
										else if( Ap2.x >= (Bp0.x+Bp2.x)/2 && Ap2.y >= (Bp0.y+Bp2.y)/2 && myBigSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// B belongs to A
											flag_proc = 3;
										}
									}
								}

								// Area VI:
								else if( Bp0.x >= Ap2.x && Bp0.y >= Ap0.y && Bp0.y < Ap2.y )
								{
									// BP2 inside area 69 + similar size ---> combine
									if( Bp2.x >= Ap2.x && Bp2.y >= Ap0.y ) 											// 69
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
								}

								// Area VII:
								else if( Bp0.x < Ap0.x && Bp0.y >= Ap0.y )
								{
									// BP2 inside area 78 + similar size ---> combine
									if( Bp2.x >= Ap0.x && Bp2.y >= Ap2.y )												// 78
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
								}

								// Area VIII:
								else if( Bp0.x >= Ap0.x && Bp0.x < Ap2.x && Bp0.y >= Ap2.y )
								{
									// BP2 inside area 9 (right) + similar size ---> combine
									if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
								}

								// Area IX:
								else if( Bp0.x >= Ap2.x && Bp0.y >= Ap2.y )
								{
									// BP2 inside area 9 + similar size ---> combine
									if( Bp2.x >= Ap2.x && Bp2.y >= Ap2.y )
									{
										if( mySimiSize(Ap0,Ap2,Bp0,Bp2) )
										{
											// Combine
											flag_proc = 1;
										}
									}
								}
							}
						}

						switch( flag_proc )
						{
							case 0:
							{
								break;
							}
							case 1:
							{
								rects[n].belonged = true;
								Box.belonged = true;
								if( rects[n].level == 0 && Box.level == 0 )
								{
									rects[n].level = levelCounter;
									Box.level = levelCounter;
									levelCounter++;
									rects.push_back(Box);
									flag_push = true;
								}
								else if( rects[n].level != 0 && Box.level == 0 )
								{
									Box.level = rects[n].level;
									rects.push_back(Box);
									flag_push = true;
								}
								else if( rects[n].level == 0 && Box.level != 0 )
								{
									rects[n].level = Box.level;
								}
								else if( rects[n].level != 0 && Box.level != 0 )
								{
									
								}
								break;
							}
							case 2:
							{
								if( rects[n].level == 0 && Box.level == 0 )
								{
									rects[n].level = levelCounter;
									Box.level = levelCounter;
									levelCounter++;
									rects.push_back(Box);
									flag_push = true;
								}
								else if( rects[n].level == 0 && Box.level != 0 )
								{
									rects[n].level = Box.level;
								}
								else if( rects[n].level != 0 && Box.level == 0 )
								{
									if( rects[n].belonged )
									{
										Box.level = rects[n].level;
										rects.push_back(Box);
										flag_push = true;
									}
								}
								else if( rects[n].level != 0 && Box.level != 0 )
								{
									// Reject
									;
								}
								break;
							}
							case 3:
							{
								if( rects[n].level == 0 && Box.level == 0 )
								{
									rects[n].level = levelCounter;
									Box.level = levelCounter;
									levelCounter++;
									rects.push_back(Box);
									flag_push = true;
								}
								else if( rects[n].level != 0 && Box.level == 0 )
								{
									Box.level = rects[n].level;
									rects.push_back(Box);
									flag_push = true;
								}
								else if( rects[n].level == 0 && Box.level != 0 )
								{
									if( Box.belonged )
									{
										rects[n].level = Box.level;
									}
								}
								else if( rects[n].level != 0 && Box.level != 0 )
								{
									// Reject
									;
								}
								break;
							}
						}
					}

					if( !flag_push )
						rects.push_back(Box);
				}
			}
		}
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
		tmpBox.belonged = false;
		tmpBox.p0.x = tmpP0.x;
		tmpBox.p0.y = tmpP0.y;
		tmpBox.p2.x = tmpP2.x;
		tmpBox.p2.y = tmpP2.y;

		myCandi.push_back(tmpBox);

	}

	// Final steps to get candidates
	myCombineLines( myCandi );
	//vector<myt_Cand> Candidates = myGetCandi( myCandi , src );
	for( auto iter = myCandi.begin(); iter!= myCandi.end(); ++iter)
	{
		txtPos.push_back( Rect( iter->p0, iter->p2 ) );
	}

	return 0;
}


bool TextRegionExtracter::mySimiSize( Point a0 , Point a2 , Point b0 , Point b2 )
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
			cout << "		mySimiSize: true" << endl;
			return true;
		}
		else if( a_height <= b_height && (b_height - a_height < a_height/2) || (b_height < a_height && (a_height - b_height < b_height/2)) )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
	}
	else if( a_width > b_width && (a_width - b_width < b_width/2) )
	{
		if( a_width - b_width < b_width/4 )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
		else if( a_height <= b_height && (b_height - a_height < a_height/2) || (b_height < a_height && (a_height - b_height < b_height/2)) )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
	}

	// or start from height
	if( (a_height <= b_height && (b_height - a_height < a_height/2)) )
	{
		if( b_height - a_height < a_height/4 )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
		else if( a_width <= b_width && (b_width - a_width < a_width/2) || (b_width < a_width && (a_width - b_width < b_width/2)) )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
	}
	else if( (a_height > b_height && (b_height - a_height < a_height/2)) )
	{
		if( a_height - b_height < b_height/4 )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
		else if( a_width <= b_width && (b_width - a_width < a_width/2) || (b_width < a_width && (a_width - b_width < b_width/2)) )
		{
			cout << "		mySimiSize: true" << endl;
			return true;
		}
	}


	return false;
}

// Box A much bigger size than B ??
bool TextRegionExtracter::myBigSize( Point a0 , Point a2 , Point b0 , Point b2 )
{
	int a_width = a2.x - a0.x;
	int a_height = a2.y - a0.y;
	int b_width = b2.x - b0.x;
	int b_height = b2.y - b0.y;

	if( a_width > b_width && a_height > b_height )
		if( (a_width - b_width >= b_width) || (a_height - b_height >= b_height) )
		{
			cout << "myBigSize: " << a0.x << "," << a0.y << "-" << a2.x << "," << a2.y << "Bigger" << endl;
			return true;
		}
	return false;
}

bool TextRegionExtracter::myNearRect( Point a0 , Point a2 , Point b0 , Point b2 )
{
	int r1 = cvRound( ( a2.y - a0.y ) / 2 );
	int r2 = cvRound( ( b2.y - b0.y ) / 2 );

	// Adaptive neighbor distance
	int r = (r1 > r2) ? r2 : r1;
	if( r <= D_NREG ) r = D_NREG;

	if( a0.x > b2.x + r || a2.x + r < b0.x || a0.y > b2.y + D_NREG || a2.y + D_NREG < b0.y )
		return false;
	else return true;
}


void TextRegionExtracter::myCombineLines( vector<TextBox> &vi )
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
