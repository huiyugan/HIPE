
#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "diffteo.h"

using namespace cv;
using namespace cv::xfeatures2d;

Mat extractBackground(cv::Mat img, cv::Mat & back)
{
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);
	std::vector<KeyPoint> keypoints_object, keypoints_back;
	Mat descriptors_object, descriptors_back;
	detector->detectAndCompute(img, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(back, Mat(), keypoints_back, descriptors_back);

	Mat img_gray;
	
	convertGray(img, img_gray);


	BFMatcher bfMatcher;
	std::vector<DMatch> matches;
	bfMatcher.match(descriptors_object, descriptors_back, matches);

	if (matches.size() == 0)
	{
		std::cerr << "Cannot find any similitude in the image" << std::endl;
		return img;
	}
	
	//Get good matches
	double dist = 0;
	std::vector<DMatch> good_matches;
	//Compute average distance
	for (std::vector<DMatch>::iterator d_match = matches.begin(); d_match != matches.end(); ++d_match)
	{
		dist += (*d_match).distance;
	}
	
	double dist_mean = (dist / matches.size());
	double threshold_dist = dist_mean * 0.5;
	std::vector<Point2f> p0;
	std::vector<Point2f> p1;

	for (std::vector<DMatch>::iterator d_match = matches.begin(); d_match != matches.end(); ++d_match)
	{
		if ((*d_match).distance < threshold_dist)
		{
			p0.push_back(keypoints_object[(*d_match).queryIdx].pt);
			p1.push_back(keypoints_back[(*d_match).trainIdx].pt);
		}
	}

	Mat transform = findHomography(p0, p1, RANSAC);
	Mat output;
	Size s = img_gray.size();

	warpPerspective(img, output, transform, s);
	MatIterator_<Vec3b> it_back = back.begin<Vec3b>();
	for (MatIterator_<Vec3b> it = output.begin<Vec3b>(); it != output.end<Vec3b>(); ++it, ++it_back)
	{
		if (sum(*it) == Scalar(0.0))
		{
			(*it_back) = Vec3b();
		}
		
	}
	//ShowImage(back);

	return output;
}
