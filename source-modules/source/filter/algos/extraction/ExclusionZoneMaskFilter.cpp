//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <filter/algos/extraction/ExclusionZoneMaskFilter.h>

HipeStatus filter::algos::ExclusionZoneMaskFilter::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat().clone();
	cv::Mat output = image.clone();

	if (!output.data) throw HipeException("Error - filter::algos::ExclusionZoneMaskFilter::process - no input data found.");

	compute_exclusion_zone_mask(output);

	PUSH_DATA(data::ImageData(output));

	PUSH_DATA(data::ImageData(image));
	return OK;
}

void filter::algos::ExclusionZoneMaskFilter::compute_exclusion_zone_mask(cv::Mat & image)
{
	cv::Size clahe_tileGridSize(clahe_tile_grid_size_x, clahe_tile_grid_size_y);
	cv::Mat mask_image;
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clahe_clip_limit, clahe_tileGridSize);

	//auto t1 = cv::getTickCount();
	cv::fastNlMeansDenoisingColored(image, mask_image, denoise_h, denoise_template_window_size, denoise_search_window_size);
	//auto t2 = cv::getTickCount();
	//auto time = (t2 - t1) / cv::getTickFrequency();
	cv::cvtColor(mask_image, mask_image, cv::COLOR_BGR2GRAY);
	clahe->apply(mask_image, mask_image);
	cv::threshold(mask_image, image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}
