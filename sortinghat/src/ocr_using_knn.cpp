
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <math.h>
#include <string.h>
#include <time.h>

#include "DetectLabel.cpp"
#include "LabelOCR.cpp"

using namespace cv;
using namespace cv::ml;
using namespace std;


int main( int argc, char** argv )
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return (-1);

    Mat normalImage, modImage, cropImage1, labelImage1;
    Mat cropImage2, labelImage2, binImage;
    vector<Point> contour;
    vector<vector<Point> > contours;
    Rect label1ROI;

    string text1, text2;

    DetectLabel detectLabels;
    LabelOCR labelOcr;
    Ptr<SVM> svmClassifier = SVM::create();;

    vector<Mat> possible_labels, label_1, label_2;
    vector<string> labelText1, labelText2;
    detectLabels.showBasicImages = true;
    detectLabels.showAllImages = true;

	namedWindow("normal",WINDOW_NORMAL);

	// SVM learning algorithm

	clock_t begin_time = clock();
	// Read file storage.
	FileStorage fs;
	fs.open("/home/turtlebot/catkin_ws/src/opencv_01/src/vision/ml/SVM.xml", FileStorage::READ);
	Mat SVM_TrainingData;
	Mat SVM_Classes;
	fs["TrainingData"] >> SVM_TrainingData;
	fs["classes"] >> SVM_Classes;
	//Set SVM params
	svmClassifier->setType(SVM::C_SVC);
	svmClassifier->setKernel(SVM::LINEAR);
	svmClassifier->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.01));
	svmClassifier->setDegree(0);
	svmClassifier->setGamma(1);
	svmClassifier->setCoef0(0);
	svmClassifier->setC(1);
	svmClassifier->setNu(0);
	svmClassifier->setP(0);
	svmClassifier->train(SVM_TrainingData, ROW_SAMPLE, SVM_Classes);

	float timer = ( clock () - begin_time ) /  CLOCKS_PER_SEC;
	cout << "Time: " << timer << endl;

	while(true){

		cap >> normalImage; // get a new frame from camera
		imshow("normal", normalImage);

		possible_labels.clear();
		label_1.clear();
		label_2.clear();

		// segmentation
		detectLabels.segment(normalImage,possible_labels);

		int posLabels = possible_labels.size();
		if (posLabels > 0){
			//For each possible label, classify with svm if it's a label or no
			for(int i=0; i< posLabels; i++)
				{
				if (!possible_labels[i].empty() ){
					Mat gray;
					cvtColor(possible_labels[i], gray, COLOR_RGB2GRAY);
					Mat p= gray.reshape(1, 1);
					p.convertTo(p, CV_32FC1); // CV_32FC1
					int response = (int)svmClassifier->predict( p );
					cout << "Class: " << response << endl;
					if(response==1)
						label_1.push_back(possible_labels[i]);
					if(response==2)
						label_2.push_back(possible_labels[i]);
					}
			}
		}
		if ( label_1.size() > 0) {
			labelText1 = labelOcr.runRecognition(label_1,1);
		}
		if ( label_2.size() > 0) {
			labelText2 = labelOcr.runRecognition(label_2,2);
		}


		if(waitKey(30) >= 0) break;
	}


	return (0);
}
