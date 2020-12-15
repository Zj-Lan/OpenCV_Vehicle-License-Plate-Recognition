#include "Possible.h"

PossibleChar::PossibleChar(vector<Point> _contour) {
    v_contour = _contour;

    Rect_bounding = boundingRect(v_contour);

    i_CenterX = (Rect_bounding.x + Rect_bounding.x + Rect_bounding.width) / 2;
    i_CenterY = (Rect_bounding.y + Rect_bounding.y + Rect_bounding.height) / 2;

    d_DiagonalSize = sqrt(pow(Rect_bounding.width, 2) + pow(Rect_bounding.height, 2));

    d_AspectRatio = (float)Rect_bounding.width / (float)Rect_bounding.height;
}

