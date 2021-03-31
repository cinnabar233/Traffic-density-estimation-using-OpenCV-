//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include<vector>
#include<pthread.h>
#include<fstream>
#include<chrono>
using namespace cv;
using namespace std;
using namespace std::chrono;
// returns the queue density in frame, by subracting the background image

int thread_num=3;

struct queue_params{
    Mat frame ; Mat img ;
    double area;
};

 double queue_density(Mat frame , Mat img)
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
        if(contourArea(contours[idx])>5000)  // area less than 5000 is mostly noise in image
           {
                Scalar color( 0, 255, 0);
                drawContours( x, contours, idx, color, FILLED, 8 ); // filled countours are drawn in frame x, for visualisation of recognised contours
                area += contourArea(contours[idx]);
           }

    }

    return (area/(544*867));  //queue density is returned
}



void *f(void *x)
{
    queue_params *z = (queue_params *)x;
    double area = queue_density(z->frame, z->img /* , z.v*/);
    double *val ;
    val = &area;
    z->area=area;

    pthread_exit(NULL);
}



vector<double> run_queue_density(VideoCapture cap, Mat img,Mat h)
{
      Mat frame,frame_2 ,cropped;

       pthread_t threads[thread_num];  queue_params parts[thread_num];
       vector<double> vals;
       cap >> frame ;
       Rect roi_parts[thread_num];
       Rect roi(831,211,544,867);
       
       for(int i=0;i<thread_num;i++){
           roi_parts[i]=Rect(i*(544/thread_num),0,(544/thread_num),867);
       }
       
       warpPerspective(frame,frame_2, h,Size(1920,1080));
       frame = frame_2(roi);
  
       int cnt = 0 ; // denotes the frame number
       double time;  // denotes time stamp of frame
       while(true)
       {
          
           cap >> frame ; //in order to shorten the length of video we pick every third frame  ( 15/3 = 5 fps)
           if(frame.empty()) return vals ;
           cap >> frame ;
           if(frame.empty()) return vals ;
           cap >> frame ;
           if(frame.empty()) return vals ;
           
           warpPerspective(frame,frame_2, h,Size(1920,1080));
           
           for(int i=0;i<thread_num;i++){
    
               cropped = frame_2(roi);
               
               parts[i]={cropped(roi_parts[i]),img(roi_parts[i])};
               
               int l = pthread_create(&threads[i] , NULL , f , (void *)(&parts[i]));
               
               if(l){
                   cout << "could not make thread" ;
                   exit(-1);
               }
               
           }
           cnt+=3*thread_num;
           time=cnt/15;
           
           double area=0.0;
           
           for(int i=0;i<thread_num;i++){
               pthread_join(threads[i] , NULL);
               area=area+parts[i].area;
           }
           vals. push_back(area);

       }
       return vals ;
}

void generate(string vid, Mat img,Mat h)
{
    VideoCapture cap1(vid);
    vector<double> v1 = run_queue_density(cap1 , img , h) ;

    int cnt = 1 ;
    for(auto x : v1)
    {
        cout << cnt << "," << x << "\n";
        cnt++;
    }
}


int main(int argc, char** argv)
{
    auto start = high_resolution_clock::now();
    
    string vid = argv[1];
    thread_num = atoi(argv[2]);
    Mat image_proj;
    Mat image_src = imread("empty2.jpg" , IMREAD_GRAYSCALE);  // background image used in queue density, it has been extracted from the video itself

    vector<Point2f> pts_src,pts_dst;
    pts_src={Point2f(985,245),Point2f(1292,255),Point2f(1520,1056),Point2f(349,1055)};   //hardcoded the boundary points of road
    pts_dst={Point2f(831,211),Point2f(1375,211),Point2f(1375,1078),Point2f(831,1078)};
    Mat h = findHomography(pts_src,pts_dst);  // returns the homographic matrix
    
    warpPerspective(image_src,image_proj, h,Size(1920,1080));   // transformed image (1920x1080)
    
    
    Rect roi(831,211,544,867);
    Mat bg = image_proj(roi); // cropped image (544x867)
 
   
    freopen("out_spatial.txt","w",stdout); // file in which data will be written
    cout<<"frame,queue density"<<"\n";

    generate(vid,bg,h);  // function to generate data
    //out.close();
    // fclose(stdout);
    auto stop = high_resolution_clock::now();
    
    
    auto duration = duration_cast<microseconds>(stop - start);
    
    // cout << duration.count();
    return 0;
}










