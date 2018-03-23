#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "../include/gms.h"

GMS::GMS(){}
GMS::~GMS(){}

void GMS::match(const cv::Mat& im1_in, const cv::Mat& im2_in)
{
	// Pre-process images
	cv::Mat im1, im2;
	pre_process(im1_in, im1);
	pre_process(im2_in, im2);

	// Compute ORB Matches
	computeORBMatches(im1, im2);

}

void GMS::pre_process(const cv::Mat& src, cv::Mat& dst)
{

}

std::vector<cv::DMatch> GMS::computeORBMatches(const cv::Mat& im1, const cv::Mat& im2)
{
	// Initialize ORB Feature Descriptor
	std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
    cv::Ptr<cv::ORB> detector = cv::ORB::create(5000);
    cv::Ptr<cv::ORB> extractor = cv::ORB::create();
    //
	cv::Mat descriptors_1, descriptors_2;
    detector->detect(im1, keypoints_1);
    extractor->compute(im1, keypoints_1, descriptors_1);
	detector->detect(im2, keypoints_2);
	extractor->compute(im2, keypoints_2, descriptors_2);
    // drawKeypoints(im1, h20fall_keypoints, im1);

	cv::BFMatcher matcher;
	std::vector<cv::DMatch> matches;
	matcher.match(descriptors_1, descriptors_2, matches);
	return matches;
}


