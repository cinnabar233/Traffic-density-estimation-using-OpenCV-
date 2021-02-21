//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>

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
    if(event == EVENT_LBUTTONDOWN){
        // clicked points
        pts_src.push_back(Point2f(x,y));
        circle(image_src_1 , Point(x,y) , 16 , Scalar(255,0,0) ,FILLED, 8) ; // to draw a circle at the clicked points
        imshow(window_src,image_src_1);
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
int main(int argc, char** argv)
{
    
    image_src = imread(string(argv[1])+string(".jpg") , IMREAD_GRAYSCALE);
    image_src_1 = imread(string(argv[1])+string(".jpg") , IMREAD_GRAYSCALE);
    if(image_src.empty())
    {
        cout<<"Source image not found"<<endl;
        cin.get();  return -1;
    }

    window_src = "Original "+string(argv[1]) ;  window_proj= "Transformed "+string(argv[1]) ; window_crop = "Cropped "+string(argv[1]);

    namedWindow(window_src); 
    
    setMouseCallback(window_src,callbackfnc,NULL);
    imshow(window_src,image_src_1);
    waitKey(0);
    //for(Point2f p : pts_src) circle(image_src , Point(p.x,p.y) , 16 , Scalar(255,0,0) ,FILLED, 8) ; // to draw a circle at the clicked points
    destroyWindow(window_src);
   
   
    pts_dst.push_back(Point2f(831,211));
    pts_dst.push_back(Point2f(831,1078));              // destination points where the corners of main road are mapped to, better points can be chosen
    pts_dst.push_back(Point2f(1375,1078));
    pts_dst.push_back(Point2f(1375,211));
    
    Mat h =findHomography(pts_src,pts_dst);  // returns the homographic matrix
    warpPerspective(image_src,image_proj, h,image_src.size());
    
    if( !display_and_save(window_proj,image_proj) ) {
        save_error(window_proj) ; return -1;
    }
    
    Rect roi(831,211,544,867);
    image_crop = image_proj(roi);
    
    if( !display_and_save(window_crop,image_crop) ) {
        save_error(window_crop) ; return -1;
    }
    
    return 0;
}

