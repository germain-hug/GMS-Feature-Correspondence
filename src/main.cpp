#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "../include/gms.h"

using std::string;

int main(int argc, char** argv){

	if (argc < 3)
	{
			std::cerr << "Usage: " << argv[0] << " <path to image 1> <path to image 2>" << std::endl;
			return 1;
	}

	// Load both images
	cv::Mat im1 = cv::imread(argv[1]);
	cv::Mat im2 = cv::imread(argv[2]);

	// Initialize GMS Feature Matcher and run it
	GMS gms;
	gms.init(im1, im2);
	gms.run();
	return 0;
}
