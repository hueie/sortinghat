/*
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <math.h>
#include <string.h>
#include <time.h>

#include "DetectLabel.cpp"
#include "LabelOCR.cpp"

using namespace cv;
using namespace cv::ml;
using namespace std;



string pathLabels1 = "C:/Users/Kait/git/sortinghat/sortinghat/ml/LabelDataset/label1_";
string pathLabels2 = "C:/Users/Kait/git/sortinghat/sortinghat/ml/LabelDataset/label2_";
string path_NoLabels = "C:/Users/Kait/git/sortinghat/sortinghat/ml/LabelDataset/noLabel_";
string result_xml_filepath = "C:/Users/Kait/git/sortinghat/sortinghat/ml/SVM.xml";
int numLabel1=100;
int numLabel2=100;
int numNoLabels=200;
int imageWidth=400;
int imageHeight=200;

void generateLabelDataset(VideoCapture cap, int numData, int nClass){
	DetectLabel detectLabels;
	detectLabels.showBasicImages = true;
	vector<Mat> label;
	Mat normalImage;
	string path_data;
	int i = 0;


	if (nClass == 1){
	    path_data = pathLabels1;
		numLabel1 = numData;
	} else if (nClass == 2){
		path_data = pathLabels2;
		numLabel2 = numData;
	} else{
		path_data = path_NoLabels;
		numNoLabels = numData;
	}

	while (i < numData) {
		cap >> normalImage;
		detectLabels.segment(normalImage, label);
		// TODO: RGB to gray
		if (label.size() > 0){
			if (!label[0].empty()){
				stringstream ss;
				ss << path_data << i << ".jpg";
				imwrite(ss.str(), label[0]);
				cout << "path = "<< ss.str() << endl;
				//cout <<  label[0].cols << endl;
				i++;
			}
		}
		namedWindow("normalImage",WINDOW_NORMAL);
		imshow("normalImage",normalImage);
		label.clear();
		if(waitKey(30) >= 0) break;
	}
}

void labelToXml(){
    Mat classes;//(numLabel1+numLabel2+numNoLabels, 1, CV_32FC1);
    Mat trainingData;//(numLabel+numNoLabels, imageWidth*imageHeight, CV_32FC1 );

    Mat trainingImages;
    vector<int> trainingLabels;

    cout << numLabel1 << endl;
    cout << path_NoLabels << endl;

    for(int i=0; i< numLabel1; i++)
    {
        stringstream ss(stringstream::in | stringstream::out);
        ss << pathLabels1 << i << ".jpg";
        cout << "read path = "<< ss.str() << endl;
        Mat img=imread(ss.str(), 0);
        img= img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(1);
    }

    for(int i=0; i< numLabel2; i++)
    {
        stringstream ss(stringstream::in | stringstream::out);
        ss << pathLabels2 << i << ".jpg";
        cout << "read path 2= "<< ss.str() << endl;
        Mat img=imread(ss.str(), 0);
        img= img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(2);
    }

    for(int i=0; i< numNoLabels; i++)
    {
        stringstream ss(stringstream::in | stringstream::out);
        ss << path_NoLabels << i << ".jpg";
        cout << "read path 3= "<< ss.str() << endl;
        Mat img=imread(ss.str(), 0);
        if (img.empty()) break;
        img= img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(0);

    }

    Mat(trainingImages).copyTo(trainingData);
    //trainingData = trainingData.reshape(1,trainingData.rows);
    trainingData.convertTo(trainingData, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);

    FileStorage fs(result_xml_filepath, FileStorage::WRITE);
    fs << "TrainingData" << trainingData;
    fs << "classes" << classes;
    fs.release();
}

void generateDatasetModule(){
	VideoCapture cap(1); // open the default camera
	    if(!cap.isOpened()){  // check if we succeeded
	        return;
	    }
	    int opcion;
	    cout << "OpenCV Training SVM " << endl;

		while(true){
			cout << "1) Generate Label 1 Sample From Camera. " << endl;
			cout << "2) Generate Label 2 Sample From Camera. " << endl;
			cout << "3) Generate None Label Sample From Camera. " << endl;
			cout << "4) Generate XML File. " << endl;
			cin >> opcion;
			switch (opcion){
				case 1:
					generateLabelDataset(cap, numLabel1, 1);
					break;
				case 2:
					generateLabelDataset(cap, numLabel2, 2);
					break;
				case 3:
					generateLabelDataset(cap, numNoLabels, 0);
					break;
				case 4:
					labelToXml();
					break;
				default:
					break;
			}
		}
}

int main( int argc, char** argv )
{


	generateDatasetModule();


	//
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
*/
