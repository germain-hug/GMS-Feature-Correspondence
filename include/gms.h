#ifndef GMS_FEATURE_MATCHING_H
#define GMS_FEATURE_MATCHING_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <array>

#define N 10

using std::string;

/** Keypoint cell matching description */
struct cellMatch
{
  /** Index of keypoint in image 1 */
  int idx1;
  /** Index of keypoint in image 2 */
  int idx2;
  /** Source cell index */
  int src;
  /** Destination cell index */
  int dst;
};

/** cellMatches: 2D Array of cellMatches */
typedef std::array<std::vector<cellMatch>, N*N> cellMatches;

/** cellBins: Count of destination cell matches */
typedef std::array<std::array<int, N*N>,  N*N> cellBins;

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


    /**
    * @name init
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @brief Initialize GMS Feature Matching on images
    */
    void init(cv::Mat& im1, cv::Mat& im2);

    /**
    * @name run
    * @brief Run GMS Feature Matching on initialized images
    */
    void run();

    /**
    * @name computeORBMatches
    * @param[in] matches Vector of matches (to be filled)
    * @param[in] kp_1 Keypoints in image 1 (to be filled)
    * @param[in] kp_2 Keypoints in image 2 (to be filled)
    * @brief Compute ORB Features and correspondence on initialized images
    */
    void computeORBMatches(std::vector<cv::DMatch>& matches,
    	 std::vector<cv::KeyPoint>& kp_1,
    	 std::vector<cv::KeyPoint>& kp_2);

    /**
    * @name assignMatchesToCells
    * @param[in] matches Vector of matches
    * @param[in] kp_1 Keypoints in image 1
    * @param[in] kp_2 Keypoints in image 2
    * @brief Assign every pair of keypoints to corresponding pair of cells
    */
    void assignMatchesToCells(const std::vector<cv::DMatch>& matches,
    	const std::vector<cv::KeyPoint>& kp_1,
    	const std::vector<cv::KeyPoint>& kp_2,
    	std::array<cellMatches, 4>& cell_matches,
    	std::array<cellBins, 4>& cell_bins);

    int getGridIdxFromPoint(const cv::Point& pt, const int& off_x, const int& off_y, const int& dw, const int& dh);

    /**
    * @name displayMatches
    * @param[in] matches Vector of matches
    * @param[in] kp_1 Keypoints in image 1
    * @param[in] kp_2 Keypoints in image 2
    * @brief Display resulting feature matching
    */
    void displayMatches(const std::vector<cv::DMatch>& matches,
    	const std::vector<cv::KeyPoint>& kp_1,
    	const std::vector<cv::KeyPoint>& kp_2);

    /**
    * @name computeOffset
    * @param[in] k Grid offset index
    * @param[in] off_x x offset to be assigned
    * @param[in] off_y y offset to be assigned
    * @brief Compute x and y offset values
    */
    void computeOffset(const int& k, float& off_x, float& off_y)
    {
      off_x = (k==0 || k==1)? 0.0 : 0.5;
      off_y = (k==0 || k==2)? 0.0 : 0.5;
    };

private:
  cv::Mat _im1;
  cv::Mat _im2;
  int _w_1, _w_2, _h_1, _h_2;
};

#endif
