#ifndef _IEEE1394CAM_H_
#define _IEEE1394CAM_H_

#include "imagecapture.h"

//
// IEEE1394Cam class
//
// @author Hideyasu TAKAHASHI
// @last modified in Sep., 2009
//

//=============================================================================
// PGR Includes
//=============================================================================
//Windows ver.
#ifdef WIN32
#include <pgrflycapture.h>
#endif //WIN32

namespace ALTH{

//=============================================================================
// IEEE1394Cam class
// @ brief: this class supports point gray camera (bumblebee2 and flea2 ) on windows.
//          this class outputs RGB/BGR image.
// @ Note: this class depends on FryCapture library.
//         Please set pgrflycapture.lib for point gray camera.
//=============================================================================
class IEEE1394Cam : public ImageCapture
{
public:
	// constructor
	IEEE1394Cam();
	// destructor
	~IEEE1394Cam();
//---------------------------------------------------------------
// FlyCapture's variable and function
//---------------------------------------------------------------
private:
#ifdef WIN32
	FlyCaptureContext _flycapture;      // context
	FlyCaptureInfoEx _pInfo;            // capture infomation:the kinds of Camera, and so on.
	FlyCapturePixelFormat _pixelFormat; // pixel format
	FlyCaptureVideoMode _videoMode;     // capture mode
#endif //WIN32

//---------------------------------------------------------------
// camera state
//---------------------------------------------------------------
private:
	bool _use_stereo;      // stereo camera(for bumblebee):"true/false", true:both, false:right
	                      // monocular camera:"false"
// get/set function for camera state
public:
	bool setStereo(bool stereo);
	bool getStereo();

//--------------------------------------------------------------
// common function for camera control
//--------------------------------------------------------------
	// open camera
	bool Opens();
	// open camera
	// @param
	//   stereo : stereo/single
	bool Opens(bool stereo);

	// close camera
	bool Closes();


	//set image size;
	bool setImageSize( int w, int h); //unpopluated function

	// capture image
	// @param
	//  unsigned char* buffer : image buffer
	bool CaptureImages(unsigned char* buffer);
	// capture image for stereo camera
	// @param
	//  bufferL : image buffer of Left Camera
	//  bufferR : image buffer of Right Camera
	bool CaptureImages(unsigned char* bufferL, unsigned char*bufferR);
};


};
#endif //_IEEE1394CAM_H_