#include "CharsProc.h"

Ptr<ml::KNearest> kNearest = ml::KNearest::create();

vector<PossibleChar> findPossibleChars(Mat &img_Grayscale, Mat &img_Thresh) {
    vector<PossibleChar> v_Possible_Chars;                            

    Mat img_ThreshCopy;

    vector<vector<Point> > v_contours;

    img_ThreshCopy = img_Thresh.clone();

    findContours(img_ThreshCopy, v_contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); 

    for (auto &contour : v_contours) {                           
        PossibleChar possibleChar(contour);

        if (checkIfPossibleChar(possibleChar)) {                
            v_Possible_Chars.push_back(possibleChar);      
        }
    }

    return(v_Possible_Chars);
}

bool checkIfPossibleChar(PossibleChar &possibleChar) {
    if (possibleChar.Rect_bounding.area() > MIN_PIXEL_AREA &&
        possibleChar.Rect_bounding.width > MIN_PIXEL_WIDTH && possibleChar.Rect_bounding.height > MIN_PIXEL_HEIGHT &&
        MIN_ASPECT_RATIO < possibleChar.d_AspectRatio && possibleChar.d_AspectRatio < MAX_ASPECT_RATIO) {
        return(true);
    }
    else {
        return(false);
    }
}

vector<vector<PossibleChar> > findMatchingChars(const vector<PossibleChar> &vectorOfPossibleChars) {
    // find all the possible chars  
    vector<vector<PossibleChar> > v_VectorsOfMatchingChars;             

    for (auto &possibleChar : vectorOfPossibleChars) {                  

                                                                        
        vector<PossibleChar> v_MatchingChars = findMatchingChars(possibleChar, vectorOfPossibleChars);

        v_MatchingChars.push_back(possibleChar);          

                                                                
        if (v_MatchingChars.size() < MIN_NUMBER_OF_MATCHING_CHARS) {
            continue;                       
        }
       
        v_VectorsOfMatchingChars.push_back(v_MatchingChars);            

        vector<PossibleChar> v_PossibleChars_WithCurrentMatches_Removed;

        for (auto &possChar : vectorOfPossibleChars) {
            if (find(v_MatchingChars.begin(), v_MatchingChars.end(), possChar) == v_MatchingChars.end()) {
                v_PossibleChars_WithCurrentMatches_Removed.push_back(possChar);
            }
        }
        vector<vector<PossibleChar> > v_recursiveVector_VectorsOfMatchingChars;

        v_recursiveVector_VectorsOfMatchingChars = findMatchingChars(v_PossibleChars_WithCurrentMatches_Removed);	

        for (auto &recursiveVectorOfMatchingChars : v_recursiveVector_VectorsOfMatchingChars) {      
            v_VectorsOfMatchingChars.push_back(recursiveVectorOfMatchingChars);               
        }

        break;		
    }

    return(v_VectorsOfMatchingChars);
}

vector<PossibleChar> findMatchingChars(const PossibleChar &possibleChar, const vector<PossibleChar> &vectorOfChars) {

    vector<PossibleChar> v_MatchingChars;                

    for (auto &possibleMatchingChar : vectorOfChars) {              

                                                                    
        if (possibleMatchingChar == possibleChar) {
            continue;           
        }
        // check if chars match
        double d_Distance_Chars = CharsDistance(possibleChar, possibleMatchingChar);
        double d_Angle_Chars = CharsAngle(possibleChar, possibleMatchingChar);
        double d_Change_Area = (double)abs(possibleMatchingChar.Rect_bounding.area() - possibleChar.Rect_bounding.area()) / (double)possibleChar.Rect_bounding.area();
        double d_Change_Width = (double)abs(possibleMatchingChar.Rect_bounding.width - possibleChar.Rect_bounding.width) / (double)possibleChar.Rect_bounding.width;
        double d_Change_Height = (double)abs(possibleMatchingChar.Rect_bounding.height - possibleChar.Rect_bounding.height) / (double)possibleChar.Rect_bounding.height;

        // check if chars match
        if (d_Distance_Chars < (possibleChar.d_DiagonalSize * MAX_DIAG_SIZE_MULTIPLE_AWAY) &&
            d_Angle_Chars < MAX_ANGLE_BETWEEN_CHARS &&
            d_Change_Area < MAX_CHANGE_IN_AREA &&
            d_Change_Width < MAX_CHANGE_IN_WIDTH &&
            d_Change_Height < MAX_CHANGE_IN_HEIGHT) {
            v_MatchingChars.push_back(possibleMatchingChar);    
        }
    }

    return(v_MatchingChars);          
}

bool loadKNN(void) {

	//load training data

	Mat mat_ClassificationInts;//to store data

	FileStorage fsClassifications("classifications.xml", FileStorage::READ);

	if (fsClassifications.isOpened() == false) {// if the file was not opened
		cout << "error, loading training file\n\n";
		return(false);
	}

	fsClassifications["classifications"] >> mat_ClassificationInts;
	fsClassifications.release();

	Mat mat_TrainingImages;

	FileStorage fsTrainingImages("images.xml", FileStorage::READ);// open the training images file

	if (fsTrainingImages.isOpened() == false) {
		cout << "error, unable to open training images file, exiting program\n\n";
		return(false);
	}

	fsTrainingImages["images"] >> mat_TrainingImages;
	fsTrainingImages.release();
	kNearest->setDefaultK(1);
	kNearest->train(mat_TrainingImages, ml::ROW_SAMPLE, mat_ClassificationInts);

	return true;
}

vector<PossiblePlate> detectChars(vector<PossiblePlate> &vectorOfPossiblePlates) {

	RNG rng;

	if (vectorOfPossiblePlates.empty()) {//if no possible item
		return(vectorOfPossiblePlates);
	}

	for (auto &possiblePlate : vectorOfPossiblePlates) {

		//grayscale and threshold images
		preprocess(possiblePlate.imgPlate, possiblePlate.imgGrayscale, possiblePlate.imgThresh);
		//upscale size
		resize(possiblePlate.imgThresh, possiblePlate.imgThresh, Size(), 1.6, 1.6);
		//threshold
		threshold(possiblePlate.imgThresh, possiblePlate.imgThresh, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
		// find all possible chars
		vector<PossibleChar> v_PossibleChars_InPlate = findPossibleChars(possiblePlate.imgGrayscale, possiblePlate.imgThresh);
		vector<vector<PossibleChar> > v_Vectors_MatchingChars_InPlate = findMatchingChars(v_PossibleChars_InPlate);

		if (v_Vectors_MatchingChars_InPlate.size() == 0) {
			possiblePlate.strChars = "";
			continue;
		}
		//handling chars
		for (auto &vectorOfMatchingChars : v_Vectors_MatchingChars_InPlate) {
			sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), PossibleChar::sortCharsLeftToRight);
			vectorOfMatchingChars = removelappingChars(vectorOfMatchingChars);
		}

		//set the longest as the result
		unsigned int i_longgest = 0;
		unsigned int i_longgest_index = 0;

		for (unsigned int i = 0; i < v_Vectors_MatchingChars_InPlate.size(); i++) {
			if (v_Vectors_MatchingChars_InPlate[i].size() > i_longgest) {
				i_longgest = v_Vectors_MatchingChars_InPlate[i].size();
				i_longgest_index = i;
			}
		}

		vector<PossibleChar> V_longgest = v_Vectors_MatchingChars_InPlate[i_longgest_index];

		imshow("6a", possiblePlate.imgPlate);
		imshow("6b", possiblePlate.imgGrayscale);
		imshow("6c", possiblePlate.imgThresh);
		//perform char recognition
		possiblePlate.strChars = recognizeChars(possiblePlate.imgThresh, V_longgest);
		

	}
	

	return(vectorOfPossiblePlates);
}

double CharsDistance(const PossibleChar &firstChar, const PossibleChar &secondChar) {
	//calculate distance between two chars
	int intX = abs(firstChar.i_CenterX - secondChar.i_CenterX);
    int intY = abs(firstChar.i_CenterY - secondChar.i_CenterY);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

double CharsAngle(const PossibleChar &firstChar, const PossibleChar &secondChar) {
	// calculate angle between chars
	double d_Adj = abs(firstChar.i_CenterX - secondChar.i_CenterX);
    double d_Opp = abs(firstChar.i_CenterY - secondChar.i_CenterY);

    double d_AngleInRad = atan(d_Opp / d_Adj);

    double d_AngleInDeg = d_AngleInRad * (180.0 / CV_PI);

    return(d_AngleInDeg);
}

vector<PossibleChar> removelappingChars(vector<PossibleChar> &vectorOfMatchingChars) {
    vector<PossibleChar> v_MatchingChars_WithInnerChar_Removed(vectorOfMatchingChars);

    for (auto &currentChar : vectorOfMatchingChars) {
        for (auto &otherChar : vectorOfMatchingChars) {
            if (currentChar != otherChar) { 
                if (CharsDistance(currentChar, otherChar) < (currentChar.d_DiagonalSize * MIN_DIAG_SIZE_MULTIPLE_AWAY)) {                  

                    // if current char is smaller
                    if (currentChar.Rect_bounding.area() < otherChar.Rect_bounding.area()) {
                        // look for char in vector
                        vector<PossibleChar>::iterator v_currentCharIterator = find(v_MatchingChars_WithInnerChar_Removed.begin(), v_MatchingChars_WithInnerChar_Removed.end(), currentChar);
                        
                        if (v_currentCharIterator != v_MatchingChars_WithInnerChar_Removed.end()) {
                            v_MatchingChars_WithInnerChar_Removed.erase(v_currentCharIterator);       
                        }
                    }
                    else {        
                        vector<PossibleChar>::iterator v_otherCharIterator = find(v_MatchingChars_WithInnerChar_Removed.begin(), v_MatchingChars_WithInnerChar_Removed.end(), otherChar);
                        if (v_otherCharIterator != v_MatchingChars_WithInnerChar_Removed.end()) {
                            v_MatchingChars_WithInnerChar_Removed.erase(v_otherCharIterator);         
                        }
                    }
                }
            }
        }
    }

    return(v_MatchingChars_WithInnerChar_Removed);
}

string recognizeChars(Mat &imgThresh, vector<PossibleChar> &vectorOfMatchingChars) {
    string str_Chars;               

    Mat img_ThreshColor;

    // sort chars from left to right
    sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), PossibleChar::sortCharsLeftToRight);

    cvtColor(imgThresh, img_ThreshColor, CV_GRAY2BGR);       

    for (auto &currentChar : vectorOfMatchingChars) {           
        rectangle(img_ThreshColor, currentChar.Rect_bounding, SCALAR_GREEN, 2);       

        Mat img_ROItoBeCloned = imgThresh(currentChar.Rect_bounding);                 

        Mat img_ROI = img_ROItoBeCloned.clone();      

        Mat img_ROIResized;
        // resize image for char recognition
        resize(img_ROI, img_ROIResized, Size(RESIZED_CHAR_IMAGE_WIDTH, RESIZED_CHAR_IMAGE_HEIGHT));

        Mat mat_ROIFloat;

        img_ROIResized.convertTo(mat_ROIFloat, CV_32FC1);         

        Mat mat_ROI_FlattenedFloat = mat_ROIFloat.reshape(1, 1);       

        Mat matCurrentChar(0, 0, CV_32F);                   

        kNearest->findNearest(mat_ROI_FlattenedFloat, 1, matCurrentChar);     

        float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);       

        str_Chars = str_Chars + char(int(fltCurrentChar));
		
		imshow(str_Chars, img_ThreshColor);
    }

	

    return(str_Chars);              
}

