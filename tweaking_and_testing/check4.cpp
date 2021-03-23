#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

vector<Point2f> pts_src,pts_dst;

Mat image_src ; // original image
Mat image_proj ; // image after homogographic projection;
Mat image_crop ; // cropped image
Mat image_src_1;
string window_src  , window_proj, window_crop ;

void callbackfnc(int event,int x,int y,int flags,void *userdata)
{
    if(flags == (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON)){
        pts_src.clear();
        image_src_1=image_src.clone();
        imshow(window_src,image_src_1);
    }
    else if(event == EVENT_LBUTTONDOWN && (pts_src.size()<4)){
        // clicked points
        pts_src.push_back(Point2f(x,y));
        circle(image_src_1 , Point(x,y) , 16 , Scalar(0,0,0) ,FILLED, 8) ; // to draw a circle at the clicked points
        imshow(window_src,image_src_1);
    }

}

/*sort the pts in anti-clockwise order starting from top-left pt */
void sort_inputpts(vector<Point2f> &pts_src)
{
    sort(pts_src.begin(),pts_src.end(),[](const cv::Point2f &a, const cv::Point2f &b){
        return a.y<b.y ;
    });        // sort pts by y co-ordinate
    
    if(pts_src[0].x>pts_src[1].x){
        swap(pts_src[0],pts_src[1]);
    }
    if(pts_src[2].x<pts_src[3].x){
        swap(pts_src[2],pts_src[3]);
    }
}


int main()
{
    string vid = "traffic.mp4";
    VideoCapture capture(vid);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }

    
    image_src = imread("empty2.jpg" , IMREAD_GRAYSCALE);
    image_src_1=image_src.clone();
    if(image_src.empty())
    {
        cout<<"Source image not found"<<endl;
        cin.get();  return -1;
    }
    window_src = "original empty";
    namedWindow(window_src);

    setMouseCallback(window_src,callbackfnc,NULL);
    imshow(window_src,image_src_1);
    waitKey(0);
   //  //for(Point2f p : pts_src) circle(image_src , Point(p.x,p.y) , 16 , Scalar(255,0,0) ,FILLED, 8) ; // to draw a circle at the clicked points
    destroyWindow(window_src);
   
   
    pts_dst.push_back(Point2f(831,211));        // destination points where the corners of main road are mapped to, better points can be chosen
    pts_dst.push_back(Point2f(1375,211));
    pts_dst.push_back(Point2f(1375,1078));
    pts_dst.push_back(Point2f(831,1078));
    
     sort_inputpts(pts_src);     // sort the inputs in anti-clockwise order starting from top-left point
    
    Mat h =findHomography(pts_src,pts_dst);  // returns the homographic matrix
    warpPerspective(image_src,image_proj, h,Size(1920,1080));   // transformed image (1920x1080)
    
    
    Rect roi(831,211,544,867);              // cropped image (544x867)
    image_crop = image_proj(roi);
    
    Mat bg,bg1,bg_final;
    bg = image_crop;
    
    freopen("out.csv","w",stdout);
    cout<<"Frame,Fraction\n";


    int cnt = 0 ;
    vector<vector < Point>> contours;


    Mat frame1, prvs,fgMask,x;
    capture >> frame1;
    warpPerspective(frame1,frame1, h,Size(1920,1080));
    frame1 = frame1(roi);
    cvtColor(frame1, prvs, COLOR_BGR2GRAY);
    while(true){
        Mat frame2, next,frame_original,frame_2;

        capture >> frame_original;
        capture >> frame_original;
        warpPerspective(frame_original,frame_2, h,Size(1920,1080));
        frame2 = frame_2(roi);

       // capture >> frame2;

        cvtColor(frame2, next, COLOR_BGR2GRAY);
        Mat flow(prvs.size(), CV_32FC2);
        calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        // visualization
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr,bw,thresh;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        cvtColor(bgr, bw, COLOR_BGR2GRAY);

        threshold(bw,  thresh ,4, 255, THRESH_BINARY);

        imshow("frame2", thresh);

        fgMask=thresh;
        
        erode(fgMask,fgMask, Mat(), Point(-1, -1), 1, 1, 1);

        findContours(fgMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        double area = 0 ;
     
     //   for(auto contour : contours) area += contourArea(contour);

        Mat x=  Mat::zeros(fgMask.rows, fgMask.cols, CV_8UC3);

        for(int idx = 0 ; idx < contours.size(); idx++)
            {
             if(contourArea(contours[idx])>5000)
                {
                    Scalar color( 0, 255, 0);
                  // Scalar color( rand()&255, rand()&255, rand()&255);
                    drawContours( x, contours, idx, color, FILLED, 8 );
                }
                area += contourArea(contours[idx]);

        }

        imshow("output", x); 
        cnt=cnt+2;
        cout<<cnt<<","<<area/(544*867)<<"\n";
        //  Mat y=  Mat::zeros(50, 100, CV_8UC3);
        // imshow("pppp",y);
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
        prvs = next;
    }
}
