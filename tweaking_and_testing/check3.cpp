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
const char* params
    = "{ help h         |           | Print usage }"
      "{ input          | vtest.avi | Path to a video or a sequence of image }"
      "{ algo           | MOG2      | Background subtraction method (KNN, MOG2) }";


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

/* to display and save the image*/
bool display_and_save( string window_name , Mat image)
{
    imshow(window_name , image);
    waitKey(0);
    destroyWindow(window_name);
    return imwrite(window_name + string(".jpg") , image);
    
}
/* printing error message*/
void save_error(string name )
{
    cout<<"The image "+ name + " could not be saved\n" ;
    cin.get();
}


int main(int argc, char* argv[])
{
    CommandLineParser parser(argc, argv, params);
    parser.about( "This program shows how to use background subtraction methods provided by "
                  " OpenCV. You can process both videos and images.\n" );
    if (parser.has("help"))
    {
        //print help information
        parser.printMessage();
    }
    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub;
    if (parser.get<String>("algo") == "MOG2")
        pBackSub = createBackgroundSubtractorMOG2();
    else
        pBackSub = createBackgroundSubtractorKNN();
    VideoCapture capture( samples::findFile( parser.get<String>("input") ) );
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open: " << parser.get<String>("input") << endl;
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
    Mat frame_original, frame,frame_2, fgMask ;
    int cnt = 0 ;
    vector<vector < Point>> contours;
    while (true) {
        capture >> frame_original;
        warpPerspective(frame_original,frame_2, h,Size(1920,1080));
        frame = frame_2(roi);
        if (frame.empty())
            break;
        
        //update the background model
        pBackSub->apply(frame, fgMask);
        //get the frame number and write it on the current frame
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        stringstream ss;
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,255,0));
        //show the current frame and the fg masks
        imshow("Frame", frame);
      //  imshow("FG Mask", fgMask);
       // dilate(fgMask,fgMask, Mat(), Point(-1, -1), 1, 1, 1);

        findContours(fgMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        double area = 0 ;
     
     //   for(auto contour : contours) area += contourArea(contour);

        Mat x=  Mat::zeros(fgMask.rows, fgMask.cols, CV_8UC3);

        for(int idx = 0 ; idx < contours.size(); idx++)
            {
             if(contourArea(contours[idx])>200)
                {
                    Scalar color( 0, 255, 0);
                  // Scalar color( rand()&255, rand()&255, rand()&255);
                    drawContours( x, contours, idx, color, FILLED, 8 );
                }
                area += contourArea(contours[idx]);

        }

        imshow("output", x); 

        cnt++;
        cout<<cnt<<","<<area/(544*867)<<"\n";
        //get the input from the keyboard
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }
    return 0;
}








