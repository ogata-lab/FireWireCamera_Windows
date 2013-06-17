#include "imagecaptureIEEE1394Cam.h"

namespace ALTH{

// constructor
IEEE1394Cam::IEEE1394Cam():
_use_stereo(false){
}

// destructor
IEEE1394Cam::~IEEE1394Cam() {
	if (_activated){
		Closes();
	}
}

//======================================================================================
// implimetation of functions for IEEE1394 camera (for Point Gray Inc.)
//======================================================================================
// initialize camera
bool IEEE1394Cam::Opens() {
#ifdef WIN32
	FlyCaptureError fe;

	// Open the camera
	fe = flycaptureCreateContext( &_flycapture );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}

	// Initialize the WebIEEE1394Cam context
	fe = flycaptureInitialize( _flycapture, 0 );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}

	// Get camera information
	fe = flycaptureGetCameraInfo( _flycapture, &_pInfo );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}

	// set video mode
	_videoMode = FLYCAPTURE_VIDEOMODE_ANY;

	if ( _pInfo.CameraModel == FLYCAPTURE_BUMBLEBEE2 || _pInfo.CameraModel == FLYCAPTURE_FLEA2 || _pInfo.CameraModel == FLYCAPTURE_FLEA ) {
		// start transfering the image from camera
		if (!_use_stereo) {
			// flea, flea2
			fe = flycaptureStart( _flycapture, _videoMode, FLYCAPTURE_FRAMERATE_ANY);
		} else {
			// bumblebee2
			fe = flycaptureStartCustomImage( _flycapture, 3, 0, 0, 640, 480, 100, FLYCAPTURE_RAW16);
		}
		
		if (fe != FLYCAPTURE_OK) {
			return false;
		}
	} else {
		// camera is not supported by this class
		return false;
	}

	FlyCaptureImage flycaptureImage;
	// Grab an image from the camera
	fe = flycaptureGrabImage2( _flycapture, &flycaptureImage );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}
	
	_height = flycaptureImage.iRows;
	_width = flycaptureImage.iCols;
	_row_size =	flycaptureImage.iRowInc;

	_activated = true;
	return true;
#else
	return false;
#endif //WIN32
}

// open camera
bool IEEE1394Cam::Opens(bool stereo){
	setStereo(stereo);
	return Opens();
}

bool IEEE1394Cam::setImageSize(int w, int h){
	return false;
}
// capture image
bool IEEE1394Cam::CaptureImages(unsigned char* buffer) {
	if (!_activated){return false;}
#ifdef WIN32
	FlyCaptureError fe;
	FlyCaptureImage flycaptureImage;
	// Grab an image from the camera
	fe = flycaptureGrabImage2( _flycapture, &flycaptureImage );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}

	if (flycaptureImage.pixelFormat != FLYCAPTURE_RAW16 && flycaptureImage.pixelFormat != FLYCAPTURE_RAW8) {
		// RGB mode
		if (_ptype == PIXEL_BGR) {
			flycaptureInplaceRGB24toBGR24(flycaptureImage.pData, flycaptureImage.iRows*flycaptureImage.iCols*flycaptureImage.iNumImages);
		}
		memcpy(buffer, flycaptureImage.pData, flycaptureImage.iRowInc*flycaptureImage.iRows*flycaptureImage.iNumImages);
	} else {
		// convert pixel format raw8/16 to BGR
		FlyCaptureImage destimg;
		destimg.pData = buffer;
		destimg.pixelFormat = FLYCAPTURE_BGR;
		flycaptureConvertImage( _flycapture, &flycaptureImage, &destimg);
		if (_ptype == PIXEL_RGB){
			// BGR->RGB
			unsigned char* src = buffer;
			unsigned char tmp;
			for(int i=0; i<destimg.iRows; ++i){
				for(int j=0; j<destimg.iCols; ++j){
					tmp = src[0];
					src[0] = src[2];
					src[2] = tmp;
					src+=3;
				}
			}
		}
	}

	return true;
#else
	return false;
#endif //WIN32
}

// capture image
// for bumblebee2
bool IEEE1394Cam::CaptureImages(unsigned char* bufferL, unsigned char*bufferR) {
	if (!_activated){return false;}
#ifdef WIN32
	FlyCaptureError fe;
	FlyCaptureImage flycaptureImage;
	// Grab an image from the camera
	fe = flycaptureGrabImage2( _flycapture, &flycaptureImage );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}

	if (flycaptureImage.pixelFormat != FLYCAPTURE_RAW16) {
		if (_ptype == PIXEL_BGR) {
			flycaptureInplaceRGB24toBGR24(flycaptureImage.pData, flycaptureImage.iRows*flycaptureImage.iCols*flycaptureImage.iNumImages);
		}
		for (int i=0; i<flycaptureImage.iRows; ++i){
			memcpy(&bufferR[flycaptureImage.iRowInc*flycaptureImage.iNumImages*i], &flycaptureImage.pData[flycaptureImage.iRowInc*flycaptureImage.iNumImages*i], flycaptureImage.iRowInc);
			memcpy(&bufferL[flycaptureImage.iRowInc*(flycaptureImage.iNumImages*(i-1)+1)], &flycaptureImage.pData[flycaptureImage.iRowInc*(flycaptureImage.iNumImages*(i-1)+1)], flycaptureImage.iRowInc);
		}
	} else {
		// convert pixel format to RGB or BGR
		FlyCaptureImage rgb_image;
		unsigned char* buf = new unsigned char[flycaptureImage.iRows*flycaptureImage.iCols*flycaptureImage.iNumImages*4];
		rgb_image.pData = buf;
		flycaptureSetColorProcessingMethod( _flycapture, FLYCAPTURE_EDGE_SENSING );
		flycaptureConvertImage( _flycapture, &flycaptureImage, &rgb_image); // ƒxƒCƒ„[->BGRU
		unsigned char* dstR = bufferR;
		unsigned char* dstL = bufferL;
		unsigned char* src = buf;
		if (_ptype == PIXEL_BGR) { 
			for (int i=0; i<rgb_image.iRows; ++i) { // BGRU->BGR
				// right image
				for (int j=0; j< flycaptureImage.iCols; ++j) {
					dstR[0] = src[0];//b
					dstR[1] = src[1];//g
					dstR[2] = src[2];//r
					dstR+=3; src+=4;
				}
				// left image
				for (int j=0; j< flycaptureImage.iCols; ++j) {
					dstL[0] = src[0];//b
					dstL[1] = src[1];//g
					dstL[2] = src[2];//r
					dstL+=3; src+=4;
				}
			}
		} else {
			for (int i=0; i<rgb_image.iRows; ++i) { // BGRU->RGB
				// right image
				for (int j=0; j< flycaptureImage.iCols; ++j) {
					dstR[0] = src[2];//r
					dstR[1] = src[1];//g
					dstR[2] = src[0];//b
					dstR+=3; src+=4;
				}
				// left iamge
				for (int j=0; j< flycaptureImage.iCols; ++j) {
					dstL[0] = src[2];//r
					dstL[1] = src[1];//g
					dstL[2] = src[0];//b
					dstL+=3; src+=4;
				}
			}
		}
		delete [] buf;
	}

	return true;
#else
	return false;
#endif //WIN32
}

// close camera
bool IEEE1394Cam::Closes() {
#ifdef WIN32
	FlyCaptureError fe;
	// Close the camera
	fe = flycaptureStop( _flycapture );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}
	fe = flycaptureDestroyContext( _flycapture );
	if (fe != FLYCAPTURE_OK) {
		return false;
	}

	_activated = false;
	return true;
#else
	return false;
#endif //WIN32
}

//===================================================================
// get/set function for camera state
//===================================================================
bool IEEE1394Cam::setStereo(bool stereo){
	if(!_activated){
		_use_stereo = stereo;
		return true;
	}
	return false;
}
bool IEEE1394Cam::getStereo(){
	return _use_stereo;
}


};

//===================================== test code =====================================
#if 0
#include <iostream>
#include "ImageViewer.h"
int main(){
	IEEE1394Cam ic;
	ImageViewer iv;

	ic.setPixelType(ALTH::PIXEL_RGB);
	ic.Opens();
	int w, h;
	ic.getImageSize(&w, &h);
	unsigned char *buf = new unsigned char[w*h*3];
	iv.openWindow("test window", w, h);
	for(int i=0; i<100; ++i){
		ic.CaptureImages(buf);
		iv.showImage(buf);
	}
	ic.Closes();
	iv.closeWindow();

	return 0;
}
#endif
//================================= end of test code ==================================
