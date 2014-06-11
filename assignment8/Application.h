////////////////////////////////////////////////////////////////////////////////
//
// Basic class organizing the application
//
// Authors: Stephan Richter (2011) and Patrick Lühne (2012)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __APPLICATION_H
#define __APPLICATION_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <list>

// Forward declarations
class DepthCamera;

////////////////////////////////////////////////////////////////////////////////
//
// Application
//
////////////////////////////////////////////////////////////////////////////////

class Application
{
	public:
		Application();
		~Application();

		void loop();

		////////////////////////////////////////////////////////////////////////
		//
		// To do: Insert your code here
		//
		////////////////////////////////////////////////////////////////////////
		void processFrame();

		void makeScreenshots();
		void clearOutputImage();

		bool isFinished();

	protected:
		DepthCamera *m_depthCamera;

		cv::Mat m_rgbImage;
		cv::Mat m_depthImage;
		cv::Mat m_outputImage;
		cv::Mat m_baseFrame;
		cv::Mat temp;


		bool m_isFinished;
		bool firstRun;
		bool footOnFloor;
		bool recording;
		cv::Mat data;
		cv::Mat labels;
		std::vector<cv::Point> pointList;
		std::vector<cv::Point> points;


};

#endif // __APPLICATION_H
