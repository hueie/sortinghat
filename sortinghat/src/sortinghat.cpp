#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;
using namespace cv;

void test1();

int main() {
    /* File Write*/
    ofstream outputFile;
    outputFile.open("points.txt");


    namedWindow("clustering",1);
    cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
    const string& filename = "C:/upload/sample.mp4";
    VideoCapture cap(filename);
    if(!cap.isOpened())  // check if we succeeded
        return 0;

    Mat edges;
    Mat curframe, preframe;
    int tot_frame_cnt = cap.get(CV_CAP_PROP_FRAME_COUNT);
    cout << tot_frame_cnt << endl;

    //start getting first cut In order to Use Web and sample Image
    cap >> curframe;
    preframe = curframe.clone();

    int height = curframe.rows;
    int width = curframe.cols;
    cout << "height : "<< height << " width : " << width << endl ;
    imwrite("sample1.jpg", curframe);
    //end

    int cluster_count = 3;//8
    int accuracy = 100;// 0 to 255 black to white

    Mat labels, centers;
    int tmp_size = height * width * 2; //float * tmp_points = new float[320*480*2];
    float * tmp_points = new float[tmp_size];
    float * ptmp_points = NULL;
    int frame_idx=0;
    for(;;) {
        cap >> curframe; // get a new frame from camera
        edges = curframe - preframe;
        preframe = curframe.clone();

        int points_idx = 0;
        for(int y = 0; y < edges.rows; y++) {
               for(int x = 0;x < edges.cols; x++) {
                   //if(edges.at<uchar>(y,x) != 0){ //Not Black
                   if( accuracy < abs(edges.at<uchar>(y,x))){ //More brighter than gray
                       tmp_points[points_idx] = y;
                       tmp_points[points_idx + height*width] = x;
                       //cout << x << y << endl;
                       points_idx++;
                   }
            }
        }
        //cout << "Finish putting data" << endl;

        //Start Transfering Data To Fit with cv function
        tmp_size = (points_idx+1)*2;
        ptmp_points = new float[tmp_size];
        for(int i=0; i<points_idx; i++){
            ptmp_points[i] = tmp_points[i];
            ptmp_points[i + points_idx] = tmp_points[i + points_idx];
        }

        //cout << "kmeans start" << endl;
        if(points_idx+1 > cluster_count){
            Mat points(points_idx+1, 2, CV_32F, ptmp_points); //2 : (y,x) array(matrix)
            kmeans(points, cluster_count, labels,
                    cvTermCriteria( CV_TERMCRIT_EPS, 1000, 0 ), 1000, KMEANS_RANDOM_CENTERS, centers);
            //cout << "centers : " << centers << endl;
            for(int i=0; i<cluster_count; i++){
                outputFile << centers.at<float>(i,0) << " " << centers.at<float>(i,1) << "/";
                //1
                //circle(curframe, Point(centers.at<float>(i,0), centers.at<float>(i,1)), 5, Scalar(255, 255, 255));
                //2
                //circle(edges, Point(centers.at<uchar>(i,1), centers.at<uchar>(i,0)), 5, Scalar(255, 255, 255));
            }
            outputFile << endl;
        }/* else if(points_idx+1 <= cluster_count || points_idx > 0) {
            Mat points(points_idx+1, 2, CV_32F, ptmp_points); //2 : (y,x) array(matrix)
            kmeans(points, points_idx+1, labels,
                    cvTermCriteria( CV_TERMCRIT_EPS, 1000, 0 ), 1000, KMEANS_RANDOM_CENTERS, centers);
            //cout << "centers : " << centers << endl;
            for(int i=0; i<points_idx+1; i++){
                circle(curframe, Point(centers.at<float>(i,0), centers.at<float>(i,1)), 5, Scalar(255, 255, 255));
                //circle(edges, Point(centers.at<uchar>(i,1), centers.at<uchar>(i,0)), 5, Scalar(255, 255, 255));
            }
        } */

        //1
        //imshow("clustering", curframe);
        //2
        //imshow("clustering", edges);
        if(waitKey(30) >= 0) break;

        delete ptmp_points;
    }
    outputFile.close();
    return 0;
}


void test1(){

    cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

    const string& filename = "sample1.mp4";

    VideoCapture cap(filename);

    if(!cap.isOpened())  // check if we succeeded

        return ;


    Mat edges;

    namedWindow("edges",1);

    Mat curframe, preframe;


    //start getting first cut In order to Use Web and sample Image

    cap >> curframe;

    cout << "height : "<< curframe.rows << " width : " << curframe.cols <<endl ;

    imwrite("sample1.jpg", curframe);

    //end


    for(;;) {

        preframe = curframe.clone();

        cap >> curframe; // get a new frame from camera

        //imshow("edges", curframe);

        edges = curframe - preframe;


        int min_y = 99999, max_y = 0, min_x = 99999, max_x = 0;

        for(int y = 0; y < edges.rows; y++) {

            for(int x = 0;x < edges.cols; x++) {

                   //edges.at<Vec3b>(y,x)[1,2,3] RGB

                if(edges.at<uchar>(y,x) != 0){

                //Not Black

                    if(y < min_y){ min_y = y; }

                    if(y > max_y){ max_y = y; }

                    if(x < min_x){ min_x = x; }

                       if(x > max_x){ max_x = x; }

                   }

            }

        }

        if(min_x == 0){ min_x = 1; }

        if(min_y == 0){ min_y = 1; }


        int thickness = 2;

        int lineType = 8;

        line(edges, Point(max_y, max_x), Point(min_y, max_x), Scalar(255, 0, 0), thickness, lineType);

           line(edges, Point(max_y, min_x), Point(min_y, min_x), Scalar(255, 0, 0), thickness, lineType);

           line(edges, Point(max_y, max_x), Point(max_y, min_x), Scalar(255, 0, 0), thickness, lineType);

           line(edges, Point(min_y, max_x), Point(min_y, min_x), Scalar(255, 0, 0), thickness, lineType);


           imshow("edges", edges);

        if(waitKey(30) >= 0) break;

    }

}
