#ifndef POSSIBLE_H
#define POSSIBLE_H

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

class PossibleChar {
public:
    vector<Point> v_contour;

    Rect Rect_bounding;

    int i_CenterX;
    int i_CenterY;

    double d_DiagonalSize;
    double d_AspectRatio;

    static bool sortCharsLeftToRight(const PossibleChar &pcLeft, const PossibleChar & pcRight) {
        return(pcLeft.i_CenterX < pcRight.i_CenterX);
    }

    bool operator == (const PossibleChar& otherPossibleChar) const {
        if (this->v_contour == otherPossibleChar.v_contour) return true;
        else return false;
    }

    bool operator != (const PossibleChar& otherPossibleChar) const {
        if (this->v_contour != otherPossibleChar.v_contour) return true;
        else return false;
    }

    PossibleChar(vector<Point> _contour);

};

class PossiblePlate {
public:
	Mat imgPlate;
	Mat imgGrayscale;
	Mat imgThresh;

	RotatedRect rrLocationOfPlateInScene;

	string strChars;

	static bool sortDescendingByNumberOfChars(const PossiblePlate &ppLeft, const PossiblePlate &ppRight) {
		return(ppLeft.strChars.length() > ppRight.strChars.length());
	}

};

#endif  // POSSIBLE_H

