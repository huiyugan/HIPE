#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>



namespace filter
{
	namespace algos
	{
		class Resize : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(Resize, ()), _connexData(data::INOUT)
			{
				ratio = 1;
				width = 0;
				height = 0;
			}

			REGISTER_P(double, ratio);
			REGISTER_P(int, width);
			REGISTER_P(int, height);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					data::ImageArrayData images = _connexData.pop();
					if (images.getType() == data::PATTERN)
					{
						throw HipeException("The resize object cant resize PatternData. Please Develop ResizePatterData");
					}
					
					//Resize all images coming from the same parent
					for (auto &myImage : images.Array()) 
					{
						if (width == 0 || height == 0)
						{
							int l_iwidth = myImage.cols;
							int l_iheight = myImage.rows;
							cv::Size size(l_iwidth / ratio, l_iheight / ratio);

							cv::resize(myImage, myImage, size, 0.0, 0.0, cv::INTER_CUBIC);
						}
						else
						{
							cv::Size size(width, height);
							cv::resize(myImage, myImage, size, 0.0, 0.0, cv::INTER_CUBIC);
						}
					}
				}
				return OK;
			}
		};

		ADD_CLASS(Resize, ratio, width, height);
	}
}
