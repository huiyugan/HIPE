//@HIPE_LICENSE@
#pragma once

//LINUX and windows doesn't have the same version and caffe has change the main interface Net
#ifdef WIN32
#define Hipe_net Net<Dtype>
#define Hipe_blob Blob<Dtype>
#else
#define Hipe_net Net
#define Hipe_blob TBlob<Dtype>
#endif