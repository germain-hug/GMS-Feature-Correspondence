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
	_w_1 = _im1.cols;
	_h_1 = _im1.rows;
	_w_2 = _im2.cols;
	_h_2 = _im2.rows;
}


void GMS::run()
{
	// Compute ORB Feature Descriptors and Matches
  std::vector<cv::DMatch> matches;
  std::vector<cv::KeyPoint> kp_1, kp_2;
	computeORBMatches(matches, kp_1, kp_2);

	// Assign feature points to their corresponding cells
	std::array<cellMatches, 4> cell_matches;
	std::array<cellBins, 4> cell_bins{};
	assignMatchesToCells(matches, kp_1, kp_2, cell_matches, cell_bins);

	// Threshold candidates on highest matching pairs
	std::vector<cv::DMatch> new_matches;
	std::vector<cv::KeyPoint> new_kp_1, new_kp_2;
	filterMatches(kp_1, kp_2, cell_matches, cell_bins, new_matches, new_kp_1, new_kp_2);

	// Display matches
	displayMatches(matches, kp_1, kp_2);
	displayMatches(new_matches, new_kp_1, new_kp_2);
}


void GMS::computeORBMatches(std::vector<cv::DMatch>& matches,
	 std::vector<cv::KeyPoint>& kp_1,
	 std::vector<cv::KeyPoint>& kp_2)
{
	// Initialize ORB Feature Descriptor
	cv::Mat dsc_1, dsc_2;
	cv::OrbFeatureDetector FeatureDesc(5000, 1.2f, 8, 31, 0, 2, 1, 31);
	cv::ORB orb;

  // Descriptors : Image 1
	orb.detect(_im1, kp_1);
	orb.compute(_im1, kp_1, dsc_1);
	// Descriptors : Image 2
	orb.detect(_im2, kp_2);
	orb.compute(_im2, kp_2, dsc_2);

	// Brute-Force matching
	cv::BFMatcher matcher;
	matcher.match(dsc_1, dsc_2, matches);
	std::cout <<"OK" << std::endl;
}


void GMS::assignMatchesToCells(const std::vector<cv::DMatch>& matches,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	std::array<cellMatches, 4>& cell_matches,
	std::array<cellBins, 4>& cell_bins)
{
	// For every grid offset
	float off_x, off_y;
	cv::Point pt_1, pt_2;
	int grid_idx_1, grid_idx_2;

	for(int k = 0; k < 4; k++)
	{
			computeOffset(k, off_x, off_y);
			for(int i = 0; i < matches.size(); i++)
			{
				// Find keypoint corresponding cell coordinates
				pt_1 = kp_1[matches[i].queryIdx].pt;
				pt_2 = kp_2[matches[i].trainIdx].pt;	// Offsets ???
				grid_idx_1 = getGridIdxFromPoint(pt_1, off_x, off_y, _w_1 / N, _h_1 / N);
				grid_idx_2 = getGridIdxFromPoint(pt_2, off_x, off_y, _w_2 / N, _h_2 / N);

				// Instantiate cellMatch object and add to grid
				cellMatch c {matches[i].queryIdx, matches[i].trainIdx, grid_idx_1, grid_idx_2};
				cell_matches[k][grid_idx_1].push_back(c);

				// Increment match count
				cell_bins[k][grid_idx_1][grid_idx_2]++;
		}
	}
}

void GMS::filterMatches(const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	const std::array<cellMatches,4>& cell_matches,
	const std::array<cellBins,4>& cell_bins,
	std::vector<cv::DMatch>& new_matches,
	std::vector<cv::KeyPoint>& new_kp_1,
	std::vector<cv::KeyPoint>& new_kp_2)
{

	// For cell every offset
	float off_x, off_y;
	for(int k = 0; k < 4; k++)
	{
		computeOffset(k, off_x, off_y);
		for(int i = 1; i < N * N - N; i++) // TODO deal with borders
		{
			// Find the best cell match
			int s = 0, best_dest_idx = 0, pair_size;
			for(int j = 1; j < N * N - N; j++) // TODO deal with borders
			{
				pair_size = cell_bins[k][i][j];
				if(pair_size > s)
				{
					best_dest_idx = j;
					s = pair_size;
				}
			}

			// TODO Check for neighbours if neighbours have sufficient matches before adding current one (smoothness)
			int valid_features = 0, total_features = 0, offset;
			for(int l = 0; l < 9; l++)
			{
				offset = _neighbour_x[l] + _neighbour_y[l];
				valid_features += cell_bins[k][i + offset][best_dest_idx + offset];
				for(auto& n : cell_bins[k][i + offset]) total_features += n;
			}
			// Merge inliers for the best pairs
			bool valid_cell = double(valid_features) / double(total_features) > _thresh;
			if(s and valid_cell) computeInliers(kp_1, kp_2, cell_matches[k][i], best_dest_idx, new_matches, new_kp_1, new_kp_2);
		}
	}
}

void GMS::computeInliers(const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	const std::vector<cellMatch>& cell_matches,
	const int& best_dest_idx,
	std::vector<cv::DMatch>& new_matches,
	std::vector<cv::KeyPoint>& new_kp_1,
	std::vector<cv::KeyPoint>& new_kp_2)
{
	// Filter out cell matches
	for(auto& c: cell_matches)
	{
		if(c.dst == best_dest_idx)
		{
			// Construct DMatch object
			new_kp_1.push_back(kp_1[c.idx1]);
			new_kp_2.push_back(kp_2[c.idx2]);
			// TODO ??  idx2 then idx1 or other way around ??
			new_matches.push_back(cv::DMatch(c.idx1, c.idx2, 1.0));
		}
	}
}


int GMS::getGridIdxFromPoint(const cv::Point& pt, const int& off_x, const int& off_y, const int& dw, const int& dh)
{
	 int idx_x = int(pt.x + off_x * dw) / dw; // Offset impact ??
	 int idx_y = int(pt.y + off_y * dh) / dh;
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
