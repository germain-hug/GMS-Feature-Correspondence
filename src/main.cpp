#include <opencv2/core/core.hpp>

#include "../include/gms.h"

using std::string;

int main(int argc, char** argv){

	// Load both images 
	const std::string im1 = argv[1]; // Path to image 1
	const std::string im2 = argv[2]; // Path to image 2
	
	// Initialize GMS Feature Matcher and run it
	GMS gms();
	// TODO Create custom matching structure
	// std::vector<> gms.match(im1, im2);

	return 0;
}
