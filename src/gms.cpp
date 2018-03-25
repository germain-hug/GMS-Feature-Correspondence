#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include "../include/gms.h"

GMS::GMS(){}
GMS::~GMS(){}

// TODO : Init with pointers !!
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


void GMS::run()
{
	// Compute ORB Feature Descriptors and Matches
  std::vector<cv::DMatch> matches;
  std::vector<cv::KeyPoint> kp_1, kp_2;
	computeORBMatches(matches, kp_1, kp_2);

	// Assign matches to corresponding cells
	std::array<cellMatches, 4> cell_matches;
	std::array<cellBins, 4> cell_bins{};
	assignMatchesToCells(matches, kp_1, kp_2, cell_matches, cell_bins);

	// For cell every offset
	float off_x, off_y;
	for(int k = 0; k < 4; k++)
	{
		computeOffset(k, off_x, off_y);
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
				// Find best pair of cells
				//std::cout << cell_matches[k][j].size() << std::endl;
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
	cv::Mat dsc_1, dsc_2;
  cv::Ptr<cv::ORB> detector  = cv::ORB::create("ORB");
  cv::Ptr<cv::ORB> extractor = cv::ORB::create("ORB");

  // Descriptors : Image 1
	detector->detect(_im1, kp_1);
	extractor->compute(_im1, kp_1, dsc_1);

	// Descriptors : Image 2
	detector->detect(_im2, kp_2);
	extractor->compute(_im2, kp_2, dsc_2);

	// Brute-Force matching
	cv::BFMatcher matcher;
	matcher.match(dsc_1, dsc_2, matches);
}


void GMS::assignMatchesToCells(const std::vector<cv::DMatch>& matches,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	std::array<cellMatches, 4>& cell_matches,
	std::array<cellBins, 4>& cell_bins)
{
	// For every grid offset
	float off_x, off_y;
	for(int k = 0; k < 4; k++)
	{
			computeOffset(k, off_x, off_y);
			for(int i = 0; i < matches.size(); i++)
			{
				// Find keypoint corresponding cell coordinates
				cv::Point pt_1 = kp_1[matches[i].queryIdx].pt;
				cv::Point pt_2 = kp_2[matches[i].trainIdx].pt;	// Offsets ???
				int grid_idx_1 = getGridIdxFromPoint(pt_1, off_x, off_y, _w_1/N, _h_1/N);
				int grid_idx_2 = getGridIdxFromPoint(pt_2, off_x, off_y, _w_2/N, _h_2/N);

				std::cout << grid_idx_1 << " " << grid_idx_2 << std::endl;

				// Instantiate cellMatch object and add to grid
				cellMatch c {matches[i].queryIdx, matches[i].trainIdx, grid_idx_1, grid_idx_2};
				cell_matches[k][grid_idx_1].push_back(c);

				// Increment match count
				cell_bins[k][grid_idx_1][grid_idx_2]++;
		}
	}
}


int GMS::getGridIdxFromPoint(const cv::Point& pt, const int& off_x, const int& off_y, const int& dw, const int& dh)
{
	 int idx_x = int( (pt.x + off_x * dw) % dw);
	 int idx_y = int( (pt.y + off_y * dh) % dh);
	 return idx_y * N + idx_y;
}


void GMS::displayMatches(const std::vector<cv::DMatch>& m,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2)
{
	cv::Mat disp;
	std::vector<char> mask(m.size(), 1);
	cv::drawMatches(_im1, kp_1, _im2, kp_2, m, disp, cv::Scalar::all(255), cv::Scalar::all(255), mask, 0);
	cv::imshow("Matches", disp);
	cv::waitKey(0);
}
