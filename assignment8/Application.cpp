////////////////////////////////////////////////////////////////////////////////
//
// Basic class organizing the application
//
// Authors: Stephan Richter (2011) and Patrick Lühne (2012)
//
////////////////////////////////////////////////////////////////////////////////

#include "Application.h"

#include "DepthCamera.h"
#include "DepthCameraException.h"
#include <iostream>
#include <list>
#include "DataSet.h"

////////////////////////////////////////////////////////////////////////////////
//
// Application
//
////////////////////////////////////////////////////////////////////////////////

void Application::processFrame() {
    
	using namespace cv;
	using namespace std;

	//create Baseframe
	if (firstRun) {
		m_depthImage.copyTo(m_baseFrame);
		firstRun = false;
	}
	absdiff(m_depthImage, m_baseFrame, temp);

	//convert to 8bit 
	temp.convertTo(temp, CV_8UC1);
	
	//cutoff everything closer to the camera than about 10cm above the baseimage
	threshold(temp, temp, 60, 255, THRESH_TOZERO_INV);
	//cutoff everything too far away
	threshold(temp, temp, 20, 255, 0);

	//cvtColor(m_rgbImage,temp , CV_RGB2GRAY);
	//threshold(temp, temp, 100, 255, CV_THRESH_BINARY);

	// Because the depth cam does not create an exact image, we get rid of the small noises bz eroding it
	erode(temp, temp,getStructuringElement(MORPH_RECT, Size(12,12))); 

	// To gain the original size we dilate it afterwards
	dilate(temp,temp,getStructuringElement(MORPH_RECT, Size(12,12)));

	vector<vector<Point> > contours;
	double maxContourArea = 0;
	int maxContourIndex = 0;
	int i;
	double contourArea;
	vector<Point> maxContour;
	RotatedRect fittedBox;

	//find all contours in Image and save to contours-variable
	findContours(temp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//iterate over all contours and find the biggest
	for(i = 0; i < (int)contours.size(); i++){
		//smaller contours will crash the programm
		if (contours[i].size() < 6) continue;
		
		contourArea = cv::contourArea(contours[i]);
	
		if (contourArea > maxContourArea) {
			maxContourIndex = i;
			maxContourArea = contourArea;
			maxContour = contours[i];
		}
	}

	if (maxContour.size() > 20) {
		footOnFloor = true;
		if (!recording)
			recording = true;

		fittedBox = fitEllipse(maxContour);
		//draw circle around center of fitted Ellipse
		circle(temp, fittedBox.center, 35, Scalar(100, 150, 200, 0), 2);
		//pointList.push_back(fittedBox.center);
		//cout<<pointList; 
		pointList.push_back(fittedBox.center); 
		cout << pointList.back() << "\n"; 
	}
	else
		footOnFloor = false;

	if (recording && !footOnFloor) {
		recording = false;

		// do not search vor letter, if input was just noise; 
		if(pointList.size() > 10) {
			cout << "This number was recorded for: "<<pointList.size() << " frames\n\n"; 
			cout << "The 8 points are:\n";
			for (int i=0; i < 8; i++ ) {
				points.push_back(pointList.at(int(pointList.size()*(i/8)))   );
				cout << i+1 << ": " <<points.back() << "\n";
			}
		}
		else
			cout << "this was just noise! frames: " << pointList.size() << "\n";

		//clear list to have an empty list if recording a new input
		pointList.clear();
		points.clear();
	}
	temp *= 32;
	temp.copyTo(m_outputImage);


    m_depthImage *= 32;
    
    // Linear interpolation
    y = y0 + (y1-y0)*(x-x0)/(x1-x0);

	//vector<Point> maxContour = contours.at(maxContourIndex);
	//Mat convertedContour;
	//Mat(contours[maxContourIndex]).convertTo(convertedContour, CV_32F);
	//fittedBox = fitEllipse(convertedContour);


}

////////////////////////////////////////////////////////////////////////////////

Application::Application()
{
	m_isFinished = false;
	firstRun = true; 
	footOnFloor = false; 
	recording = false;
	
	readDataSet("./pendigits.tra", 10, data, labels);
	//std::cout << data; 

	try
	{
		m_depthCamera = new DepthCamera;
	}
	catch (DepthCameraException)
	{
		m_isFinished = true;
		return;
	}

    // open windows
	cv::namedWindow("output", 1);
	cv::namedWindow("depth", 1);
	cv::namedWindow("raw", 1);

    // create work buffer
	m_rgbImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_outputImage = cv::Mat(480, 640, CV_8UC1);
	temp = cv::Mat(480, 640, CV_8UC1);

	//std::list<float> pointList(8,0);
/*	for (int i =0; i<8, i++) {
		std::cout << pointList.size() << "Elemente, "<< i << ": " << pointList[i];
	}*/

	
}

////////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
	if (m_depthCamera)
		delete m_depthCamera;
}

////////////////////////////////////////////////////////////////////////////////

void Application::loop()
{
	// Check for key input
	int key = cv::waitKey(20);

	switch (key)
	{
		case 's':
			makeScreenshots();
			break;

		case 'c':
			clearOutputImage();
			break;

		case 'q':
			m_isFinished = true;
	}

	// Grab new images from the Kinect's cameras
	m_depthCamera->frameFromCamera(m_rgbImage, m_depthImage, CV_16UC1);

	// Process the current frame
	processFrame();

	// Display the images
	cv::imshow("raw", m_rgbImage);
	cv::imshow("depth", m_depthImage);
	cv::imshow("output", m_outputImage);
}

////////////////////////////////////////////////////////////////////////////////

void Application::makeScreenshots()
{
	cv::imwrite("raw.png", m_rgbImage);
	cv::imwrite("depth.png", m_depthImage);
	cv::imwrite("output.png", m_outputImage);
}

////////////////////////////////////////////////////////////////////////////////

void Application::clearOutputImage()
{
	cv::rectangle(m_outputImage, cv::Point(0, 0), cv::Point(640, 480),
				  cv::Scalar::all(0), CV_FILLED);
}

////////////////////////////////////////////////////////////////////////////////

bool Application::isFinished()
{
	return m_isFinished;
}
