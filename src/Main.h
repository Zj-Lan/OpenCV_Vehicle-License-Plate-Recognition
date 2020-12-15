#ifndef MAIN_H       
#define MAIN_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "PlatesProc.h"
#include "Possible.h"
#include "CharsProc.h"

#include<iostream>
#include<conio.h>     

using namespace std;
using namespace cv;

const Scalar SCALAR_BLACK = Scalar(0.0, 0.0, 0.0);
const Scalar SCALAR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar SCALAR_YELLOW = Scalar(0.0, 255.0, 255.0);
const Scalar SCALAR_GREEN = Scalar(0.0, 255.0, 0.0);
const Scalar SCALAR_RED = Scalar(0.0, 0.0, 255.0);


int main(void);
void drawRectangle(Mat &img_OriginalScene, PossiblePlate &licPlate);
void writeLicensePlate(Mat &img_OriginalScene, PossiblePlate &licPlate);


# endif	// MAIN

