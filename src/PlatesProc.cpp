#include "PlatesProc.h"

vector<PossibleChar> findPossibleChars(Mat &img_Thresh) {
    vector<PossibleChar> v_PossibleChars;            

    Mat imgContours(img_Thresh.size(), CV_8UC3, SCALAR_BLACK);
    int i_PossibleChars = 0;

    Mat img_ThreshCopy = img_Thresh.clone();

    vector<vector<Point> > v_contours;

    findContours(img_ThreshCopy, v_contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	
    for (unsigned int i = 0; i < v_contours.size(); i++) {                
		drawContours(imgContours, v_contours, i, SCALAR_WHITE);		
        PossibleChar possibleChar(v_contours[i]);

        if (checkIfPossibleChar(possibleChar)) {               
            i_PossibleChars++;                          
            v_PossibleChars.push_back(possibleChar);      
        }
    }

	imshow("2b", imgContours);
    return(v_PossibleChars);
}

vector<PossiblePlate> detectPlates(Mat &imgOriginalScene) {
	vector<PossiblePlate> v_PossiblePlates;
	Mat img_GrayscaleScene;
	Mat img_ThreshScene;	
	RNG rng;
	destroyAllWindows();
	//grayscale and threshold images
	preprocess(imgOriginalScene, img_GrayscaleScene, img_ThreshScene);	
	imshow("1", img_GrayscaleScene);
	imshow("2a", img_ThreshScene);
	// find all possible chars
	vector<PossibleChar> v_PossibleCharsInScene = findPossibleChars(img_ThreshScene);
	vector<vector<PossibleChar> > v_VectorsOfMatchingCharsInScene = findMatchingChars(v_PossibleCharsInScene);

	for (auto &vectorOfMatchingChars : v_VectorsOfMatchingCharsInScene) {
		PossiblePlate possiblePlate = extractPlate(imgOriginalScene, vectorOfMatchingChars);

		if (possiblePlate.imgPlate.empty() == false) {
			v_PossiblePlates.push_back(possiblePlate);
		}
	}


	//for testing
	Mat imgContours(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);
	vector<vector<Point> > contours;
	for (auto &possibleChar : v_PossibleCharsInScene) {
		contours.push_back(possibleChar.v_contour);
	}
	drawContours(imgContours, contours, -1, SCALAR_WHITE);
	imshow("3", imgContours);
	imgContours = Mat(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);
	for (auto &vectorOfMatchingChars : v_VectorsOfMatchingCharsInScene) {		
		vector<vector<Point> > contours;

		for (auto &matchingChar : vectorOfMatchingChars) {
			contours.push_back(matchingChar.v_contour);
		}
		drawContours(imgContours, contours, -1, SCALAR_WHITE);
	}
	imshow("4", imgContours);
	for (unsigned int i = 0; i < v_PossiblePlates.size(); i++) {
		Point2f p2fRectPoints[4];

		v_PossiblePlates[i].rrLocationOfPlateInScene.points(p2fRectPoints);

		for (int j = 0; j < 4; j++) {
			line(imgContours, p2fRectPoints[j], p2fRectPoints[(j + 1) % 4], SCALAR_RED, 2);
		}
		imshow("5", imgContours);		
	}

	
	return v_PossiblePlates;
}

PossiblePlate extractPlate(Mat &imgOriginal, vector<PossibleChar> &vectorOfMatchingChars) {
    PossiblePlate possiblePlate;            

    sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), PossibleChar::sortCharsLeftToRight);
    // calculate the center 
    double d_PlateCenterX = (double)(vectorOfMatchingChars[0].i_CenterX + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].i_CenterX) / 2.0;
    double d_PlateCenterY = (double)(vectorOfMatchingChars[0].i_CenterY + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].i_CenterY) / 2.0;
    Point2d p2dPlateCenter(d_PlateCenterX, d_PlateCenterY);

    // width and height
    int i_PlateWidth = (int)((vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].Rect_bounding.x + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].Rect_bounding.width - vectorOfMatchingChars[0].Rect_bounding.x) * PLATE_WIDTH_PADDING_FACTOR);

    double i_TotalOfCharHeights = 0;

    for (auto &matchingChar : vectorOfMatchingChars) {
        i_TotalOfCharHeights = i_TotalOfCharHeights + matchingChar.Rect_bounding.height;
    }

    double d_Average_Char_Height = (double)i_TotalOfCharHeights / vectorOfMatchingChars.size();

    int i_Plate_Height = (int)(d_Average_Char_Height * PLATE_HEIGHT_PADDING_FACTOR);

    // calculate  angle 
    double d_Opposite = vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].i_CenterY - vectorOfMatchingChars[0].i_CenterY;
    double d_Hypotenuse = CharsDistance(vectorOfMatchingChars[0], vectorOfMatchingChars[vectorOfMatchingChars.size() - 1]);
    double d_CorrectionAngle_InRad = asin(d_Opposite / d_Hypotenuse);
    double d_CorrectionAngle_InDeg = d_CorrectionAngle_InRad * (180.0 / CV_PI);

    possiblePlate.rrLocationOfPlateInScene = RotatedRect(p2dPlateCenter, Size2f((float)i_PlateWidth, (float)i_Plate_Height), (float)d_CorrectionAngle_InDeg);

    Mat rotation_Matrix;             
    Mat img_Rotated;
    Mat img_Cropped;

    rotation_Matrix = getRotationMatrix2D(p2dPlateCenter, d_CorrectionAngle_InDeg, 1.0);         

    warpAffine(imgOriginal, img_Rotated, rotation_Matrix, imgOriginal.size());            
                                                                                            
    getRectSubPix(img_Rotated, possiblePlate.rrLocationOfPlateInScene.size, possiblePlate.rrLocationOfPlateInScene.center, img_Cropped);

    possiblePlate.imgPlate = img_Cropped;            

    return(possiblePlate);
}

