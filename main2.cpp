//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include<vector>

using namespace cv;
using namespace std;
double queue_density(Mat frame , Mat img )
{
    Mat gray,blurred,dst,thresh,dilated,contourOut,temp;
    vector<vector<Point> > contours;
    Mat frame_crop;
    Mat x=  Mat::zeros(img.rows, img.cols, CV_8UC3);
   
   cvtColor(frame, gray, COLOR_BGR2GRAY);
                                                    //  GaussianBlur(gray,blurred,Size(5,5),0
   absdiff(gray,img, temp);
   
                                                          //sharpens the image
   GaussianBlur(temp, temp, cv::Size(0, 0), 5);
   
   //addWeighted(temp, 1.5, dst, -0.5, 0, dst);

   threshold(temp,  thresh ,25, 255, THRESH_BINARY);
   
                                                       //  dilate(thresh,dilated, Mat(), Point(-1, -1), 3, 1, 1);
   
   contourOut = thresh.clone();
   
   findContours( thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
   
   for(int idx = 0 ; idx < contours.size(); idx++)
       {
        if(contourArea(contours[idx])>3000)
           {
               Scalar color( 0, 255, 0);
             // Scalar color( rand()&255, rand()&255, rand()&255);
              drawContours( x, contours, idx, color, FILLED, 8 );
              // drawContours( x, contours, idx, color,2 );
           }
          // v.push_back(contourArea(contours[idx]));

    }

          imshow("queue_density", x);        // displays contours in the frame
                                        // imshow("original",frame);  //original frame is also played
   double area = 0 ;
   for(auto contour : contours) area += contourArea(contour);
    return area/(544*867);
}


double dynamic_density( Mat nxt ,Mat prvs )
{
    // fill // do not the name the variable as nxt , usse gand mach rahi
       // cvtColor(frame, nxt, COLOR_BGR2GRAY);
        Mat flow(prvs.size(), CV_32FC2);
        calcOpticalFlowFarneback(prvs, nxt, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        // visualization
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr,bw,thresh;
         vector<vector < Point>> contours;


        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        cvtColor(bgr, bw, COLOR_BGR2GRAY);

        threshold(bw,  thresh ,4, 255, THRESH_BINARY);

        // imshow("frame2", hsv);

        //fgMask=thresh;
        
        erode(thresh,thresh, Mat(), Point(-1, -1), 1, 1, 1);

        findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        double area = 0 ;
     
     //   for(auto contour : contours) area += contourArea(contour);

        Mat x=  Mat::zeros(thresh.rows, thresh.cols, CV_8UC3);
        for(int idx = 0 ; idx < contours.size(); idx++)
            {
             if(contourArea(contours[idx])>7000)
                {
                    Scalar color( 0, 255, 0);
                  // Scalar color( rand()&255, rand()&255, rand()&255);
                    drawContours( x, contours, idx, color, FILLED, 8 );
                }
                area += contourArea(contours[idx]);

        }

        imshow("dynamic_density", x);
       // prvs = nxt;

      return area/(544*867);

}


void f(VideoCapture cap, Mat img,Mat h)
{
    Mat frame,frame_2;
    cap >> frame ;
    Mat prvs;
    Rect roi(831,211,544,867);
    warpPerspective(frame,frame_2, h,Size(1920,1080));
    frame = frame_2(roi);
    cvtColor(frame, prvs, COLOR_BGR2GRAY);
    int cnt = 0 ;
    double time;
    while(true)
    {
        Mat nxt;
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
        time=cnt/15;
       // cout<<cnt/15<<","<<queue_density(frame_2,img)<<","<<dynamic_density(frame_2,nxt, prvs)<<"\n";
        cvtColor(frame_2, nxt, COLOR_BGR2GRAY);
        cout<<time<<","<<queue_density(frame_2,img)<<","<<dynamic_density(nxt, prvs)<<"\n";
        prvs = nxt;
        //  imshow("output", contourOut);
        int key = waitKey(30);
        if(key == 'q')  break;

    }
}


int main(int argc, char** argv)
{
    string vid = argv[1];
    Mat image_proj;
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





