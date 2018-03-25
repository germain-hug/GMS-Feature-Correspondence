#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include "../include/gms.h"

GMS::GMS(){}
GMS::~GMS(){}

void GMS::init(cv::Mat& _im1, cv::Mat& _im2, const int& _N)
{
	im1 = _im1;
	im2 = _im2;
	N =_N;
}

void GMS::match()
{
	// Compute ORB Matches
	computeORBMatches();

	// Compute Grid coordinates
	computeGrid();

	
	// 1 - Compute Grid
	// 2 - double for loops on grid
	// 3 - assign best cell
	// 3 - divide matches for this cell

	// cv::imshow("Left", im1);
	// cv::waitKey(0);
}


void GMS::computeORBMatches()
{
	// Initialize ORB Feature Descriptor
  cv::Ptr<cv::ORB> detector = cv::ORB::create("ORB");
  cv::Ptr<cv::ORB> extractor = cv::ORB::create("ORB");

  // Descriptors : Image 1
	cv::Mat descriptors_1, descriptors_2;
	detector->detect(im1, kp_1);
	extractor->compute(im1, kp_1, descriptors_1);

	// Descriptors : Image 2
	detector->detect(im2, kp_2);
	extractor->compute(im2, kp_2, descriptors_2);

	// Brute-Force matching
	cv::BFMatcher matcher;
	matcher.match(descriptors_1, descriptors_2, matches);
}

void GMS::displayMatches()
{
	cv::Mat disp;
	std::vector<char> mask(matches.size(), 1);
	cv::drawMatches(im1, kp_1, im2, kp_2, matches,
		disp, cv::Scalar::all(255), cv::Scalar::all(255), mask, 0);
	cv::imshow("Matches", disp);
	cv::waitKey(0);
}
