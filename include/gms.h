#ifndef GMS_FEATURE_MATCHING_H
#define GMS_FEATURE_MATCHING_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <array>

#define N 20

struct cellMatch
{
  int idx1; // index of keypoint in image 1
  int idx2; // index of keypoint in image 2
  int src; // source cell
  int dst; // destination cell
};

// cellMatches: 2D Array of cellMatches
typedef std::array<std::vector<cellMatch>, N*N> cellMatches;

// cellBins: Count of destination cell matches
typedef std::array<std::array<int, N*N>,  N*N> cellBins;


using std::string;

/**
 *  GMS Feature Matching Class
 */
class GMS
{
public:

    /**
    * @name Constructor
    * @brief Initialize GMS Feature Matcher
    */
    GMS();

    ~GMS();

    void init(cv::Mat& im1, cv::Mat& im2);

    /**
    * @name match
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @param[in] N Number of features
    * @brief Apply GMS Feature Matching on both images
    */
    void match();

    /**
    * @name computeORBMatches
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @param[in] N Number of features
    * @brief Compute ORB Features and correspondence
    */
    void computeORBMatches(std::vector<cv::DMatch>& matches,
    	 std::vector<cv::KeyPoint>& kp_1,
    	 std::vector<cv::KeyPoint>& kp_2);

    void assignMatchesToCells(const std::vector<cv::DMatch>& matches,
    	const std::vector<cv::KeyPoint>& kp_1,
    	const std::vector<cv::KeyPoint>& kp_2);

    void displayMatches(const std::vector<cv::DMatch>& matches,
    	const std::vector<cv::KeyPoint>& kp_1,
    	const std::vector<cv::KeyPoint>& kp_2);

private:
  cv::Mat _im1;
  cv::Mat _im2;
  int _w_1, _w_2, _h_1, _h_2;
};

#endif
