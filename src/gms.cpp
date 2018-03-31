#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include "../include/gms.h"

GMS::GMS(){}

GMS::~GMS()
{
	delete _im1;
	delete _im2;
}

// TODO : Smart pointers ??
// TODO : Check ORB params
// TODO : Cell rotations

void GMS::init(cv::Mat& im1, cv::Mat& im2)
{
	_im1 = &im1;
	_im2 = &im2;
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
	filterMatches(kp_1, kp_2, matches, cell_matches, cell_bins, new_matches, new_kp_1, new_kp_2);

	// Display matches
	displayMatches(matches, kp_1, kp_2);
	displayMatches(new_matches, kp_1, kp_2);
}


void GMS::computeORBMatches(std::vector<cv::DMatch>& matches,
	 std::vector<cv::KeyPoint>& kp_1,
	 std::vector<cv::KeyPoint>& kp_2)
{
	// Initialize ORB Feature Descriptor
	cv::Mat dsc_1, dsc_2;
	cv::OrbFeatureDetector FeatureDesc(20000, 1.4f, 8, 64, 0, 2, 1, 64);
	cv::ORB orb;
  // Descriptors : Image 1
	orb.detect(*_im1, kp_1);
	orb.compute(*_im1, kp_1, dsc_1);
	// Descriptors : Image 2
	orb.detect(*_im2, kp_2);
	orb.compute(*_im2, kp_2, dsc_2);
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
	int G_idx_1, G_idx_2;

	for(int k = 0; k < 4; k++)
	{
			computeOffset(k, off_x, off_y);
			for(int i = 0; i < matches.size(); i++)
			{
				// Keypoint index
				int idx_1 = matches[i].queryIdx;
				int idx_2 = matches[i].trainIdx;
				// Source cell
				G_idx_1 = getGridIdxFromPoint(kp_1[idx_1].pt, off_x, off_y, _im1->cols / N, _im1->rows / N);
				// Destination cell
				G_idx_2 = getGridIdxFromPoint(kp_2[idx_2].pt, off_x, off_y, _im2->cols / N, _im2->rows / N);
				// Instantiate cellMatch object and add to grid
				cell_matches[k][G_idx_1].push_back({G_idx_1, G_idx_2, kp_1[i], kp_2[i], matches[i]});
				// Increment match count
				cell_bins[k][G_idx_1][G_idx_2]++;
		}
	}
}

void GMS::filterMatches(const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	const std::vector<cv::DMatch>& matches,
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
		std::vector<std::array<int, 3>> intermediate_matches;

		// TODO deal with borders
		for(int i = 0; i < N * N; i++)
		{
			// Find best destination cell match
			int s = 0, best_dest_idx = 0;
			for(int j = 1; j < N * N - N; j++)
			{
				if(cell_bins[k][i][j] > s)
				{
					best_dest_idx = j;
					s = cell_bins[k][i][j];
				}
			}

			int valid_features = 0, total_features = 0, offset;
			for(int l = 0; l < 9; l++)
			{
				offset = _neighbour_x[l] + _neighbour_y[l];
				if(i + offset > 0 && i + offset < N*N && best_dest_idx + offset > 0 && best_dest_idx + offset < N*N)
				{
					valid_features += cell_bins[k][i + offset][best_dest_idx + offset];
					for(const auto& n : cell_bins[k][i + offset]) total_features += n;
				}
			}
			// Merge inliers for the best pairs
			bool valid_cell = double(valid_features) / double(total_features) > _thresh;
			if(s and valid_cell) intermediate_matches.push_back({i, best_dest_idx, s});
		}


		// BACKWARD PASS
		for(int i = 0; i < N*N; i++)
		{
			int s = 0, best_dest_idx = 0;
			for(auto& im : intermediate_matches)
			{
				if(im[1]==i and im[2] > s)
				{
					s = im[2];
					best_dest_idx = im[0];
 				}
			}
			if(s) computeInliers(kp_1, kp_2, matches, cell_matches[k][best_dest_idx], i, new_matches, new_kp_1, new_kp_2);
		}
	}
}

void GMS::computeInliers(const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	const std::vector<cv::DMatch>& matches,
	const std::vector<cellMatch>& cell_matches,
	const int& best_dest_idx,
	std::vector<cv::DMatch>& new_matches,
	std::vector<cv::KeyPoint>& new_kp_1,
	std::vector<cv::KeyPoint>& new_kp_2)
{
	// Filter out keypoints from cell matches
	for(auto& c: cell_matches)
	{
		if(c.dst == best_dest_idx)
		{
			new_kp_1.push_back(c.kp_1);
			new_kp_2.push_back(c.kp_2);
			new_matches.push_back(c.m);
		}
	}
}


int GMS::getGridIdxFromPoint(const cv::Point& pt, const int& off_x, const int& off_y, const int& dw, const int& dh)
{
	 int idx_x = int(pt.x + off_x * dw) / dw;
	 int idx_y = int(pt.y + off_y * dh) / dh;
 	 return idx_y * N + idx_x;
}


void GMS::displayMatches(const std::vector<cv::DMatch>& m,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2)
{
	cv::Mat disp;
	std::vector<char> mask(m.size(), 1);
	cv::drawMatches(*_im1, kp_1, *_im2, kp_2, m, disp, cv::Scalar::all(255), cv::Scalar::all(255), mask, 0);
	cv::imshow("Matches", disp);
	cv::waitKey(0);
}
