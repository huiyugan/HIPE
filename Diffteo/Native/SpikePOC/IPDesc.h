#pragma once
#include <opencv2/core/mat.hpp>

class IPDesc
{
public:
	cv::Point corner;
	int width;
	int height;
	cv::Mat descImg;

public :
	IPDesc(cv::Mat object, int ptx, int pty, int ptWidth, int ptHeigt)
	{
		corner.x = ptx;
		corner.y = pty;
		width = ptWidth;
		height = ptHeigt;
		descImg = object;
	}

};
