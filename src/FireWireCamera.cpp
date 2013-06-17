// -*- C++ -*-
/*!
 * @file  FireWireCamera.cpp
 * @brief Capture 1394 Camera
 * @date $Date$
 *
 * $Id$
 */

#include "FireWireCamera.h"

// Module specification
// <rtc-template block="module_spec">
static const char* firewirecamera_spec[] =
  {
    "implementation_id", "FireWireCamera",
    "type_name",         "FireWireCamera",
    "description",       "Capture 1394 Camera",
    "version",           "1.0.0",
    "vendor",            "Osaka Univ",
    "category",          "Camera",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "0",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.string_output_color_format", "RGB",
    "conf.default.string_camera_param_filename", "camera.yml",
    "conf.default.ImageWindow", "on",
    // Widget
    "conf.__widget__.string_output_color_format", "text",
    "conf.__widget__.string_camera_param_filename", "text",
    "conf.__widget__.ImageWindow", "text",
    // Constraints
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
FireWireCamera::FireWireCamera(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_CameraImageOut("CameraImage", m_CameraImage)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
FireWireCamera::~FireWireCamera()
{
}



RTC::ReturnCode_t FireWireCamera::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("CameraImage", m_CameraImageOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("string_output_color_format", m_string_output_color_format, "RGB");
  bindParameter("string_camera_param_filename", m_string_camera_param_filename, "camera.yml");
  bindParameter("ImageWindow", m_ImageWindow, "on");
  
  // set window's name
  _imgViewer.setWindowName("FireWebCamera Image");
  std::cout << "### FireWebCamera Component ###" << std::endl;
	

  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FireWireCamera::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FireWireCamera::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FireWireCamera::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t FireWireCamera::onActivated(RTC::UniqueId ec_id)
{
	if(!_camera.Opens()){
		std::cout << "Open camera: failed" << std::endl;
		return RTC::RTC_ERROR;
	}
	std::cout << "Open camera: done" << std::endl;

  // load camera parameter file
  std::cout << "Load camera parameter from " << m_string_camera_param_filename << std::endl;
  cv::FileStorage fs(m_string_camera_param_filename, cv::FileStorage::READ);
  if (!fs.isOpened())
    {
      RTC_ERROR(("Unable to open camera parameter file. [%s]",
		 m_string_camera_param_filename.c_str()));
      std::cerr << "Could not open camera parameter." << std::endl;
      return RTC::RTC_ERROR;
    }

  // show paramaters
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
  fs["image_width"] >> cam_param.imageSize.width;
  fs["image_height"] >> cam_param.imageSize.height;
  fs["camera_matrix"] >> cam_param.cameraMatrix;
  fs["distortion_coefficients"] >> cam_param.distCoeffs;
#else  // CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
  cv::FileNode node(fs.fs, NULL);
  cam_param.imageSize.width = node["image_width"];
  cam_param.imageSize.height = node["image_height"];
  cv::FileNode fn = node["camera_matrix"];
  cv::read(fn, cam_param.cameraMatrix);
  fn = node["distortion_coefficients"];
  cv::read(fn, cam_param.distCoeffs);
#endif  // CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2

  std::cout << "image size = " << cam_param.imageSize.width << " x " << cam_param.imageSize.height << std::endl;
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
  std::cout << "camera matrix = " << cam_param.cameraMatrix << std::endl;
  std::cout << "distortion coefficients = " << cam_param.distCoeffs << std::endl;
#endif
  std::cout << std::endl;


	_camera.getImageSize(&iwidth, &iheight);
	std::cout << "Image Size: height " << iheight << ",width " << iwidth << std::endl;
	int isize = iwidth*iheight*3; // width*height*3channels


	m_CameraImage.data.image.raw_data.length(isize);
	m_CameraImage.data.intrinsic.distortion_coefficient.length(5);

	// set window size
	_imgViewer.setWindowSize(iwidth, iheight);

	_befTime = UTIL::getCurrentTimeStamp();


  return RTC::RTC_OK;
}


RTC::ReturnCode_t FireWireCamera::onDeactivated(RTC::UniqueId ec_id)
{
	// close camera
	if(!_camera.Closes()){
		std::cout << "Close camera :failed" << std::endl;
		return RTC::RTC_ERROR;
	}
	std::cout << "Close camera :done " << std::endl;

    // close window
	_imgViewer.closeWindow();


  return RTC::RTC_OK;
}


RTC::ReturnCode_t FireWireCamera::onExecute(RTC::UniqueId ec_id)
{
	// capture image
	if(!_camera.CaptureImages((unsigned char*)&(m_CameraImage.data.image.raw_data[0]))){
		std::cout << "Cannot capture camera image" << std::endl;
		return RTC::RTC_ERROR;
	}

	// set image property
	m_CameraImage.data.image.width  = iwidth;
	m_CameraImage.data.image.height = iheight;
	m_CameraImage.data.image.format = Img::CF_RGB;


    // copy camera intrinsic matrix to Data Port
    m_CameraImage.data.intrinsic.matrix_element[0] = cam_param.cameraMatrix.at<double>(0, 0);
	m_CameraImage.data.intrinsic.matrix_element[1] = cam_param.cameraMatrix.at<double>(0, 1);
    m_CameraImage.data.intrinsic.matrix_element[2] = cam_param.cameraMatrix.at<double>(1, 1);
    m_CameraImage.data.intrinsic.matrix_element[3] = cam_param.cameraMatrix.at<double>(0, 2);
    m_CameraImage.data.intrinsic.matrix_element[4] = cam_param.cameraMatrix.at<double>(1, 2);
    m_CameraImage.data.intrinsic.distortion_coefficient.length(cam_param.distCoeffs.rows);

    // copy distortion coefficient to Data Port
    for(int j = 0; j < 5; ++j)
      m_CameraImage.data.intrinsic.distortion_coefficient[j] = cam_param.distCoeffs.at<double>(j, 0);

	//set time stamp
	m_CameraImage.tm = UTIL::getCurrentTimeStamp();

	// send image to outport
	if(!m_CameraImageOut.write()) {
		//std::cout << "Image was not sent!" << std::endl;
		//std::cout << "TimeStamp: " << m_ImageData.tm.sec << "[sec] " << m_ImageData.tm.nsec << "[nsec]" << std::endl;
	}

	// show image buffer
	if(m_ImageWindow == "on"){
		_imgViewer.openWindow();
		char key(_imgViewer.waitKeyDisplay(&m_CameraImage.data.image.raw_data[0], 10));
		if( key == 's'){
			char filename[80];
			//static string filename="Image";				
			static int fidx=0;
			sprintf( filename,  "Image_%02d.png" , fidx);
			if( _imgViewer.saveImage( filename)){
			//if( _imgViewer[0].saveImage(&m_CameraImage.data.image.raw_data[0], filename)){
				fidx++;
				std::cout << "Saving Image done ...." << std::endl;
			}
			else
				std::cout << "Error occured when image was saved" << std::endl;
		}
	} else if(m_ImageWindow  == "off"){
		_imgViewer.closeWindow();
	}

	_curTime = UTIL::getCurrentTimeStamp();
	double  tm = UTIL::toDouble( _curTime ) - UTIL::toDouble( _befTime);
	//std::cout << "One cycle of get one image : " << tm.sec <<"." << tm.nsec << ",  Frame rate = "<< 1000000000.0 /tm.nsec <<" \r" <<std::ends;
	cout << setiosflags(ios::left);
	std::cout << "One cycle of get one image : " <<setw(10) << tm << ",  Frame rate = "<< setw(10) <<1.0 / tm  <<" \r" <<std::ends;
	_befTime = _curTime;



  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FireWireCamera::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FireWireCamera::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t FireWireCamera::onReset(RTC::UniqueId ec_id)
{
	std::cout << "reset" << std::endl;
	// close camera
	if(_camera.getActivated()){
		_camera.Closes();
	}
	// close window
	_imgViewer.closeWindow();
	
	return RTC::RTC_OK;
}


/*
RTC::ReturnCode_t FireWireCamera::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t FireWireCamera::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void FireWireCameraInit(RTC::Manager* manager)
  {
    coil::Properties profile(firewirecamera_spec);
    manager->registerFactory(profile,
                             RTC::Create<FireWireCamera>,
                             RTC::Delete<FireWireCamera>);
  }
  
};


