#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iostream>
#include <iomanip>

#include "../include/gms.h"

GMS::GMS(){}
GMS::~GMS(){}

void GMS::init(cv::Mat& im1, cv::Mat& im2)
{
	_im1 = &im1;
	_im2 = &im2;
}

void GMS::run()
{
	// Compute ORB Feature Descriptors and Matches
	double start = std::clock();
  std::vector<cv::DMatch> matches;
  std::vector<cv::KeyPoint> kp_1, kp_2;
	computeORBMatches(matches, kp_1, kp_2);
	double orbComputeTime = (std::clock() - start)/(double)CLOCKS_PER_SEC;

	// Assign feature points to their corresponding cells
	start = std::clock();
	std::array<cellMatches, 4> cell_matches;
	std::array<cellBins, 4> cell_bins{};
	assignMatchesToCells(matches, kp_1, kp_2, cell_matches, cell_bins);

	// Threshold candidates on highest matching pairs
	std::vector<cv::DMatch> new_matches;
	filterMatches(kp_1, kp_2, matches, cell_matches, cell_bins, new_matches);
	double gmsComputeTime = (std::clock() - start)/(double)CLOCKS_PER_SEC;

	// Print results
	std::cout << "+--------------------------------+" << std::endl;
	std::cout << "| " << std::left << std::setw(22) << "Grid size: " << N << std::endl;
	std::cout << "| " << std::left << std::setw(22) << "Feature Points (ORB): " << matches.size() << std::endl;
	std::cout << "| " << std::left << std::setw(22) << "Feature Points (GMS): " << new_matches.size() << std::endl;
	std::cout << "| " << std::left << std::setw(22) << "ORB Runtime: " << orbComputeTime << std::endl;
	std::cout << "| " << std::left << std::setw(22) << "GMS Runtime: " << gmsComputeTime << std::endl;
	std::cout << "+--------------------------------+" << std::endl;

	// Display matches
	displayMatches(new_matches, kp_1, kp_2);
}


void GMS::computeORBMatches(std::vector<cv::DMatch>& matches,
	 std::vector<cv::KeyPoint>& kp_1,
	 std::vector<cv::KeyPoint>& kp_2)
{
	// Initialize ORB Feature Descriptor
	cv::ORB orb(400, 1.2f, 4, 31, 0, 2, 1, 31);
  // Descriptors : Image 1
	cv::Mat d1, d2;
	orb.detect(*_im1, kp_1);
	orb.compute(*_im1, kp_1, d1);
	// Descriptors : Image 2
	orb.detect(*_im2, kp_2);
	orb.compute(*_im2, kp_2, d2);
	// Brute-Force matching
	cv::BFMatcher matcher(cv::NORM_HAMMING);
	matcher.match(d1, d2, matches);
}


void GMS::assignMatchesToCells(const std::vector<cv::DMatch>& m,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	std::array<cellMatches, 4>& cell_matches,
	std::array<cellBins, 4>& cell_bins)
{
	float off_x, off_y;
	int gIdx_1, gIdx_2, kIdx_1, kIdx_2;

	// Every grid offset
	for(int k = 0; k < 4; k++)
	{
			computeOffset(k, off_x, off_y);
			for(int i = 0; i < m.size(); i++)
			{
				// Keypoint indices
				kIdx_1 = m[i].queryIdx;
				kIdx_2 = m[i].trainIdx;

				// Source and destination cells
				gIdx_1 = getGridIdxFromPoint(kp_1[kIdx_1].pt, off_x, off_y, _im1->cols / N, _im1->rows / N);
				gIdx_2 = getGridIdxFromPoint(kp_2[kIdx_2].pt, off_x, off_y, _im2->cols / N, _im2->rows / N);

				// Instantiate cellMatch object and add to grid
				cell_matches[k][gIdx_1].push_back({gIdx_1, gIdx_2, kp_1[kIdx_1], kp_2[kIdx_2], m[i]});

				// Increment match count
				cell_bins[k][gIdx_1][gIdx_2]++;
		}
	}
}

void GMS::filterMatches(const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	const std::vector<cv::DMatch>& matches,
	const std::array<cellMatches,4>& cell_matches,
	const std::array<cellBins,4>& cell_bins,
	std::vector<cv::DMatch>& new_matches)
{

	// For cell every offset
	float off_x, off_y;
	for(int k = 0; k < 4; k++)
	{
		computeOffset(k, off_x, off_y);
		std::vector<std::array<int, 3>> intermediate_matches;

		// Find best cell match (Forward-pass)
		for(int i = 0; i < N * N; i++)
		{
			int s = 0, dst_idx = 0;
			for(int j = 0; j < N * N; j++)
			{
				if(cell_bins[k][i][j] > s)
				{
					dst_idx = j;
					s = cell_bins[k][i][j];
				}
			}
			if(s) intermediate_matches.push_back({i, dst_idx, s});
		}

		// Find best cell match (Backward-pass)
		for(int i = 0; i < N*N; i++)
		{
			int s = 0, dst_idx = 0;
			for(auto& im : intermediate_matches)
			{
				if(im[1]==i and im[2] > s)
				{
					s = im[2];
					dst_idx = im[0];
 				}
			}

			// Inlier thresholding (neighbour-based motion smoothness)
			int valid_features = 0, total_features = 0, offset;
			bool valid_offset;

			for(int l = 0; l < 9; l++)
			{
				offset = _neighbour_x[l] + _neighbour_y[l];
				valid_offset = i + offset > 0
					&& i + offset < N*N
					&& dst_idx + offset > 0
					&& dst_idx + offset < N*N;

				if(valid_offset)
				{
					valid_features += cell_bins[k][dst_idx + offset][i + offset];
					for(const auto& n : cell_bins[k][dst_idx + offset]) total_features += n;
				}
			}

			// Merge inliers for the best pairs
			bool valid_cell = double(valid_features) / double(total_features) > _thresh && valid_features > 5;
			if(s && valid_cell) computeInliers(kp_1, kp_2, cell_matches[k][dst_idx], i, new_matches);
		}
	}
}

void GMS::computeInliers(const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2,
	const std::vector<cellMatch>& cell_matches,
	const int& dst_idx,
	std::vector<cv::DMatch>& new_matches)
{
	for(const auto& c: cell_matches)
		if(c.dst == dst_idx)
			new_matches.push_back(c.m);
}


int GMS::getGridIdxFromPoint(const cv::Point& pt,
	const int& off_x,
	const int& off_y,
	const int& dw,
	const int& dh)
{
	 int idx_x = int(pt.x + off_x * dw) / dw;
	 int idx_y = int(pt.y + off_y * dh) / dh;
 	 return idx_y * N + idx_x;
}


void GMS::displayMatches(const std::vector<cv::DMatch>& m,
	const std::vector<cv::KeyPoint>& kp_1,
	const std::vector<cv::KeyPoint>& kp_2)
{
	cv::Mat result;
	std::vector<char> mask(m.size(), 1);
	cv::drawMatches(*_im1, kp_1, *_im2, kp_2, m, result, cv::Scalar::all(255), cv::Scalar::all(255), mask, 2);
	cv::imshow("Matches", result);
	cv::waitKey(0);
}
