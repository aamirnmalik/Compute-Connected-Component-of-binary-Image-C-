/*
This program is written by Aamir Malik
This program computes connected components for a binary image.
The program takes a binary image as input and then zero frames it.
It then goes trough three passes to compute the connected components
each followed by a pretty print of the image along with the label array.\
The output file will have the image after pass1, then pass 2 and finally
pass 3 which is the connected component image.

To run the program provide an input file as argument 1 and output file
as argument 2.


If there is any question please email me @  malik_2234@hotmail.com

*/
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;
class ImageProcessing {

	int ** imgAry; // declare a 2d dynamic array 
	int ** MirrorframedAry; // declare a 2d dynamic array 
	int ** zeroframedAry; // declare a 2d dynamic array
	
	int ** SobelVertical; // declare a 2d dynamic array
	int ** SobelHorizontal; // declare a 2d dynamic array
	int ** SobelRightDiag; // declare a 2d dynamic array
	int ** SobelLeftDiag; // declare a 2d dynamic array
	int ** SobelVHSum; // declare a 2d dynamic array
	int ** SobelDiagSum; // declare a 2d dynamic array


	int ** tempAry; // declare a 2d dynamic array 
	int * histogram = NULL; // declare a histogram dynamic array
	int * eqArray = NULL;
	int rows, cols, min, max, avgMin, avgMax, medMin, medMax;
	int label = 0;
	int sizeEQ = 0;
	int maskHorizontal[3][3] = {
		{ 1, 2, 1} ,   /*  initializers for row indexed by 0 */
		{ 0,0,0 } ,   /*  initializers for row indexed by 1 */
		{ -1,-2,-1 }   /*  initializers for row indexed by 2 */
	};

	int maskVertical[3][3] = {
		{ 1, 0, -1 } ,   /*  initializers for row indexed by 0 */
		{ 2,0,-2 } ,   /*  initializers for row indexed by 1 */
		{ 1,0,-1 }   /*  initializers for row indexed by 2 */
	};

	int maskRightDiag[3][3] = {
		{ 0, 1, 2 } ,   /*  initializers for row indexed by 0 */
		{ -1,0,1 } ,   /*  initializers for row indexed by 1 */
		{ -2,-1,0 }   /*  initializers for row indexed by 2 */
	};

	int maskLeftDiag[3][3] = {
		{ 2, 1, 0 } ,   /*  initializers for row indexed by 0 */
		{ 1,0,-1 } ,   /*  initializers for row indexed by 1 */
		{ 0,-1,-2 }   /*  initializers for row indexed by 2 */
	};

public:
	ImageProcessing(string file_name) {
		fstream input_file(file_name.c_str());
		input_file >> rows;
		input_file >> cols;
		input_file >> min;
		input_file >> max;


		// declaring the size of imgAry
		imgAry = new int*[rows];
		for (int i = 0; i<rows; i++)
			imgAry[i] = new int[cols];

		// declaring the size of mirrorArray
		MirrorframedAry = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			MirrorframedAry[i] = new int[cols + 2];

		// declaring the size of zeroArray
		zeroframedAry = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			zeroframedAry[i] = new int[cols + 2];

		// declaring the size of tempArray
		tempAry = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			tempAry[i] = new int[cols + 2];

		/**************/

		SobelVertical = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			SobelVertical[i] = new int[cols + 2];

		SobelHorizontal = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			SobelHorizontal[i] = new int[cols + 2];

		SobelRightDiag = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			SobelRightDiag[i] = new int[cols + 2];

		SobelLeftDiag = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			SobelLeftDiag[i] = new int[cols + 2];

		SobelVHSum = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			SobelVHSum[i] = new int[cols + 2];

		SobelDiagSum = new int*[rows + 2];
		for (int i = 0; i<rows + 2; i++)
			SobelDiagSum[i] = new int[cols + 2];



		/**************/


		sizeEQ = (rows*cols) / 4;
		eqArray = new int[sizeEQ];
		for (int i = 0; i < sizeEQ; i++)
		{
			eqArray[i] = i;
		}

		input_file.close();

		loadImage(file_name);

	} // Image constructor

	void loadImage(string file_name) {
		fstream input_file(file_name);

		int temp;
		for (int i = 0; i<4; i++)
			input_file >> temp;

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				input_file >> imgAry[i][j];

	} // loadImage method

	void computeHistogramSum(string filename) {
		// first zero out the histogram array to get rid of the garbage values
		for (int i = 0; i < max + 1; i++) {
			histogram[i] = 0;
		}

		// fill the histogram array 
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				histogram[SobelVHSum[i][j]]++;
			} // for
		} // for

		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);

		ofs << "Printing the Histogram" << endl;
		for (int i = 0; i < max + 1; i++) {
			ofs << "(" << i << ")" << " : " << histogram[i] << " ";
			for (int j = 0; j < histogram[i]; j++) {
				ofs << "*";
				if (j == 79) // dont print more than 80 stars
					j = histogram[i] + 1;
			} // for
			ofs << endl;
		} // for

		ofs << endl << endl;
		ofs.close();

	} // computeHistogram mehtod

	void computeHistogramDiagSum(string filename) {
		// first zero out the histogram array to get rid of the garbage values
		for (int i = 0; i < max + 1; i++) {
			histogram[i] = 0;
		}

		// fill the histogram array 
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				histogram[SobelDiagSum[i][j]]++;
			} // for
		} // for

		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);

		ofs << "Printing the Histogram" << endl;
		for (int i = 0; i < max + 1; i++) {
			ofs << "(" << i << ")" << " : " << histogram[i] << " ";
			for (int j = 0; j < histogram[i]; j++) {
				ofs << "*";
				if (j == 79) // dont print more than 80 stars
					j = histogram[i] + 1;
			} // for
			ofs << endl;
		} // for

		ofs << endl << endl;
		ofs.close();

	} // computeHistogram mehtod

	void computeThresholdSum(int thresh_value, string filename) {
		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);

		ofs << "Printing the ThreshholdIMage " << endl;


		for (int i = 1; i < rows + 1; i++)
			for (int j = 1; j < cols + 1; j++)
				if (SobelVHSum[i][j] < thresh_value)
					SobelVHSum[i][j] = 0;
				else
					SobelVHSum[i][j] = 1;


		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++)
				ofs << SobelVHSum[i][j];
			ofs << endl;
		}
		
		ofs << endl << endl;
		ofs.close();

	}

	void computeThresholdDiag(int thresh_value, string filename) {

		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);

		ofs << "Printing the ThreshholdIMage " << endl;


		for (int i = 1; i < rows + 1; i++)
			for (int j = 1; j < cols + 1; j++)
				if (SobelDiagSum[i][j] < thresh_value)
					SobelDiagSum[i][j] = 0;
				else
					SobelDiagSum[i][j] = 1;


		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++)
				ofs << SobelDiagSum[i][j];
			ofs << endl;
		}

		ofs << endl << endl;
		ofs.close();

	}

	void prettyPrint(string filename, string heading) {
		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);
		ofs << "Printing the Pretty Image " << heading << endl;
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++)
				if (zeroframedAry[i][j] == 0)
					ofs << " ";
				else
					ofs << zeroframedAry[i][j];
			ofs << endl;
		}
		ofs << endl;

		ofs << endl << endl;
		ofs.close();

	} // pretty print method

	void mirrorFramed() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				MirrorframedAry[i + 1][j + 1] = imgAry[i][j];
			} // for
		} // for


		for (int i = 0; i < rows + 2; i++) {
			for (int j = 0; j < cols + 2; j++) {
				if (i == 0)
					MirrorframedAry[i][j] = MirrorframedAry[i + 1][j];
				if (j == 0)
					MirrorframedAry[i][j] = MirrorframedAry[i][j + 1];
				if (i == rows + 1)
					MirrorframedAry[i][j] = MirrorframedAry[i - 1][j];
				if (j == cols + 1)
					MirrorframedAry[i][j] = MirrorframedAry[i][j - 1];
			} // for
		} // for

		MirrorframedAry[0][0] = imgAry[0][0];
	} // mirrorframe method

	void zeroFramed() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				zeroframedAry[i + 1][j + 1] = imgAry[i][j];
			} // for
		} // for


		for (int i = 0; i < rows + 2; i++) {
			for (int j = 0; j < cols + 2; j++) {
				if (i == 0)
					zeroframedAry[i][j] = 0;
				if (j == 0)
					zeroframedAry[i][j] = 0;
				if (i == rows + 1)
					zeroframedAry[i][j] = 0;
				if (j == cols + 1)
					zeroframedAry[i][j] = 0;
			} // for
		} // for5


	} // zero frame method



	void avg3x3() {
		avgMax = min;
		avgMin = max;
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				tempAry[i][j] = ((MirrorframedAry[i - 1][j - 1] + MirrorframedAry[i - 1][j] + MirrorframedAry[i - 1][j + 1]) + (MirrorframedAry[i][j - 1] + MirrorframedAry[i][j] + MirrorframedAry[i][j + 1]) + (MirrorframedAry[i + 1][j - 1] + MirrorframedAry[i + 1][j] + MirrorframedAry[i + 1][j + 1])) / 9;
				if (avgMin > tempAry[i][j])
					avgMin = tempAry[i][j];
				if (avgMax < tempAry[i][j])
					avgMax = tempAry[i][j];

			} // for
		} // for
		ofstream out_file;
		out_file.open("avg3x3.txt");
		out_file << "Printing the Average 3x3 Image Below" << endl;
		out_file << rows << " " << cols << " " << avgMin << " " << avgMax << endl;

		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				out_file << tempAry[i][j] << " ";
			} // for
			out_file << endl;
		} // for
		out_file << endl << endl;
		out_file.close();

	} // 3x3avg method


	  /*
	  This function fills the tmp array with with median 3x3
	  */
	void median3x3() {
		medMax = min;
		medMin = max;

		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				//tempAry[i][j] = ((MirrorframedAry[i - 1][j - 1] + MirrorframedAry[i - 1][j] + MirrorframedAry[i - 1][j + 1]) + (MirrorframedAry[i][j - 1] + MirrorframedAry[i][j] + MirrorframedAry[i][j + 1]) + (MirrorframedAry[i + 1][j - 1] + MirrorframedAry[i + 1][j] + MirrorframedAry[i + 1][j + 1])) / 9;
				int medianSort[] = { MirrorframedAry[i - 1][j - 1] , MirrorframedAry[i - 1][j] , MirrorframedAry[i - 1][j + 1] , MirrorframedAry[i][j - 1] , MirrorframedAry[i][j] , MirrorframedAry[i][j + 1] , MirrorframedAry[i + 1][j - 1] , MirrorframedAry[i + 1][j] , MirrorframedAry[i + 1][j + 1] };
				tempAry[i][j] = (int)CalcMedian(medianSort, 9);
				if (medMin > tempAry[i][j])
					medMin = tempAry[i][j];
				if (medMax < tempAry[i][j])
					medMax = tempAry[i][j];
			} // for
		} // for


		ofstream out_file;
		out_file.open("median3x3.txt");
		out_file << "Printing the Median 3x3 Image Below" << endl;
		out_file << rows << " " << cols << " " << medMin << " " << medMax << endl;

		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				out_file << tempAry[i][j] << " ";
			} // for
			out_file << endl;
		} // for
		out_file << endl << endl;
		out_file.close();

	} // Median method

	  /*
	  This function returns a median given an
	  integer array and its size
	  */
	int CalcMedian(int price[], int a)
	{
		int i, j;

		for (i = 0; i< (a - 1); i++)
		{
			for (j = (i + 1); j < a; j++)
			{
				if (price[i] > price[j])
					swap(price[i], price[j]);
			}
		}


		return price[4];
	}

	void ccPass1() {
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				if (zeroframedAry[i][j]>0) {
					int a = zeroframedAry[i - 1][j - 1];
					int b = zeroframedAry[i - 1][j];
					int c = zeroframedAry[i - 1][j + 1];
					int d = zeroframedAry[i][j - 1];

					if (a == 0 && b == 0 && c == 0 && d == 0) {
						zeroframedAry[i][j] = ++label;

					}

					else if (a == b && b == c && c == d) {
						zeroframedAry[i][j] = a;
					}
					else {

						int sort[] = { a,b,c,d };

						// sort

						for (int i = 0; i < (4 - 1); i++)
						{
							for (int j = (i + 1); j < 4; j++)
							{
								if (sort[i] > sort[j])
									swap(sort[i], sort[j]);
							}
						}
						// finished sorting

						// getting the min non zero
						int x = 0;
						while (sort[x] == 0) {
							x++;
						}

						// put it in zero framed
						zeroframedAry[i][j] = sort[x];

						eqArray[a] = zeroframedAry[i][j];
						eqArray[b] = zeroframedAry[i][j];
						eqArray[c] = zeroframedAry[i][j];
						eqArray[d] = zeroframedAry[i][j];

					} // else

				} // if not equal to 0

			} // for 
		} // for 


	} // method

	void ccPass2() {
		for (int i = rows; i > 0; i--) {
			for (int j = cols; j > 0; j--) {
				if (zeroframedAry[i][j] > 0) {
					int e = zeroframedAry[i][j + 1];
					int f = zeroframedAry[i + 1][j - 1];
					int g = zeroframedAry[i + 1][j];
					int h = zeroframedAry[i + 1][j + 1];

					int min = (std::min({ e,f,g,h }));
					int max = (std::max({ e,f,g,h }));

					if (min != max) {

						int sort[] = { e,f,g,h, zeroframedAry[i][j] };

						for (int i = 0; i < (5 - 1); i++)
						{
							for (int j = (i + 1); j < 5; j++)
							{
								if (sort[i] > sort[j])
									swap(sort[i], sort[j]);
							} // for
						} // for

						int x = 0;
						while (sort[x] == 0) {
							x++;
						}
						zeroframedAry[i][j] = sort[x];


						eqArray[e] = zeroframedAry[i][j];
						eqArray[f] = zeroframedAry[i][j];
						eqArray[g] = zeroframedAry[i][j];
						eqArray[h] = zeroframedAry[i][j];

					}


				}
			} // for 
		} // for 


	} // method

	void manageEq() {

		vector<int> myvector;
		for (int i = 1; i < label + 1; i++)
		{
			if (eqArray[i] != i) {


				int j = eqArray[i];
				while (eqArray[j] != j) {
					j = eqArray[j];
				}
				eqArray[i] = eqArray[j];

			} // if
		} // for

		for (int i = 1; i < label + 1; i++)
		{
			if (find(myvector.begin(), myvector.end(), eqArray[i]) != myvector.end()) {

			}
			else {
				myvector.push_back(eqArray[i]);
			}

		}


		for (int i = 1; i < label + 1; i++)
		{
			for (int j = 0; j < myvector.size(); j++) {
				if (myvector[j] == eqArray[i])
					eqArray[i] = j + 1;
			}
		}

	} // manageEq

	void ccPass3() {
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				if (zeroframedAry[i][j]>0) {
					zeroframedAry[i][j] = eqArray[zeroframedAry[i][j]];
				}
			}
		}

	} // fucntion pass 3


	void distPass1() {

		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				if (zeroframedAry[i][j]>0) {

					int a = zeroframedAry[i - 1][j - 1];
					int b = zeroframedAry[i - 1][j];
					int c = zeroframedAry[i - 1][j + 1];
					int d = zeroframedAry[i][j - 1];

					int min = (std::min({ a,b,c,d }));

					zeroframedAry[i][j] = min + 1;

				}
			}
		}

	} //method 


	void distPass2() {

		for (int i = rows; i > 0; i--) {
			for (int j = cols; j > 0; j--) {
				if (zeroframedAry[i][j] > 0) {

					int e = zeroframedAry[i][j + 1];
					int f = zeroframedAry[i + 1][j - 1];
					int g = zeroframedAry[i + 1][j];
					int h = zeroframedAry[i + 1][j + 1];

					int min = (std::min({ e,f,g,h }));
					min++;

					zeroframedAry[i][j] = (std::min(min, zeroframedAry[i][j]));

				}
			}
		}

	} //method

	void computeSkeleton() {

		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				int a = zeroframedAry[i - 1][j - 1];
				int b = zeroframedAry[i - 1][j];
				int c = zeroframedAry[i - 1][j + 1];
				int d = zeroframedAry[i][j - 1];
				int e = zeroframedAry[i][j + 1];
				int f = zeroframedAry[i + 1][j - 1];
				int g = zeroframedAry[i + 1][j];
				int h = zeroframedAry[i + 1][j + 1];

				int max = (std::max({ a,b,c,d,e,f,g,h }));

				if (zeroframedAry[i][j] >= max) {
					tempAry[i][j] = zeroframedAry[i][j];
				}
				else {
					tempAry[i][j] = 0;
				}

			}
		}

	} // method


	void prettyPrintSkeleton(string filename, string heading) {

		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);
		ofs << "Printing the Pretty Image " << heading << endl;
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++)
				if (tempAry[i][j] == 0)
					ofs << " ";
				else
					ofs << tempAry[i][j];
			//ofs << static_cast<char>(tempAry[i][j]);  not sure if this is how you want the output so i commented it
			ofs << endl;
		}
		ofs << endl;

		ofs << endl << endl;
		ofs.close();

	}

	int convolute(int i, int j, int ary[3][3]) {
		int sum = 0;

		for (int k = 0; k < 3; k++)
		{
			for (int m = 0; m < 3; m++)
			{
				sum +=  ary[k][m] * MirrorframedAry[i+k-1][j+m-1];
			}
		}
		return sum;
	} // convolute

	void computeConvolute(string sobel) {
		
		if (sobel == "vertical") {
			for (int i = 1; i < rows + 1; i++)
			{
				for (int j = 0; j < cols + 1; j++)
				{
					SobelVertical[i][j] = convolute(i, j, maskVertical);
				}
			}
		}

		if (sobel == "horizontal") {
			for (int i = 1; i < rows + 1; i++)
			{
				for (int j = 0; j < cols + 1; j++)
				{
					SobelHorizontal[i][j] = convolute(i, j, maskHorizontal);
				}
			}
		} //if
		
		if (sobel == "rightDiag") {
			for (int i = 1; i < rows + 1; i++)
			{
				for (int j = 0; j < cols + 1; j++)
				{
					SobelRightDiag[i][j] = convolute(i, j, maskRightDiag);
				}
			}
		} //if

		if (sobel == "leftDiag") {
			for (int i = 1; i < rows + 1; i++)
			{
				for (int j = 0; j < cols + 1; j++)
				{
					SobelLeftDiag[i][j] = convolute(i, j, maskLeftDiag);
				}
			}
		} // if
	
	}

	void sobelVHSum() {
		int maxer = 0;
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				SobelVHSum[i][j] = abs(SobelHorizontal[i][j]) + abs(SobelVertical[i][j]);
				if (SobelVHSum[i][j]>maxer)
					maxer = SobelVHSum[i][j];
			}

		}
		histogram = new int[maxer + 1];
		max = maxer;

	}

	void sobelDiagSum() {
		int maxer = 0;
		for (int i = 1; i < rows + 1; i++) {
			for (int j = 1; j < cols + 1; j++) {
				SobelDiagSum[i][j] = abs(SobelLeftDiag[i][j]) + abs(SobelRightDiag[i][j]);
				if (SobelDiagSum[i][j]>maxer)
					maxer = SobelDiagSum[i][j];
			}
		}
		histogram = new int[maxer + 1];
		max = maxer;

		cout << maxer;
	}

	void prettyPrintSobel(string filename, string sobel) {
		ofstream ofs;
		ofs.open(filename, ofstream::out | ofstream::app);
		if (sobel=="vertical")
		{
			ofs << "Printing the Pretty Image for sobel" << sobel  << endl;
			for (int i = 1; i < rows + 1; i++) {
				for (int j = 1; j < cols + 1; j++)
					ofs << SobelVertical[i][j] << " ";
				ofs << endl;
			}
			ofs << endl;

		} //if

		if (sobel == "horizontal")
		{
			ofs << "Printing the Pretty Image for sobel" << sobel << endl;
			for (int i = 1; i < rows + 1; i++) {
				for (int j = 1; j < cols + 1; j++)
					ofs << SobelHorizontal[i][j] << " ";
				ofs << endl;
			}
			ofs << endl;

		} //if

		if (sobel == "rightDiag")
		{
			ofs << "Printing the Pretty Image for sobel" << sobel << endl;
			for (int i = 1; i < rows + 1; i++) {
				for (int j = 1; j < cols + 1; j++)
					ofs << SobelRightDiag[i][j] << " ";
				ofs << endl;
			}
			ofs << endl;

		} //if

		if (sobel == "leftDiag")
		{
			ofs << "Printing the Pretty Image for sobel" << sobel << endl;
			for (int i = 1; i < rows + 1; i++) {
				for (int j = 1; j < cols + 1; j++)
					ofs << SobelLeftDiag[i][j] << " ";
				ofs << endl;
			}
			ofs << endl;

		} // if

		if (sobel == "vhSum")
		{
			ofs << "Printing the Pretty Image for sobel" << sobel << endl;
			for (int i = 1; i < rows + 1; i++) {
				for (int j = 1; j < cols + 1; j++)
					ofs << SobelVHSum[i][j] << " ";
				ofs << endl;
			}
			ofs << endl;

		} // if

		if (sobel == "diagSum")
		{
			ofs << "Printing the Pretty Image for sobel" << sobel << endl;
			for (int i = 1; i < rows + 1; i++) {
				for (int j = 1; j < cols + 1; j++)
					ofs << SobelDiagSum[i][j] << " ";
				ofs << endl;
			}
			ofs << endl;

		} // if


		ofs << endl << endl;
		ofs.close();

	} // pretty print method



}; // Image Class

int main(int argc, const char * argv[]) {
	ImageProcessing grey(argv[1]);
	
	grey.mirrorFramed();

	grey.computeConvolute("vertical");

	grey.computeConvolute("horizontal");

	grey.computeConvolute("leftDiag");

	grey.computeConvolute("rightDiag");

	grey.sobelVHSum();

	grey.prettyPrintSobel(argv[2], "vhSum");

	grey.computeHistogramSum(argv[2]);

	grey.computeThresholdSum(15,argv[2]);

	grey.sobelDiagSum();

	grey.prettyPrintSobel(argv[3], "diagSum");
	
	grey.computeHistogramDiagSum(argv[3]);

	grey.computeThresholdDiag(22, argv[3]);


	return 0;
} // main