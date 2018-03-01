#include "roi_hci.hpp"

using namespace std;
int ROI::getPoints()
{
	return points;
}
void ROI::set(char *windowName)
{
    qDebug() <<"flag=" << roi_flag << endl;
	cvSetMouseCallback(windowName, selectPoints, this);
		
}
Mat ROI::createMask(Size2i size)
{
	mask = Mat::zeros(size, CV_8UC3);
	fillPoly(mask, corner_list, &num_points, num_polygons, cv::Scalar(255, 255, 255), line_type);
	return mask;
}
void ROI::selectPoints(int event, int x, int y, int, void *this_)
{
	static ROI *dat = (ROI *)this_;
	
	if (event == CV_EVENT_LBUTTONDOWN && !dat->roi_flag)
	{
		
        qDebug() <<"points: "<<dat->points << endl;
		switch (dat->points) {
		case 0:

			dat->corners[0][0] = Point(x, y);
            dat->p1 = true; dat->p2 = false;dat->p3 = false;dat->p4 = false;
            qDebug() << dat->corners[0][0].x << endl;
			break;
		case 1:

			dat->corners[0][1] = Point(x, y);
            dat->p1 = true; dat->p2 = true;dat->p3 = false;dat->p4 = false;
            qDebug() << dat->corners[0][1].x << endl;
			break;
		case 2:

			dat->corners[0][2] = Point(x, y);
            dat->p1 = true; dat->p2 = true;dat->p3 = true;dat->p4 = false;
            qDebug() << dat->corners[0][2].x << endl;
			break;
		case 3:

			dat->corners[0][3] = Point(x, y);
            dat->p1 = true; dat->p2 = true;dat->p3 = true;dat->p4 = true;
            qDebug() << dat->corners[0][3].x << endl;
			
			break;
	
		}
		if (dat->points < 4) {
			dat->points++;
		}
	

	}
    if(event == CV_EVENT_MOUSEMOVE){
        dat->temporary = Point(x,y);
    }
	if (event == CV_EVENT_RBUTTONDOWN)
	{
		dat->roi_flag = 0;
		if (dat->points != 0)
			dat->points--;
	}
	if (dat->points >= 4) {
		dat->roi_flag = 1;
	}
	
}
int ROI::isSet() 
{
	return roi_flag;
}
Mat ROI::getMask() 
{
	return mask;
}
