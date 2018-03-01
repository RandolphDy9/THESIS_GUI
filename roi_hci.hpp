#pragma once
#ifndef __ROI_HCI_H__
#define __ROI_HCI_H__
#include <opencv2\imgproc\imgproc.hpp>
#include <QDebug>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>

using namespace cv;

class ROI {
public:
	static void selectPoints(int event, int x, int y, int, void *);
	void set(char * );
	int isSet();
	Mat createMask(Size2i );
	Mat getMask();
	int getPoints();
	
public:
    bool p1,p2,p3,p4;
    Point temporary;
	int points = 0;
	int num_points = 4;
	int num_polygons = 1;
	int line_type = 8;
	int roi_flag = 0;
	Mat mask;
	Point corners[1][4];
	const Point* corner_list[1] = { corners[0] };
};

#endif // !__ROI_HCI_H__
