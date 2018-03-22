#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "../include/gms.h"

GMS::GMS(){}
GMS::~GMS(){}

void GMS::match(const cv::Mat& im1, const cv::Mat& im2)
{
	
}

std::vector<cv::DMatch> GMS::computeORBMatches(const cv::Mat& im1, const cv::Mat& im2)
{
	// Initiate ORB detector
	cv::Ptr<cv::FeatureDetector> detector = ORB::create();

	// find the keypoints and descriptors with ORB
	detector->detect(gray_image1, keypoints_object);
	detector->detect(gray_image2, keypoints_scene);

	cv::Ptr<cv::DescriptorExtractor> extractor = ORB::create();
	extractor->compute(im1, keypoints_object, descriptors_object);
	extractor->compute(im2, keypoints_scene, descriptors_scene);

	// Flann needs the descriptors to be of type CV_32F
	descriptors_scene.convertTo(descriptors_scene, CV_32F);
	descriptors_object.convertTo(descriptors_object, CV_32F);

	FlannBasedMatcher matcher;
	std::vector<cv::DMatch> matches;
	matcher.match(descriptors_object, descriptors_scene, matches);
	return matches;
}