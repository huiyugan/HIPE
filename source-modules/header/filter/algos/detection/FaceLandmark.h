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

#pragma once

#pragma once
#include <iostream>

#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/SquareCrop.h>
#include <corefilter/filter_export.h>
#include <data/ShapeData.h>

#pragma warning(push, 0)   
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#pragma warning(pop)


namespace data {
	class ImageData;
}


namespace filter
{
	namespace algos
	{
		/**
		 * \var FaceLandmark::skip_frame
		 * The number of frames to skip between each detection.
		 */

		 /**
		  * \todo
		  * \brief The FaceLandmark filter will find the faces and their landmarks.
		  *
		  * The landmarks are the keypoints needed to identify the facial expression.
		  */
		class FILTER_EXPORT FaceLandmark : public filter::IFilter
		{
			int count_frame;							//<! The number of frames already processed.
			dlib::frontal_face_detector detector;		//<! The detector object used to find the faces in the images.
			std::vector<dlib::rectangle> dets;			//<! The list of all the found faces.
			dlib::shape_predictor pose_model;			//<! The model(machine learning) used to identify the shape of the faces.

			std::atomic<bool> isStart;										//<! [TODO] Boolean used to know when the thread is running and the faces should be detected?
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;		//<! [TODO] The queue containing the frames to process.
			core::queue::ConcurrentQueue<data::ShapeData> shapes;			//<! [TODO] The shapes of the found faces.
			data::ShapeData tosend;			//<! The image containing the drawn facial landmarks to output to the ConnexData port.

			boost::thread *thr_server;		//<! [TODO] Pointer to the face detection task.
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ShapeData);

			REGISTER(FaceLandmark, ()), _connexData(data::INDATA)
			{
				count_frame = 0;
				skip_frame = 4;
				detector = dlib::get_frontal_face_detector();
				isStart = false;
				thr_server = nullptr;
			}

			REGISTER_P(int, skip_frame);


			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			/**
			 * \brief Detects the faces in images. Runs as a separate thread.
			 *  Fetch its images from the imagesStack queue then feed the detectFaces method.
			 */
			void startDetectFace();

			/**
			 * \brief Find faces, if present, then their landmarks.
			 * \param image The image to process to try finding faces.
			 * \return Returns the list of Shapes of landmarks of the found faces drawn on it.
			 */
			data::ShapeData detectFaces(const data::ImageData & image);


			HipeStatus process();


			virtual void dispose()
			{
				isStart = false;

				if (thr_server != nullptr) {
					thr_server->join();
					delete thr_server;
					thr_server = nullptr;
				}
			}
		};

		/**
		 * \brief Draws on an image a complex line made of multiple points.
		 * \param img The image on which we want to draw the lines.
		 * \param d [TODO] The detected facial landmarks.
		 * \param start [TODO] The index of the first point of the line in the container.
		 * \param end [TODO] The index of the last point of the line in the container.
		 * \param isClosed [TODO] Should it be a line or a polygon (draw a line from the last point to the first).
		 */
		void draw_polyline(data::ShapeData &img, const dlib::full_object_detection& d, const int start, const int end, bool isClosed = false);
		/**
		 * \brief Draws facial landmarks on an image, using the draw_polyline method.
		 * \param shapes The list of shape on which the face will be stored.
		 * \param d [TODO] The detected facial landmarks.
		 */
		void render_face(data::ShapeData &shapes, const dlib::full_object_detection& d);

		ADD_CLASS(FaceLandmark, skip_frame);
	}
}