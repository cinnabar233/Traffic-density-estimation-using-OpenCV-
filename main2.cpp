//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include<vector>

using namespace cv;
using namespace std;
void queue_density(Mat frame , Mat img, Mat h , int cnt )
{
    Mat gray,blurred,dst,thresh,dilated,contourOut,temp;
    vector<vector<Point> > contours;
    Mat frame_crop;
    Mat x=  Mat::zeros(img.rows, img.cols, CV_8UC3);
   
   cvtColor(frame, gray, COLOR_BGR2GRAY);
                                                    //  GaussianBlur(gray,blurred,Size(5,5),0
   absdiff(gray,img, temp);
   
                                                          //sharpens the image
   GaussianBlur(temp, dst, cv::Size(0, 0), 3);
   
   addWeighted(temp, 1.5, dst, -0.5, 0, dst);

   threshold(temp,  thresh ,50, 255, THRESH_BINARY);
   
                                                       //  dilate(thresh,dilated, Mat(), Point(-1, -1), 3, 1, 1);
   
   contourOut = thresh.clone();
   
   findContours( thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   
   for(int idx = 0 ; idx < contours.size(); idx++)
       {
        if(contourArea(contours[idx])>1000)
           {
               // Scalar color( 0, 255, 0);
              Scalar color( rand()&255, rand()&255, rand()&255);
              drawContours( x, contours, idx, color, FILLED, 8 );
              // drawContours( x, contours, idx, color,2 );
           }
          // v.push_back(contourArea(contours[idx]));

    }

                                         imshow("output", x);        // displays contours in the frame
                                        // imshow("original",frame);  //original frame is also played
   double area = 0 ;
   for(auto contour : contours) area += contourArea(contour);
   cout<<((double)cnt)/15<<","<<area/(544*867)<<"\n";
}
void f(VideoCapture cap, Mat img,Mat h)
{
    Mat frame,frame_2;
    cap >> frame ;
    Mat prvs,nxt;
    Rect roi(831,211,544,867);
    warpPerspective(frame,frame, h,Size(1920,1080));
    frame = frame(roi);
    cvtColor(frame, prvs, COLOR_BGR2GRAY);
    int cnt = 0 ;
    while(true)
    {
        cap >> frame ;
        if(frame.empty()) break ;
        cap >> frame ;
        if(frame.empty()) break ;
        cap >> frame ;
        if(frame.empty()) break ;
        
        warpPerspective(frame,frame_2, h,Size(1920,1080));
        frame_2 = frame_2(roi);
        // capture >> frame2;
        cnt+=3;
        q(frame_2,img,h,cnt);
        cvtColor(frame_2, nxt, COLOR_BGR2GRAY);
        //  imshow("output", contourOut);
        // dynamic density(frame,prvs);
        int key = waitKey(30);
        if(key == 'q')  break;
        prvs = nxt;
    }
}

void dynamic_density(Mat frame , Mat prvs )
{
    
    // fill //
}
int main(int argc, char** argv)
{
    string vid = argv[1];
    Mat image_src = imread("empty2.jpg" , IMREAD_GRAYSCALE);
    vector<Point2f> pts_src,pts_dst;
    pts_src={Point2f(968,202),Point2f(1293,225),Point2f(1559,1056),Point2f(399,1050)};
    pts_dst={Point2f(831,211),Point2f(1375,211),Point2f(1375,1078),Point2f(831,1078)};
    Mat h = findHomography(pts_src,pts_dst);  // returns the homographic matrix
    warpPerspective(image_src,image_proj, h,Size(1920,1080));   // transformed image (1920x1080)
    Rect roi(831,211,544,867);              // cropped image (544x867)
    Mat bg = image_proj(roi);
    VideoCapture cap(vid);
    freopen("out.csv","w",stdout);
    cout<<"Frame,Queue density,Dynamic density\n";
    f(cap,bg,h);


    return 0;
}





