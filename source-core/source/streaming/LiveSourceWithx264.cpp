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

#include "LiveSourceWithx264.h"
#include <core/misc.h>
#include <core/queue/ConcurrentQueue.h>

#include <UsageEnvironment.hh>
#include <FramedSource.hh>
#include "data/ImageArrayData.h"


LiveSourceWithx264* LiveSourceWithx264::createNew(UsageEnvironment& env, core::queue::ConcurrentQueue<data::Data> & concurrent_queue)
{
	return new LiveSourceWithx264(env, concurrent_queue);
}

EventTriggerId LiveSourceWithx264::eventTriggerId = 0;

unsigned LiveSourceWithx264::referenceCount = 0;

LiveSourceWithx264::LiveSourceWithx264(UsageEnvironment& env, core::queue::ConcurrentQueue<data::Data> & concurrent_queue) : FramedSource(env), _concurrent_queue(concurrent_queue)
{
	if (referenceCount == 0)
	{

	}
	++referenceCount;
	
	encoder = new x264Encoder();
	encoder->initilize();
	if (eventTriggerId == 0)
	{
		eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
	}
	fPlayTimePerFrame = 1;
	fPreferredFrameSize = 1;
}


LiveSourceWithx264::~LiveSourceWithx264(void)
{
	--referenceCount;
	_concurrent_queue.stopListening();
	//videoCaptureDevice.release();
	encoder->unInitilize();
	envir().taskScheduler().deleteEventTrigger(eventTriggerId);
	eventTriggerId = 0;
}

void LiveSourceWithx264::encodeNewFrame()
{
	rawImage.data = NULL;
//	data::Data data;
	//_concurrent_queue.readyToListen();

	//_concurrent_queue.wait_and_pop(data);
	//const std::vector<cv::Mat> & mats = static_cast<data::ImageArrayData&>(data).Array();

	////if (mats.size() > 1 ) throw HipeException("Cannot yet stream block of matrix");
	//
	//if (mats.size() >= 1 ) 
	//	rawImage = mats[0];

	//// Got new image to stream
	//if (rawImage.data == NULL) throw HipeException("No data found for stream");

	//encoder->encodeFrame(rawImage);
	//// Take all nals from encoder output queue to our input queue
	//while (encoder->isNalsAvailableInOutputQueue() == true)
	//{
	//	x264_nal_t nal = encoder->getNalUnit();
	//	nalQueue.push(nal);
	//}

	//envir().taskScheduler().triggerEvent(eventTriggerId, this);
}

void LiveSourceWithx264::deliverFrame0(void* clientData)
{
	((LiveSourceWithx264*)clientData)->deliverFrame();
}

void LiveSourceWithx264::doGetNextFrame()
{
	if (nalQueue.empty() == true)
	{
		encodeNewFrame();
		
		deliverFrame();
	}
	else
	{
		
		deliverFrame();
	}
}

void LiveSourceWithx264::deliverFrame()
{
	if (!isCurrentlyAwaitingData()) return;
	x264_nal_t nal = nalQueue.front();
	nalQueue.pop();
	assert(nal.p_payload != NULL);
	// You need to remove the start code which is there in front of every nal unit.  
	// the start code might be 0x00000001 or 0x000001. so detect it and remove it. pass remaining data to live555    
	int trancate = 0;
	if (nal.i_payload >= 4 && nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] == 1)
	{
		trancate = 4;
	}
	else
	{
		if (nal.i_payload >= 3 && nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 1)
		{
			trancate = 3;
		}
	}

	if (nal.i_payload - trancate > fMaxSize)
	{
		fFrameSize = fMaxSize;
		fNumTruncatedBytes = nal.i_payload - trancate - fMaxSize;
	}
	else
	{
		fFrameSize = nal.i_payload - trancate;
	}

	if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0) {
		if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
			// This is the first frame, so use the current time:
			hipe_gettimeofday(&currentTime, nullptr);
			fPresentationTime = currentTime;
			fLastPlayTime = 0;
		}
		else {
			// Increment by the play time of the previous data:
			struct timeval tvalAfter;
			hipe_gettimeofday(&tvalAfter, nullptr);
			fLastPlayTime = ((tvalAfter.tv_sec - currentTime.tv_sec) * 1000000L
				+ tvalAfter.tv_usec) - currentTime.tv_usec;
			unsigned int uSeconds = fPresentationTime.tv_usec + fLastPlayTime;
			fPresentationTime.tv_sec += uSeconds / 1000000;
			fPresentationTime.tv_usec = uSeconds % 1000000;
			currentTime = tvalAfter;
		}

		// Remember the play time of this data:
		//fLastPlayTime = (fPlayTimePerFrame*fFrameSize) / fPreferredFrameSize;
		fDurationInMicroseconds = fLastPlayTime;
	}
	else {
		// We don't know a specific play time duration for this data,
		// so just record the current time as being the 'presentation time':
		hipe_gettimeofday(&currentTime, nullptr);
	}


	memmove(fTo, nal.p_payload + trancate, fFrameSize);
	FramedSource::afterGetting(this);
}
