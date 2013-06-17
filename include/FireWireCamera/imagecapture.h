#ifndef _IMAGECAPTURE_H_
#define _IMAGECAPTURE_H_

//
// ImageCapture class
//
// Hideyasu TAKAHASHI
// last modified in Sep, 2009
//

#include <iostream>

namespace ALTH{

#ifndef _PIXEL_TYPE_
#define _PIXEL_TYPE_
// pixel format
typedef enum { PIXEL_UNKNOWN=0,	// default value (not meant for use)
	PIXEL_GRAY,	    // grayscale		    (1 channel /pixel) (the smallest)
	PIXEL_GRAYA,  	// grayscale + alpha	(2 channels/pixel)
	PIXEL_RGB, 	    // RGB			        (3 channels/pixel)
	PIXEL_RGBA,  	// RGB + alpha		    (4 channels/pixel)
	PIXEL_BGR, 	    // BGR			        (3 channels/pixel)
	PIXEL_YUV,  	// YUV			        (3 channels/pixel)
	PIXEL_YUYV,  	// Y and alternating UV	(2 channels/pixel)
	PIXEL_INT,  	// integer values	    (1 channels/pixel)
	PIXEL_FLOAT,  	// float values	    	(1 channels/pixel)
	PIXEL_VOIDP,  	// void pointer		    (1 channels/pixel)
   PIXEL_F3V  	    // float[3]		        (3 channels/pixel)
} pixel_t;
#endif //_PIXEL_TYPE_


//=============================================================================
// ImageCapture Class
// @ brief thiss class is super class for image capture function.
//=============================================================================
class ImageCapture {
//---------------------------------------------------------------
// image infomation
//---------------------------------------------------------------
protected:
	int _height;          // image size(height)
	int _width;           // image size(width)
	pixel_t _ptype;       // pixel type
	int _row_size;        // byte size of row
	int _pixel_size;      // byte size of pixel

//---------------------------------------------------------------
// camera state
//---------------------------------------------------------------
protected:
	bool _activated;       // whether camera had been activated or not
public:
	bool getActivated();

//--------------------------------------------------------------
// common function for camera controll
//--------------------------------------------------------------
public:
	// constructor
	ImageCapture();

	// destructor
	virtual ~ImageCapture();

	// open camera
	virtual bool Opens()=0;

	// close camera
	virtual bool Closes()=0;

	// capture image
	// @param
	//  unsigned char* buffer : image buffer
	virtual bool CaptureImages(unsigned char* buffer)=0;

	// image size
	// @param
	//  w : image's width
	//  h : image's height
	virtual bool setImageSize(int w, int h)=0; // unpopulated function

	void getImageSize(int* w, int* h);

	// pixeltype
	// @pixel type                        @number
	//  PIXEL_GRAY   (1 channels/pixel)      1
	//  PIXEL_GRAYA  (2 channels/pixel)      2
	//  PIXEL_RGB    (3 channels/pixel)      3
	//  PIXEL_RGBA   (4 channels/pixel)      4
	//  PIXEL_BGR    (3 channels/pixel)      5
	//  PIXEL_YUV    (3 channels/pixel)      6
	pixel_t getPixelType();
	void setPixelType(pixel_t ptype);
};

};
#endif //_IMAGECAPTURE_H_