//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include<vector>

using namespace cv;
using namespace std;
int r ;
// returns the queue density in frame, by subracting the background image
double queue_density(Mat frame , Mat img )
{
    Mat gray,blurred,dst,thresh,dilated,contourOut,temp;
    vector<vector<Point> > contours;

   cvtColor(frame, gray, COLOR_BGR2GRAY);   // frame is converted to gray scale
                                                  
   absdiff(gray,img, temp);     // absolute difference is taken b/w current frame and background image

   dilate(temp,temp, Mat(), Point(-1, -1), 2, 1, 1);
                                                        
   GaussianBlur(temp, temp, cv::Size(0, 0), 2);   // image is dilated and blurred for better enclosure of vehicles
   
   //addWeighted(temp, 1.5, dst, -0.5, 0, dst);

   threshold(temp,  thresh ,40, 255, THRESH_BINARY);  // gray image is converted to black and white image for finding contours
   
   findContours( thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

   Mat x=  Mat::zeros(img.rows, img.cols, CV_8UC3);
   double area = 0 ;
   for(int idx = 0 ; idx < contours.size(); idx++)
       {
        if(contourArea(contours[idx])*r*r>5000)  // area less than 5000 is mostly noise in image
           {
                //Scalar color( 0, 255, 0);
               // drawContours( x, contours, idx, color, FILLED, 8 ); // filled countours are drawn in frame x, for visualisation of recognised contours
                area += contourArea(contours[idx]);
           }

    }


    return (area*r*r)/(544*867);  //queue density is returned
}

// function to generate data
void generate(VideoCapture cap, Mat img,Mat h,Size sz)
{
    Mat frame,frame_2;
   
    resize(img, img,sz );
    
    // cap >> frame ;
    Rect roi(831,211,544,867);
    // warpPerspective(frame,frame_2, h,Size(1920,1080));
    // frame = frame_2(roi);
    
    // resize(frame, frame,sz );
    
    int cnt = 0 ; // denotes the frame number
    double time;  // denotes time stamp of frame
    while(true)
    {
        cap >> frame ;
        if(frame.empty()) break ;
        cap >> frame ;
        if(frame.empty()) break ;
        cap >> frame ;
        if(frame.empty()) break ;
        //in order to shorten the length of video we pick every third frame  ( 15/3 = 5 fps)
        
        
        warpPerspective(frame,frame_2, h,Size(1920,1080));
        
        frame_2 = frame_2(roi);
        
        resize(frame_2, frame_2,sz );
        //imshow("scaled", frame_2);
        cnt++;
        cout<<cnt<<","<<queue_density(frame_2,img)<<"\n";
        //int key = waitKey(30);
        //if(key == 'q')  break;

    }
}


int main(int argc, char** argv)
{
    string vid = argv[1];
    r = atoi(argv[2]) ; // c  = atoi(argv[3]);
    Mat image_proj;
    Mat image_src = imread("empty2.jpg" , IMREAD_GRAYSCALE);  // background image used in queue density, it has been extracted from the video itself
    
    vector<Point2f> pts_src,pts_dst;
    pts_src={Point2f(985,245),Point2f(1292,255),Point2f(1520,1056),Point2f(349,1055)};   //hardcoded the boundary points of road
    pts_dst={Point2f(831,211),Point2f(1375,211),Point2f(1375,1078),Point2f(831,1078)};
    Mat h = findHomography(pts_src,pts_dst);  // returns the homographic matrix
    warpPerspective(image_src,image_proj, h,Size(1920,1080));   // transformed image (1920x1080)
    Rect roi(831,211,544,867);
    Mat bg = image_proj(roi); // cropped image (544x867)
    
    Size sz =  Size(bg.cols/r, bg.rows/r);
    
    VideoCapture cap(vid);

    freopen("out_resolution.txt","w",stdout); // file in which data will be written

    cout<<"frame,queue density"<<"\n";

    generate(cap,bg,h,sz);  // function to generate data


    return 0;
}







