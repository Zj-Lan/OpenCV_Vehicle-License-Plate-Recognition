#include "PreProc.h"

void preprocess(Mat &img_Original, Mat &img_Grayscale, Mat &img_Thresh) {
    img_Grayscale = extractValue(img_Original);                           

    Mat img_Max_Contrast_Grayscale = maximizeContrast(img_Grayscale);       

    Mat img_Blurred;

	GaussianBlur(img_Max_Contrast_Grayscale, img_Blurred, GAUSSIAN_SMOOTH_FILTER_SIZE, 0);
                                                                                                    
	adaptiveThreshold(img_Blurred, img_Thresh, 255.0, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, ADAPTIVE_THRESH_BLOCK_SIZE, ADAPTIVE_THRESH_WEIGHT);
}


Mat extractValue(Mat &imgOriginal) {
    Mat img_HSV;
    vector<Mat> v_HSV_Images;
    Mat img_Value;

    cvtColor(imgOriginal, img_HSV, CV_BGR2HSV);

    split(img_HSV, v_HSV_Images);

    img_Value = v_HSV_Images[2];

    return(img_Value);
}

Mat maximizeContrast(Mat &imgGrayscale) {
    Mat img_TopHat;
    Mat img_BlackHat;
    Mat img_Grayscale_PlusTopHat;
    Mat img_Grayscale_PlusTopHat_MinusBlackHat;

    Mat img_structuringElement = getStructuringElement(CV_SHAPE_RECT, Size(3, 3));

    morphologyEx(imgGrayscale, img_TopHat, CV_MOP_TOPHAT, img_structuringElement);
    morphologyEx(imgGrayscale, img_BlackHat, CV_MOP_BLACKHAT, img_structuringElement);

    img_Grayscale_PlusTopHat = imgGrayscale + img_TopHat;
    img_Grayscale_PlusTopHat_MinusBlackHat = img_Grayscale_PlusTopHat - img_BlackHat;

    return(img_Grayscale_PlusTopHat_MinusBlackHat);
}


