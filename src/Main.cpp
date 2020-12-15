#include "Main.h"

void drawRectangle(Mat &img_OriginalScene, PossiblePlate &licPlate) {
	Point2f p2f_Rect[4];

	licPlate.rrLocationOfPlateInScene.points(p2f_Rect);

	for (int i = 0; i < 4; i++) {
		line(img_OriginalScene, p2f_Rect[i], p2f_Rect[(i + 1) % 4], SCALAR_GREEN, 2);
	}
}


void writeLicensePlate(Mat &imgOriginalScene, PossiblePlate &licPlate) {
	Point pt_TextCenter;
	Point pt_TextOrigin;

	int int_FontFace = CV_FONT_HERSHEY_SIMPLEX;
	double d_FontScale = (double)licPlate.imgPlate.rows / 30.0;
	int i_Font_Thickness = (int)round(d_FontScale * 1.5);
	int i_Base_line = 0;

	Size textSize = getTextSize(licPlate.strChars, int_FontFace, d_FontScale, i_Font_Thickness, &i_Base_line);

	pt_TextCenter.x = (int)licPlate.rrLocationOfPlateInScene.center.x;

	if (licPlate.rrLocationOfPlateInScene.center.y < (imgOriginalScene.rows * 0.75)) {
		pt_TextCenter.y = (int)round(licPlate.rrLocationOfPlateInScene.center.y) + (int)round((double)licPlate.imgPlate.rows * 1.6);
	}
	else {
		pt_TextCenter.y = (int)round(licPlate.rrLocationOfPlateInScene.center.y) - (int)round((double)licPlate.imgPlate.rows * 1.6);
	}

	pt_TextOrigin.x = (int)(pt_TextCenter.x - (textSize.width / 2));
	pt_TextOrigin.y = (int)(pt_TextCenter.y + (textSize.height / 2));


	putText(imgOriginalScene, licPlate.strChars, pt_TextOrigin, int_FontFace, d_FontScale, SCALAR_WHITE, i_Font_Thickness);
}

int main(void) {

	//load training data
    bool b_KNN = loadKNN();           

    if (b_KNN == false) {
        cout << endl << endl << "error: load KNN traning fail" << endl << endl;
        return(0);                                                      
    }

    Mat img_Original;           

    img_Original = imread("2.jpg");         // open image

    if (img_Original.empty()) {                             
        cout << "error: load image fail\n\n";     
        _getch();                                               
        return(0);                                              
    }
	// detect plates
    vector<PossiblePlate> v_Possible_Plates = detectPlates(img_Original);          
	// detect chars
    v_Possible_Plates = detectChars(v_Possible_Plates);                               

    imshow("imgOriginal", img_Original);           
	// if no plates
    if (v_Possible_Plates.empty()) {                                               
        cout << endl << "no license plates were detected" << endl;       
    }
    else {                                                                            
        sort(v_Possible_Plates.begin(), v_Possible_Plates.end(), PossiblePlate::sortDescendingByNumberOfChars);

        
        PossiblePlate licPlate = v_Possible_Plates.front();

        imshow("img_Plate", licPlate.imgPlate);            
        imshow("img_Thresholding", licPlate.imgThresh);

        if (licPlate.strChars.length() == 0) {                                                      
            cout << endl << "no characters were detected" << endl << endl;      
            return(0);                                                                              
        }
		// draw rectangle
        drawRectangle(img_Original, licPlate);                
		// write license plate text
        cout << endl << "license plate = " << licPlate.strChars << endl;  		
        writeLicensePlate(img_Original, licPlate);              

        imshow("img_result", img_Original);                       

        imwrite("img_result.png", img_Original);                  
    }

    waitKey(0);                 

    return(0);
}


