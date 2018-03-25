#ifndef GMS_FEATURE_MATCHING_H
#define GMS_FEATURE_MATCHING_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>

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

    void init(cv::Mat& im1, cv::Mat& im2, const int& N);

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
    void computeORBMatches();

    void displayMatches();

private:
  cv::Mat im1;
  cv::Mat im2;
  int N;
  std::vector<cv::DMatch> matches;
  std::vector<cv::KeyPoint> kp_1, kp_2;
};

#endif
