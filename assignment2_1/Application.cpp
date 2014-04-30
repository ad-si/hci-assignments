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

////////////////////////////////////////////////////////////////////////////////
//
// Application
//
////////////////////////////////////////////////////////////////////////////////

void Application::processFrame()
{
	///////////////////////////////////////////////////////////////////////////
	//
	// To do:
	//
	// This method will be called every frame of the camera. Insert code here in
	// order to fulfill the assignment. These images will help you doing so:
	//
	// * m_rgbImage: The image of the Kinect's RGB camera
	// * m_depthImage: The image of the Kinects's depth sensor
	// * m_outputImage: The image in which you can draw the touch circles.
	//
	///////////////////////////////////////////////////////////////////////////



	// Sample code brightening up the depth image to make it visible

	

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
	//cout << "MaxContourArea: "<< maxContourArea << '\n';
	//cout << "MaxContourSize: " << (int)contours.size() << '\n';
	//cout << "MaxContourIndex: " << maxContourIndex << '\n';
	

	//CHECK ONCE AGAIN BECAUSE IT DOES NOT MAKE SENSE BUT IF WE DO NOT THE WHOLE PROGRAMM CRASHES WITHOUT A DEBUGGING NOTICE, THIS TOOK US AT LEAST 4 HOURS
	//!!!!!!!!!!!!!!!!!!!!!!!!11!!!!!!!!!!!!!!!!111!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11!!EINSELF!!!!!
	if (maxContour.size() > 5) {
		fittedBox = fitEllipse(maxContour);
		//draw circle around center of fitted Ellipse
		circle(temp, fittedBox.center, 35, Scalar(100, 150, 200, 0), 2);
	}	
	temp *=32;
	temp.copyTo(m_outputImage);


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
	//temp = cv::Mat(480, 640, CV_8UC1);



	
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
