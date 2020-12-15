#ifndef PLATES_Proc_H
#define PLATES_Proc_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
#include "Main.h"
#include "Possible.h"
#include "PreProc.h"
#include "CharsProc.h"

const double PLATE_WIDTH_PADDING_FACTOR = 1.3;
const double PLATE_HEIGHT_PADDING_FACTOR = 1.5;

vector<PossiblePlate> detectPlates(Mat &img_OriginalScene);

vector<PossibleChar> findPossibleChars(Mat &img_Thresh);

PossiblePlate extractPlate(Mat &img_Original, vector<PossibleChar> &v_MatchingChars);


# endif	// DETECT_PLATES_H

