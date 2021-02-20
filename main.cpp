//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

vector<Point2f> pts_src,pts_dst;


void callbackfnc(int event,int x,int y,int flags,void *userdata)
{
    if(event == EVENT_LBUTTONDOWN){
        // clicked points
        pts_src.push_back(Point2f(x,y));
    }

}

int main(int argc, char** argv)
{
    Mat image_src = imread("/Users/abhinavjain/Desktop/cop290/COP290-Assignment-1-/empty.jpg",IMREAD_GRAYSCALE);
    Mat image_proj,image_crop;

    if(image_src.empty()){
        cout<<"image_src not found"<<endl;
        cin.get();  return -1;
    }

    string window_src = "original frame";
    string window_proj= "projected frame";
    string window_crop = "cropped frame";

    namedWindow(window_src); 

    setMouseCallback(window_src,callbackfnc,NULL);
    imshow(window_src,image_src);
    waitKey(0);
    destroyWindow(window_src);
   
   // destination points where the corners of main road are mapped to.
    // better points can be chosen
    pts_dst.push_back(Point2f(831,211));
    pts_dst.push_back(Point2f(831,1078));
    pts_dst.push_back(Point2f(1375,1078));
    pts_dst.push_back(Point2f(1375,211));
    
    
    Mat h =findHomography(pts_src,pts_dst);
    warpPerspective(image_src,image_proj, h,image_src.size());

    imshow(window_proj,image_proj);
    waitKey(0);
    destroyWindow(window_proj);

    Rect roi(831,211,544,867);
    image_crop = image_proj(roi);

    imshow(window_crop,image_crop);
    waitKey(0);
    destroyWindow(window_crop);

    return 0;
}

