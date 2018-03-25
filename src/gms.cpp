#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include "../include/gms.h"

GMS::GMS(){}
GMS::~GMS(){}

void GMS::init(cv::Mat& im1, cv::Mat& im2)
{
	// Assign images attributes
	_im1 = im1;
	_im2 = im2;
	_w_1 = _im1.rows;
	_h_1 = _im1.cols;
	_w_2 = _im2.rows;
	_h_2 = _im2.cols;
}

void GMS::match()
{
	// Compute ORB Matches
  std::vector<cv::DMatch> matches;
  std::vector<cv::KeyPoint> kp_1, kp_2;
	computeORBMatches(matches, kp_1, kp_2);

	// Assign every match to its pair of cells
	assignMatchesToCells(matches, kp_1, kp_2);

	// Begin matching
	const int grid_w_1 = _w_1 / N;
	const int grid_h_1 = _h_1 / N;
	const int grid_w_2 = _w_2 / N;
	const int grid_h_2 = _h_2 / N;

	// For cell every offset
	float off_x, off_y;
	for(int k = 0; k < 4; k++)
	{
		off_x = (k==0 || k==1) ? 0.0 : 0.5;
		off_y = (k==0 || k==2) ? 0.0 : 0.5;

		// For every pair of cells in the grid
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				// Find best pair of cells

				// For every match in the cell

			}
		}
	}

	// Display matches
	displayMatches(matches, kp_1, kp_2);
}


void GMS::computeORBMatches(std::vector<cv::DMatch>& matches,
	 std::vector<cv::KeyPoint>& kp_1,
	 std::vector<cv::KeyPoint>& kp_2)
{
	// Initialize ORB Feature Descriptor
  cv::Ptr<cv::ORB> detector = cv::ORB::create("ORB");
  cv::Ptr<cv::ORB> extractor = cv::ORB::create("ORB");

  // Descriptors : Image 1
	cv::Mat descriptors_1, descriptors_2;
	detector->detect(_im1, kp_1);
	extractor->compute(_im1, kp_1, descriptors_1);

	// Descriptors : Image 2
	detector->detect(_im2, kp_2);
	extractor->compute(_im2, kp_2, descriptors_2);

	// Brute-Force matching
	cv::BFMatcher matcher;
	matcher.match(descriptors_1, descriptors_2, matches);
}

void GMS::assignMatchesToCells(const std::vector<cv::DMatch>& matches,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2)
{
	// TODO : Deal with offsets !!!!
	cellMatches cell_matches;
	cellBins cell_bins;
	float off_x, off_y;

	// For every feature point match
	for(int i = 0; i < matches.size(); i++)
	{
		// For every grid offset
		for(int k = 0; k < 4; k++)
		{
			off_x = (k==0 || k==1)? 0.0 : 0.5;
			off_y = (k==0 || k==2)? 0.0 : 0.5;

			// Find keypoint cell location in both images
			cv::Point pt_1 = kp_1[matches[i].queryIdx].pt;
			cv::Point pt_2 = kp_2[matches[i].trainIdx].pt;	// Offsets ???
			int grid_idx_1 = getGridIdxFromPoint(pt_1);
			int grid_idx_2 = getGridIdxFromPoint(pt_2);

			// Instantiate cellMatch object and add to grid
			cellMatch c
			{
				matches[i].queryIdx,
				matches[i].trainIdx,
				grid_idx_1,
				grid_idx_2
			};
			cell_matches[grid_idx_1].push_back(c);

			// Increment match count
			cell_bins[grid_idx_1][grid_idx_2]++;
		}
	}

}

void GMS::displayMatches(const std::vector<cv::DMatch>& m,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2)
{
	cv::Mat disp;
	std::vector<char> mask(m.size(), 1);
	cv::drawMatches(_im1, kp_1, _im2, kp_2, m,
		disp, cv::Scalar::all(255), cv::Scalar::all(255), mask, 0);
	cv::imshow("Matches", disp);
	cv::waitKey(0);
}
