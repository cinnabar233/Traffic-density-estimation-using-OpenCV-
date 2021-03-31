#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include<vector>
#include<pthread.h>
#include<fstream>
#include<chrono>
#include<math.h>
using namespace cv;
using namespace std;
using namespace std::chrono;
void sparse_flow(VideoCapture capture,Mat h)
{

    // Create some random colors

    
    Mat old_frame,old_frame_0, old_gray;
    vector<Point2f> p0, p1;
    // Take first frame and find corners in it
    capture >> old_frame_0;
    Rect roi(831,211,544,867);
    warpPerspective(old_frame_0,old_frame, h,Size(1920,1080));
    old_frame = old_frame(roi);
    
    cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    
    Mat ori;
    Scalar color( 255,255,255);
    double i=1;
    while(true){
        Mat frame,frame_2, frame_gray;
        capture >> frame;
        if (frame.empty())
            break;
        capture >> frame;
        if (frame.empty())
            break;
        capture >> frame;
        if (frame.empty())
            break;
        warpPerspective(frame,frame_2, h,Size(1920,1080));
        frame = frame_2(roi);
        ori=frame.clone();

        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
        goodFeaturesToTrack(old_gray, p0, 100, 0.3,7, Mat(), 7, false, 0.04);
        // calculate optical flow
        vector<uchar> status;
        vector<float> err;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15,15), 2, criteria);
        vector<Point2f> good_new;
        double cnt=0;
        for(uint i = 0; i < p0.size(); i++)
        {
            // Select good points
            if(status[i] == 1) {
                double dist=((p1[i].x-p0[i].x)*(p1[i].x-p0[i].x)) + ((p1[i].y-p0[i].y)*(p1[i].y-p0[i].y));
               // cout<<dist<<" ";
                if(dist>25){
                    cnt++;
                    good_new.push_back(p1[i]);
                    circle(frame, p1[i], 10, color, -1);
                }
            }
        }
    
        double area = 0 ;
        double time=i/15;
        cout<<time<<","<<(cnt*0.0065)<<endl;
        i=i+3;

        // Now update the previous frame and previous points
        old_gray = frame_gray.clone();
    }
}
int main(int argc, char** argv)
{
    auto start = high_resolution_clock::now();
    
    string vid = argv[1];
    
    Mat image_proj;
    Mat image_src = imread("empty2.jpg" , IMREAD_GRAYSCALE);  // background image used in queue density, it has been extracted from the video itself

    vector<Point2f> pts_src,pts_dst;
    pts_src={Point2f(985,245),Point2f(1292,255),Point2f(1520,1056),Point2f(349,1055)};   //hardcoded the boundary points of road
    pts_dst={Point2f(831,211),Point2f(1375,211),Point2f(1375,1078),Point2f(831,1078)};
    Mat h = findHomography(pts_src,pts_dst);  // returns the homographic matrix
    
    warpPerspective(image_src,image_proj, h,Size(1920,1080));   // transformed image (1920x1080)
    
 
    freopen("out_sparse.txt","w",stdout); // file in which data will be written
    cout<<"frame,queue density"<<"\n";

    VideoCapture cap(vid);
    sparse_flow(cap,h);  // function to generate data
    //out.close();
    // fclose(stdout);
    auto stop = high_resolution_clock::now();
    
    
    auto duration = duration_cast<microseconds>(stop - start);
    
   // cout << duration.count();
    return 0;
}
