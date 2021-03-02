#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include<vector>
using namespace cv;
using namespace std;


void queue_density(VideoCapture cap, Mat img)
{
    Mat gray,frame,blurred,dst,thresh,dilated,contourOut;
    vector<vector<Point> > contours;
    while(true)
    {
        cap >> frame ;
        if(frame.empty())
            {
                break;
            }
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        
        GaussianBlur(gray,blurred,Size(5,5),0);
        
        absdiff(blurred, img, dst);
        
        threshold(dst,  thresh ,30, 255, THRESH_BINARY);
        
        dilate(thresh,dilated, Mat(), Point(-1, -1), 3, 1, 1);
        
        contourOut = dilated.clone();
        
        findContours( contourOut, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        
        double area = 0 ;
        
        for(auto contour : contours) area += contourArea(contour);
        imshow("output", contourOut);
        int key = waitKey(30);

        if(key == 'q')  break;
    }
}
int main()
{
    Mat bg,bg1,bg_final;
    bg = imread("empty.jpg");
    cvtColor(bg, bg1, COLOR_BGR2GRAY);
    GaussianBlur( bg1 , bg_final ,Size(5,5),0);
    VideoCapture cap("traffic.mp4");
    
    queue_density(cap,bg_final);
}



/*threshold( src_gray, dst, threshold_value, max_binary_value, threshold_type );
dilate( src, dilation_dst, element );
findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );*/
