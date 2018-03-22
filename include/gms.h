#ifndef GMS_FEATURE_MATCHING_H
#define GMS_FEATURE_MATCHING_H

#include <opencv2/core/core.hpp>

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

    /**
    * @name match 
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @brief Apply GMS Feature Matching on both images
    */
    void match(const cv::Mat& im1, const cv::Mat& im2);
    
    /**
    * @name computeORBMatches
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @brief Compute ORB Features and correspondence
    */
    std::vector<cv::DMatch> GMS::computeORBMatches(const cv::Mat& im1, const cv::Mat& im2);

private:
    
};

#endif