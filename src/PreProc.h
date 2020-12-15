#ifndef PREPROCESS_H
#define PREPROCESS_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
const Size GAUSSIAN_SMOOTH_FILTER_SIZE = Size(5, 5);
const int ADAPTIVE_THRESH_BLOCK_SIZE = 19;
const int ADAPTIVE_THRESH_WEIGHT = 9;

void preprocess(Mat &img_Original, Mat &img_Grayscale, Mat &img_Thresh);

Mat extractValue(Mat &img_Original);

Mat maximizeContrast(Mat &img_Grayscale);


#endif	// PREPROCESS_H

