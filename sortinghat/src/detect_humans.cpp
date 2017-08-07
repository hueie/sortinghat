#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	VideoCapture cap("C:/upload/sample2.mp4");
	// CvCapture cap = cvCreateFileCapture("TownCentreXVID.avi");

	if (!cap.isOpened()) {
		// if not success, exit program
		cout << "Cannot open the video file" << endl;
		return -1;
	}
	double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
	cout << "Frame per seconds : " << fps << endl;

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1) {
		Mat img;
		bool bSuccess = cap.read(img); // read a new frame from video
		if (!bSuccess) {
			//if not success, break loop
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		cvtColor(img, img, CV_RGB2GRAY);
		HOGDescriptor hog;
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		vector<Rect> people, people_filtered; //Vector == Array
		hog.detectMultiScale(img, people, 0, Size(8, 8), Size(128, 128));
		/*
		http://docs.opencv.org/3.2.0/d5/d33/structcv_1_1HOGDescriptor.html

		InputArray 	img,
		std::vector< Rect > & 	foundLocations,
		double 	hitThreshold = 0,
		Size 	winStride = Size(),
		Size 	padding = Size(),
		double 	scale = 1.05,
		double 	finalThreshold = 2.0,
		bool 	useMeanshiftGrouping = false

		 */
		size_t i, j;

		/* Filtered For What??? => To Eliminate Duplication!!!*/
		for (i = 0; i < people.size(); i++) {
			Rect person = people[i]; //One Person
			for (j = 0; j < people.size(); j++){
				if (j != i && (person & people[j]) == person){
					//Founded the Same Position Shoud Be Escape!
					break;
				}
			}
			if (j == people.size()){
				//If the Same Position is not Exist, We Push this to Filtered Vector!!
				people_filtered.push_back(person);
			}
		}

		/* Writing Dectected People into Image Frame*/
		for (i = 0; i < people_filtered.size(); i++) {
			Rect person_filtered = people_filtered[i];
			person_filtered.x += cvRound(person_filtered.width * 0.1);
			person_filtered.width = cvRound(person_filtered.width * 0.8);
			person_filtered.y += cvRound(person_filtered.height * 0.06);
			person_filtered.height = cvRound(person_filtered.height * 0.9);
			rectangle(img, person_filtered.tl(), person_filtered.br(), cv::Scalar(0, 255, 0), 2);
		}

		imshow("MyVideo", img); //show the frame in "MyVideo" window
		if (waitKey(30) == 27) {
			//wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	// Wait for a keystroke in the window
	return 0;
}
