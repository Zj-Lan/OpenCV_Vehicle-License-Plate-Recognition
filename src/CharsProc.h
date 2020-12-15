#ifndef CHARS_Proc_H
#define CHARS_Proc_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include "Main.h"
#include "Possible.h"
#include "PreProc.h"
using namespace std;
using namespace cv;

bool loadKNN(void);

vector<PossiblePlate> detectChars(vector<PossiblePlate> &v_PossiblePlates);

vector<PossibleChar> findPossibleChars(Mat &img_Grayscale, Mat &img_Thresh);

bool checkIfPossibleChar(PossibleChar &possibleChar);

vector<vector<PossibleChar> > findMatchingChars(const vector<PossibleChar> &v_PossibleChars);

vector<PossibleChar> findMatchingChars(const PossibleChar &possibleChar, const vector<PossibleChar> &v_Chars);

double CharsDistance(const PossibleChar &firstChar, const PossibleChar &secondChar);

double CharsAngle(const PossibleChar &firstChar, const PossibleChar &secondChar);

vector<PossibleChar> removelappingChars(vector<PossibleChar> &v_MatchingChars);

string recognizeChars(Mat &img_Thresh, vector<PossibleChar> &v_MatchingChars);


// global constants 
const int MIN_PIXEL_WIDTH = 2;
const int MIN_PIXEL_HEIGHT = 8;

const double MIN_ASPECT_RATIO = 0.25;
const double MAX_ASPECT_RATIO = 1.0;

const int MIN_PIXEL_AREA = 80;

// constants for comparing two chars
const double MIN_DIAG_SIZE_MULTIPLE_AWAY = 0.3;
const double MAX_DIAG_SIZE_MULTIPLE_AWAY = 5.0;

const double MAX_CHANGE_IN_AREA = 0.5;

const double MAX_CHANGE_IN_WIDTH = 0.8;
const double MAX_CHANGE_IN_HEIGHT = 0.2;

const double MAX_ANGLE_BETWEEN_CHARS = 12.0;

// others
const int MIN_NUMBER_OF_MATCHING_CHARS = 3;

const int RESIZED_CHAR_IMAGE_WIDTH = 20;
const int RESIZED_CHAR_IMAGE_HEIGHT = 30;

const int MIN_CONTOUR_AREA = 100;

// external global variables 
extern const bool blnShowSteps;
extern Ptr<ml::KNearest>  kNearest;



#endif	// DETECT_CHARS_H

