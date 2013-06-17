#include "imagecapture.h"

namespace ALTH{

//======================================================================================
// implimentation of common function for ImageCapture class
//======================================================================================
// constructor
ImageCapture::ImageCapture()
:_height(0),_width(0),_ptype(PIXEL_RGB),_row_size(0), _pixel_size(0),
_activated(false){
}

// destructor
ImageCapture::~ImageCapture() {
}

// set image size
// @param
//  w : image's width
//  h : image's height

//void ImageCapture::setImageSize(int w, int h) {
//	_height = h;
//	_width = w;
//}

// get image size
// @param
//  w : image's width
//  h : image's height
void ImageCapture::getImageSize(int* w, int* h) {
	*h = _height;
	*w = _width;
}

// get pixel type
pixel_t ImageCapture::getPixelType() {
	return _ptype;
}
// set pixel type
void ImageCapture::setPixelType(pixel_t ptype) {
	_ptype = ptype;
}

// get activated
bool ImageCapture::getActivated(){
	return _activated;
}

}; // namespace ALTH