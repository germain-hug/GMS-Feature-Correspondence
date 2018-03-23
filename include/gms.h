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

    /**
    * @name match 
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @param[in] N Number of features
    * @brief Apply GMS Feature Matching on both images
    */
    void match(const cv::Mat& im1, const cv::Mat& im2, const int& N=5000);

    /**
    * @name pre_process 
    * @param[in] src Source image
    * @param[in] dst Destination image
    * @brief Pre-process input images
    */
    void pre_process(const cv::Mat& src, cv::Mat& dst);
    
    /**
    * @name computeORBMatches
    * @param[in] im1 First image
    * @param[in] im2 Second image
    * @param[in] N Number of features
    * @brief Compute ORB Features and correspondence
    */
    std::vector<cv::DMatch> computeORBMatches(const cv::Mat& im1, const cv::Mat& im2, const int& N);

private:

};

#endif