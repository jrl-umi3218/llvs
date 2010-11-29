/** @doc This file implements the access to IEEE 1394 cameras
    through the dc library.

    CVS Information:
    $Id$
    $Author$
    $Date$
    $Revision$
    $Source$
    $Log$

    Copyright (c) 2003-2006,
    @author Olivier Stasse

    JRL-Japan, CNRS/AIST

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the CNRS and AIST nor the names of its contributors
    may be used to endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <dc1394/IEEE1394DCImagesInputMethod.h>

/*! Includes system */

#include <strings.h>   //bzero

#include <cstring>    //strcmp
#include <cstdlib>    //atoi

#include <iomanip>
#include <fstream>
#include <sstream>     //stringstream
#include <stdexcept>

#include <boost/foreach.hpp>

/*! Includes dc1394 library */
#include <dc1394/conversions.h>

/*! Debug macros */
#include <llvs/tools/Debug.h>

/*! FIXME:  In current state, this
  macro seems useless since the change of type would trigger
  compiler conversion errors (ImagesTab var is used as right
  operand without any explicit cast).*/
#define LOCAL_IMAGE_TYPE unsigned char *

using namespace llvs;

VisionSystemProfile::VisionSystemProfile()
{}

VisionSystemProfile::~VisionSystemProfile()
{
  for (unsigned int i = 0; i < m_CameraParameters.size(); ++i)
    if (m_CameraParameters[i])
      delete m_CameraParameters[i];
}


namespace
{
  std::string messageFromError (dc1394error_t error);

  std::string messageFromError (dc1394error_t error)
  {
    switch (error)
      {
      case DC1394_SUCCESS:
	return "success";
      case DC1394_FAILURE:
	return "failure";
      default:
	return "unknown error";
      }
  }
} // end of anonymous namespace.

class Dc1394Exception : public std::runtime_error
{
public:
  Dc1394Exception (dc1394error_t error)
    : std::runtime_error (messageFromError (error))
  {}
};

#define LLVS_DC1394_CHECK(X)			\
  do						\
    {						\
      dc1394error_t error = X;			\
      if (error != DC1394_SUCCESS)		\
	throw Dc1394Exception (error);		\
    }						\
  while (0)

namespace dc1394
{
  class Camera : boost::noncopyable
  {
  public:
    Camera (dc1394_t& handle, dc1394camera_id_t id)
      : camera (0)
    {
      camera = dc1394_camera_new_unit (&handle, id.guid, id.unit);
      if (!camera)
	throw "failed to create camera";
    }

    ~Camera ()
    {
      dc1394_camera_free (camera);
    }

    dc1394camera_t* camera;
  };

  struct CameraList
  {
    CameraList (dc1394_t& handle)
    {
      dc1394camera_list_t* list = 0;

      // FIXME: check that this macro is exception safe.
      LLVS_DC1394_CHECK (dc1394_camera_enumerate (&handle, &list));

      if (!list)
	throw "failed to retrieve camera list";
      ids.resize (list->num);
      for (unsigned i = 0; i < list->num; ++i)
	ids[i] = list->ids[i];
      dc1394_camera_free_list (list);
    }
    std::vector<dc1394camera_id_t> ids;
  };
} // end of namespace dc1394.

/**************************************************************
 * class Images Input                                         *
 **************************************************************/
HRP2IEEE1394DCImagesInputMethod::HRP2IEEE1394DCImagesInputMethod ()
throw (const char*)
  : HRP2ImagesInputMethod(),
    m_ModeRaw2RGB(HRP2IEEE1394DCImagesInputMethod::YUV422_TO_RGB),
    m_HandleDC1394(0),
    m_AtLeastOneCameraPresent(false),
    m_CurrentVisionSystemProfileID(-1)
{
  dc1394_log_set_default_handler (DC1394_LOG_DEBUG);

  /* Create input mutex to avoid concurrent access */
  pthread_mutex_init(&m_mutex_device, NULL);

  m_MapFromSemanticToRealCamera.resize(4);
  for(unsigned int i=0;i<4;i++)
    m_MapFromSemanticToRealCamera[i] = -1;

  HRP2VisionBasicProcess::m_ProcessName = "IEEE1394 Image grabbing";

  std::string ListOfVSPs[2] = {"vsp:default","vsp:unibrain"};
  std::string ListOfVSPsFN[2] = {"Default.vsp","Unibrain.vsp"};
  for(unsigned int i=0;i<2;i++)
    {
      std::string VisionSystemProfileDefault(ListOfVSPs[i]);
      std::string VSPDValue(ListOfVSPsFN[i]);
      pSetParameter(VisionSystemProfileDefault,
		   VSPDValue);
    }

  /* File descriptor to the frame grabber. */
  ODEBUG("Through the constructor ");
  InitializeBoard();
  ODEBUG("Through the constructor");

  StartContinuousShot();
  ODEBUG("After setting the parameters.");
}

void
HRP2IEEE1394DCImagesInputMethod::GetCameraFeatureValue
(std::string aCamera, std::string aFeature, std::string &aValue)
{

  int iCamera=0;
  bool Is1394Feature=true;
  dc1394feature_info_t lFeature;

  if (aCamera=="LEFT")
    iCamera = m_MapFromSemanticToRealCamera[0];
  else if (aCamera=="RIGHT")
    iCamera = m_MapFromSemanticToRealCamera[1];
  else if (aCamera=="CYCL")
    iCamera = m_MapFromSemanticToRealCamera[2];
  else if (aCamera=="WIDE")
    iCamera = m_MapFromSemanticToRealCamera[3];

  if (iCamera==-1)
    return;

  // Default value.
  lFeature.id = DC1394_FEATURE_BRIGHTNESS;

  if (aFeature=="BRIGHTNESS")
    lFeature.id = DC1394_FEATURE_BRIGHTNESS;
  else if (aFeature=="AUTO_EXPOSURE")
    lFeature.id = DC1394_FEATURE_EXPOSURE;
  else if (aFeature=="WHITE_BALANCE")
    lFeature.id = DC1394_FEATURE_WHITE_BALANCE;
  else if (aFeature=="GAMMA")
    lFeature.id = DC1394_FEATURE_GAMMA;
  else if (aFeature=="SHUTTER")
    lFeature.id = DC1394_FEATURE_SHUTTER;
  else if (aFeature=="GAIN")
    lFeature.id = DC1394_FEATURE_GAIN;
  else if (aFeature=="Format")
    {
      Is1394Feature = false;
      aValue = m_Format[iCamera];
      return;
    }

  if(Is1394Feature)
    {
      u_int avalue=0;

      pthread_mutex_lock(&m_mutex_device);
      dc1394_feature_get_value(m_DC1394Cameras[iCamera],lFeature.id,&avalue);
      pthread_mutex_unlock(&m_mutex_device);
      char Buffer[1024];
      bzero(Buffer,1024);
      sprintf(Buffer,"%d",avalue);
      aValue = Buffer;
    }
}

void HRP2IEEE1394DCImagesInputMethod::SetCameraFeatureValue
(std::string aCamera, std::string aFeature, std::string aValue)
{
  dc1394feature_info_t lFeature;
  bool Is1394Feature=true;
  int iCamera=0;

  if (aCamera=="LEFT")
    iCamera = m_MapFromSemanticToRealCamera[0];
  else if (aCamera=="RIGHT")
    iCamera = m_MapFromSemanticToRealCamera[1];
  else if (aCamera=="CYCL")
    iCamera = m_MapFromSemanticToRealCamera[2];
  else if (aCamera=="WIDE")
    iCamera = m_MapFromSemanticToRealCamera[3];

  if (iCamera==-1)
    return;

  // Default value.
  lFeature.id = DC1394_FEATURE_BRIGHTNESS;

  ODEBUG("Feature: " << aFeature);
  if (aFeature=="BRIGHTNESS")
    lFeature.id = DC1394_FEATURE_BRIGHTNESS;
  else if (aFeature=="AUTO_EXPOSURE")
    lFeature.id = DC1394_FEATURE_EXPOSURE;
  else if (aFeature=="WHITE_BALANCE")
    lFeature.id = DC1394_FEATURE_WHITE_BALANCE;
  else if (aFeature=="GAMMA")
    lFeature.id = DC1394_FEATURE_GAMMA;
  else if (aFeature=="SHUTTER")
    lFeature.id = DC1394_FEATURE_SHUTTER;
  else if (aFeature=="GAIN")
    lFeature.id = DC1394_FEATURE_GAIN;
  else if (aFeature=="Format")
    {
      // TODO : Improve the way we can change the format
      // depending on the camera system.
      bool Is1394Feature=false;
      m_Format[iCamera] = aValue;
      return;
    }

  ODEBUG("Feature id " << lFeature.id << " Camera : " << iCamera);
  u_int avalue2=0;
  dc1394_feature_get_value(m_DC1394Cameras[iCamera],lFeature.id,&avalue2);
  ODEBUG("Value taken from dc1394 before : " << avalue2);
  dc1394feature_mode_t amode;
  dc1394_feature_get_mode(m_DC1394Cameras[iCamera],lFeature.id,&amode);
  ODEBUG("Mode: " <<amode);
  amode =DC1394_FEATURE_MODE_MANUAL;
  dc1394_feature_set_mode(m_DC1394Cameras[iCamera],lFeature.id,amode);

  dc1394_feature_get_mode(m_DC1394Cameras[iCamera],lFeature.id,&amode);
  ODEBUG("Mode: " <<amode);

  //  StopContinuousShot();
  u_int avalue;
  avalue = atoi(aValue.c_str());
  ODEBUG(aFeature <<" : " << avalue);
  dc1394error_t errorcode;
  if ((errorcode=dc1394_feature_set_value
       (m_DC1394Cameras[iCamera],lFeature.id,avalue)) < 0)
    {
    }
  ODEBUG("Error code : " << errorcode);

  dc1394_feature_get_value(m_DC1394Cameras[iCamera],lFeature.id,&avalue2);
  ODEBUG("Value taken from dc1394 after : " << avalue2);
  ODEBUG("-------------------------------------------------");

  //  StartContinuousShot();
}

HRP2IEEE1394DCImagesInputMethod::~HRP2IEEE1394DCImagesInputMethod()
{
  //  if (!m_Computing)
  StopBoard();

  for(unsigned int i=0;i<m_VisionSystemProfiles.size();i++)
    {
      if (m_VisionSystemProfiles[i]!=0)
	delete m_VisionSystemProfiles[i];
    }

}

void HRP2IEEE1394DCImagesInputMethod::CleanMemory()
{
  m_DC1394Cameras.clear();
  m_VideoFrames.clear();

  for(unsigned int i=0;i<m_DC1394Cameras.size();i++)
    {
      if (m_TmpImage[i] != 0)
	{
	  delete m_TmpImage[i];
	  m_TmpImage[i] = 0;
	}
    }

  m_BoardImagesWidth.clear();
  m_BoardImagesHeight.clear();
  m_TmpImage.clear();
  m_GrabbingPeriod.clear();

  m_HandleDC1394 = 0;
}

int HRP2IEEE1394DCImagesInputMethod::pStartProcess()
{
  if (!m_Computing)
    {
      ODEBUG("StartProcess: Phase 1");
      ODEBUG("StartProcess: Phase 2");
      InitializeBoard();
      ODEBUG("StartProcess: Phase 3");
      StartContinuousShot();
      ODEBUG("StartProcess: Phase 4");
    }
  return 0;
}


int HRP2IEEE1394DCImagesInputMethod::pStopProcess()
{
  ODEBUG("StopProcess: Phase 1");
  ODEBUG("StopProcess: Phase 2");
  StopContinuousShot();
  ODEBUG("StopProcess: Phase 3");
  StopBoard();
  ODEBUG("StopProcess: Phase 4");
  return 0;
}

bool HRP2IEEE1394DCImagesInputMethod::Initialize()
{
  if (m_DC1394Cameras.size()==0)
    {
      try
	{
	  InitializeBoard();
	}
      catch(const char* msg)
	{
	  ODEBUG("Exception occured: " << msg);
	  return false;
	}
    }
  return true;
}

void HRP2IEEE1394DCImagesInputMethod::Cleanup()
{
  StopProcess();
}


unsigned int
HRP2IEEE1394DCImagesInputMethod::GetSingleImage
(unsigned char** Image, const unsigned int& SemanticCamera, double& timestamp)
{
  unsigned int cameraNumber;
  unsigned int result = GetCameraId(SemanticCamera, cameraNumber);
  if(result != RESULT_OK)
    {
      ODEBUG("Error #" << result << " occured.");
      return result;
    }

  if (m_Computing == 0)
    return ERROR_IMAGE_INPUT_NOT_READY;

  ODEBUG("m_Format["<<cameraNumber << "]=" << m_Format[cameraNumber]);
  if (m_Format[cameraNumber]=="PGM")
    return GetImageSinglePGM(Image,cameraNumber,timestamp);

  else if (m_Format[cameraNumber]=="RAW")
    return GetImageSingleRaw(Image,cameraNumber,timestamp);
  else if (m_Format[cameraNumber]=="RGB")
    return GetImageSingleRGB(Image,cameraNumber,timestamp);

  return ERROR_UNKNOWN_FORMAT;
}

unsigned int
HRP2IEEE1394DCImagesInputMethod::CaptureDequeue
(const unsigned int& cameraNumber)
{
  unsigned int dc1394result;

  //FIXME: Mutex might be specific to the physical camera. This method
  // works but add a useless constraint on all cameras.
  pthread_mutex_lock(&m_mutex_device);
  try
    {
      //FIXME: If camera is unplugged during the execution,
      //DC1394_CAPTURE_POLICY_WAIT might not be the good solution
      //since it will never return.
      dc1394result = dc1394_capture_dequeue(m_DC1394Cameras[cameraNumber],
					    DC1394_CAPTURE_POLICY_WAIT,
					    &m_VideoFrames[cameraNumber]);
    }
  catch(std::exception &except)
    {
      ODEBUG("Exception during snap: " << except.what() );
      pthread_mutex_unlock(&m_mutex_device);
      return ERROR_SNAP_EXCEPTION;
    }
  pthread_mutex_unlock(&m_mutex_device);
  if( dc1394result != DC1394_SUCCESS )
    {
      dc1394_log_error("Failed to capture from camera %d", cameraNumber);
      return ERROR_SNAP_EXCEPTION;
    }
  return RESULT_OK;
}

unsigned int
HRP2IEEE1394DCImagesInputMethod::GetImageSinglePGM
(unsigned char **Image, const unsigned int& cameraNumber, double &timestamp)
{
  unsigned char * ImagesDst;
  struct timeval tval;
  double time1, time2;

  ODEBUG("GetImageSinglePGM cam: " << cameraNumber);
  LOCAL_IMAGE_TYPE ImagesTab[1];

  if (m_TmpImage[0]==0)
    {
      ImagesTab[0] = (LOCAL_IMAGE_TYPE)*Image;
      unsigned int result = CaptureDequeue(cameraNumber);
      if( result != RESULT_OK )
	{
	  return result;
	}

      timestamp=m_VideoFrames[cameraNumber]->timestamp*1e-6;

    }
  else
    {
      ImagesTab[0] = (LOCAL_IMAGE_TYPE) m_TmpImage[0];

      ImagesDst = *Image;

      ODEBUG("Get Images " );
      unsigned int result = CaptureDequeue(cameraNumber);
      if( result != RESULT_OK )
	{
	  return result;
	}

      timestamp=m_VideoFrames[cameraNumber]->timestamp*1e-6;

      time1 = timestamp;

      //m_Board->get_images(ImagesTab);
      ODEBUG("Get Images finito...");

      unsigned char *ImgSrc,*ImgDst;
      ImgDst =(unsigned char *) ImagesDst;


      int intervalw, intervalh, indexd, indexs;
      unsigned int BWidth, BHeight;
      BWidth = m_BoardImagesWidth[cameraNumber];
      BHeight = m_BoardImagesHeight[cameraNumber];
      intervalw = BWidth / m_ImagesWidth[cameraNumber];
      intervalh =  BHeight/ m_ImagesHeight[cameraNumber];


      ImgSrc = m_TmpImage[cameraNumber];

      for(unsigned int j=0;j<m_ImagesHeight[cameraNumber];j++)
	{
	  for(unsigned int i=0;i<m_ImagesWidth[cameraNumber];i++)
	    {
	      indexd = j * m_ImagesWidth[cameraNumber] + i ;

	      indexs = j * intervalh * BWidth * 3  + i * intervalw *3;
	      unsigned long localsum = 0;
	      for(int l=0;l<intervalh;l++)
		{
		  int lindexs = l * BWidth * 3  + indexs;
		  for(int m=0;m<intervalw;m++)
		    {
		      localsum += ImgSrc[lindexs+m*3];
		    }
		}

	      ImgDst[indexd] =
		(unsigned char) (localsum/(intervalh*intervalw));
	    }
	}

    }


  if (m_VideoFrames[cameraNumber])
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_capture_enqueue
	(m_DC1394Cameras[cameraNumber], m_VideoFrames[cameraNumber]);
      pthread_mutex_unlock(&m_mutex_device);
    }

  m_LastGrabbingTime[cameraNumber]= timestamp;
  return RESULT_OK;
}

unsigned int
HRP2IEEE1394DCImagesInputMethod::GetImageSingleRGB
(unsigned char **Image, const unsigned int& cameraNumber, double &timestamp)
{
  unsigned char * ImagesDst;
  struct timeval tval;
  double time1, time2;

  ODEBUG("GetImageSingleRGB cam: " << cameraNumber);
  LOCAL_IMAGE_TYPE ImagesTab[1];

  ODEBUG("After mutex acquisition " << (int)(m_TmpImage[cameraNumber]!=0) );
  if (m_TmpImage[cameraNumber]==0)
    {
      ODEBUG("GetImageSingleRGB before dc1394_capture without TmpImage");
      ImagesTab[0] = (LOCAL_IMAGE_TYPE)*Image;

      unsigned int result = CaptureDequeue(cameraNumber);
      if( result != RESULT_OK )
	{
	  return result;
	}
      timestamp=m_VideoFrames[cameraNumber]->timestamp*1e-6;

      ODEBUG3("Before converting " << m_BoardImagesWidth[cameraNumber] << " "
	     <<      m_BoardImagesHeight[cameraNumber] << "  m_ModelRaw2RGB: "
	     << m_ModeRaw2RGB );
      switch(m_ModeRaw2RGB)
	{
	case YUV422_TO_RGB:
	  dc1394_convert_to_RGB8(m_VideoFrames[cameraNumber]->image,
				 *Image,
				 m_BoardImagesWidth[cameraNumber],
				 m_BoardImagesHeight[cameraNumber],
				 DC1394_BYTE_ORDER_UYVY,
				 DC1394_COLOR_CODING_YUV422,1);
	  break;
	case BAYER_TO_RGB:
	  dc1394_bayer_decoding_8bit(m_VideoFrames[cameraNumber]->image,
				     *Image,
				     m_BoardImagesWidth[cameraNumber],
				     m_BoardImagesHeight[cameraNumber],
				     DC1394_COLOR_FILTER_GRBG,
				     DC1394_BAYER_METHOD_SIMPLE);
	  break;
	}

      ODEBUG("After converting");
    }
  else
    {
      ODEBUG("GetImageSingleRGB before dc1394_capture with TmpImage " );
      ODEBUG("camera is "<< cameraNumber);
      ImagesTab[0] = (LOCAL_IMAGE_TYPE) m_TmpImage[cameraNumber];

      ImagesDst = *Image;

      ODEBUG("ImagesDst");
      unsigned int result = CaptureDequeue(cameraNumber);
      if( result != RESULT_OK )
	{
	  return result;
	}

      timestamp=m_VideoFrames[cameraNumber]->timestamp * 1e-6;


      ODEBUG("dequeue finished");
      switch(m_ModeRaw2RGB)
	{
	case YUV422_TO_RGB:
	  ODEBUG("YUV422_TO_RGB");
	  dc1394_convert_to_RGB8(m_VideoFrames[cameraNumber]->image,
				 ImagesTab[0],
				 m_BoardImagesWidth[cameraNumber],
				 m_BoardImagesHeight[cameraNumber],
				 DC1394_BYTE_ORDER_UYVY,
				 DC1394_COLOR_CODING_YUV422,1);
	  break;
	case BAYER_TO_RGB:
	  ODEBUG("BAYER_TO_RGB");
	  dc1394_bayer_decoding_8bit(m_VideoFrames[cameraNumber]->image,
				     ImagesTab[0],
				     m_BoardImagesWidth[cameraNumber],
				     m_BoardImagesHeight[cameraNumber],
				     DC1394_COLOR_FILTER_GRBG,
				     DC1394_BAYER_METHOD_SIMPLE);
	  break;
	}


      time1 = timestamp;

      ODEBUG("Get Images finito...");

      unsigned char *ImgSrc,*ImgDst;
      ImgDst =(unsigned char *) ImagesDst;

      ODEBUG("Width:" << dec << m_ImagesWidth[cameraNumber]);
      ODEBUG("Height:" << dec << m_ImagesHeight[cameraNumber]);


      int intervalw, intervalh, indexd, indexs;
      unsigned int BWidth, BHeight;
      BWidth = m_BoardImagesWidth[cameraNumber];
      BHeight = m_BoardImagesHeight[cameraNumber];
      intervalw = BWidth / m_ImagesWidth[cameraNumber];
      intervalh =  BHeight/ m_ImagesHeight[cameraNumber];
      ImgSrc =  ImagesTab[0];

      ODEBUG("intervalw: " << intervalw);
      ODEBUG("intervalh: " << intervalh);
      for(unsigned int j=0;j<m_ImagesHeight[cameraNumber];j++)
	{
	  for(unsigned int i=0;i<m_ImagesWidth[cameraNumber];i++)
	    {
	      indexd = j * m_ImagesWidth[cameraNumber]*3 + i*3 ;

	      indexs = j * intervalh * BWidth * 3  + i * intervalw *3;
	      unsigned long localsum[3] = {0,0,0};
	      for(int l=0;l<intervalh;l++)
		{
		  int lindexs = l * BWidth * 3  + indexs;
		  for(int m=0;m<intervalw;m++)
		    {
		      for(int n=0;n<3;n++)
			localsum[n] += ImgSrc[lindexs+m*3+n];
		    }
		}
	      for(int n=0;n<3;n++)
		{
		  //ODEBUG("indedx : " << indexd << " n: " << n );
		  ImgDst[indexd+n] =
		    (unsigned char ) (localsum[n]/(intervalh*intervalw));
		}
	    }
	}
      
      if (0)
      {
	static unsigned long int lindex=0;
	ofstream aof;
	char Buffer[1028];
	sprintf(Buffer,"I-%02d-%04ld.pgm",cameraNumber,lindex++);
	aof.open(Buffer,ofstream::out);
	aof << "P5\n640 480\n255\n";
	unsigned int lpixid=0;
	for(unsigned int j=0;j<m_ImagesHeight[cameraNumber];j++)
	  {
 	    for(unsigned int i=0;i<m_ImagesWidth[cameraNumber];i++)
	      {
		double lv = ((double)ImgSrc[lpixid] +
			     (double)ImgSrc[lpixid+1] +
			     (double)ImgSrc[lpixid+2])/3.0;
		aof << (unsigned char)lv;
		lpixid+=3;
	      }
	  }
	aof.close();
      }

    }

  if (m_VideoFrames[cameraNumber])
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_capture_enqueue
	(m_DC1394Cameras[cameraNumber], m_VideoFrames[cameraNumber]);
      pthread_mutex_unlock(&m_mutex_device);
    }
  m_LastGrabbingTime[cameraNumber]= timestamp;
  ODEBUG("GetImageSingleRGB cam: " << cameraNumber << " Finished" );
  return RESULT_OK;
}

unsigned int
HRP2IEEE1394DCImagesInputMethod::GetImageSingleRaw
(unsigned char** Image, const unsigned int& cameraNumber, double& timestamp)
{
  unsigned char* ImagesDst = 0;

  ODEBUG("GetImageSinglePGM cam: " << cameraNumber);
  LOCAL_IMAGE_TYPE ImagesTab[1];


  if (m_TmpImage[cameraNumber]==0)
    {
      ImagesTab[0] = (LOCAL_IMAGE_TYPE)*Image;

      unsigned int result = CaptureDequeue(cameraNumber);
      if( result != RESULT_OK )
	{
	  return result;
	}
     timestamp=m_VideoFrames[cameraNumber]->timestamp*1e-6;

    }
  else
    {

      ImagesTab[0] = (LOCAL_IMAGE_TYPE) m_TmpImage[cameraNumber];

      ImagesDst = *Image;

      ODEBUG("Get Images " );
      unsigned int result = CaptureDequeue(cameraNumber);
      if( result != RESULT_OK )
	{
	  return result;
	}
     timestamp=m_VideoFrames[cameraNumber]->timestamp*1e-6;


      //m_Board->get_images(ImagesTab);
      ODEBUG("Get Images finito...");

      unsigned char *ImgSrc,*ImgDst;
      ImgDst =(unsigned char *) ImagesDst;


      int intervalw, intervalh, indexd, indexs;
      unsigned int BWidth, BHeight;
      BWidth = m_BoardImagesWidth[cameraNumber];
      BHeight = m_BoardImagesHeight[cameraNumber];

      intervalw = BWidth / m_ImagesWidth[cameraNumber];
      intervalh =  BHeight/ m_ImagesHeight[cameraNumber];


      ImgSrc = m_TmpImage[cameraNumber];

      for(unsigned int j=0;j<m_ImagesHeight[cameraNumber];j+=2)
	{
	  for(unsigned int i=0;i<m_ImagesWidth[cameraNumber];i+=2)
	    {

	      indexd = j * m_ImagesWidth[cameraNumber] + i ;

	      indexs = j * intervalh * BWidth  + i * intervalw ;
	      unsigned long localsum[4] = {0,0,0,0};
	      for(int l=0;l<2*intervalh;l+=2)
		{
		  int lindexs = l * BWidth   + indexs;
		  for(int m=0;m<2*intervalw;m+=2)
		    {
		      localsum[0] += ImgSrc[lindexs+m];
		      localsum[1] += ImgSrc[lindexs+m+1];
		    }
		  lindexs += BWidth;
		  for(int m=0;m<2*intervalw;m+=2)
		    {
		      localsum[2] += ImgSrc[lindexs+m];
		      localsum[3] += ImgSrc[lindexs+m+1];
		    }
		}

	      ImgDst[indexd+0] =
		(unsigned char)
		((float)localsum[0]/(float)(intervalh*intervalw));
	      ImgDst[indexd+1] =
		(unsigned char)
		((float)localsum[1]/(float)(intervalh*intervalw));
	      ImgDst[indexd+m_ImagesWidth[cameraNumber]+0] =
		(unsigned char)
		((float)localsum[2]/(float)(intervalh*intervalw));
	      ImgDst[indexd+m_ImagesWidth[cameraNumber]+1] =
		(unsigned char)
		((float)localsum[3]/(float)(intervalh*intervalw));
	    }
	}
    }
  if (m_VideoFrames[cameraNumber])
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_capture_enqueue (m_DC1394Cameras[cameraNumber],
			      m_VideoFrames[cameraNumber]);
      pthread_mutex_unlock(&m_mutex_device);
    }

  m_LastGrabbingTime[cameraNumber]= timestamp;
  return RESULT_OK;
}


unsigned int
HRP2IEEE1394DCImagesInputMethod::SetImageSize
(int lw, int lh, const unsigned int& SemanticCamera)
{
  unsigned int cameraNumber;
  unsigned int result = GetCameraId(SemanticCamera, cameraNumber);
  if(result != RESULT_OK)
    {
      return result;
    }

  m_ImagesWidth[cameraNumber] = lw;
  m_ImagesHeight[cameraNumber] = lh;
  ODEBUG("Debug images");
  ODEBUG("Allocation for m_TmpImage");

  return RESULT_OK;
}


unsigned int
HRP2IEEE1394DCImagesInputMethod::GetImageSize
(int &lw, int &lh, const unsigned int& SemanticCamera) const
{
  lw = -1;
  lh = -1;

  unsigned int cameraNumber;
  unsigned int result = GetCameraId(SemanticCamera, cameraNumber);
  if(result != RESULT_OK)
    {
      return result;
    }

  lw = m_ImagesWidth[cameraNumber];
  lh = m_ImagesHeight[cameraNumber];
  return RESULT_OK;
}


std::string
HRP2IEEE1394DCImagesInputMethod::GetFormat(const unsigned int& SemanticCamera)
  const
{
  unsigned int cameraNumber;
  unsigned int result = GetCameraId(SemanticCamera, cameraNumber);
  if(result != RESULT_OK)
    {
      std::string ErrorMsg("Argument error: Semantic id is not valid");
      return ErrorMsg;
    }
  if (cameraNumber < m_Format.size())
    {
      return m_Format[cameraNumber];
    }
  else
    {
      std::string ErrorMsg("Error Format : wrong camera id.");
      return ErrorMsg;
    }
}

unsigned int
HRP2IEEE1394DCImagesInputMethod::SetFormat
(std::string aFormat, const unsigned int& SemanticCamera)
{
  unsigned int cameraNumber;
  unsigned int result = GetCameraId(SemanticCamera, cameraNumber);
  if(result != RESULT_OK)
    {
      return result;
    }
  if (cameraNumber >= m_Format.size())
    {
      return ERROR_CAMERA_MISSING;
    }

  if (aFormat=="RGB")
    {
      m_Format[cameraNumber] = "RGB";
      ODEBUG("Format Image : RGB");
    }
  else if (aFormat=="PGM")
    {
      m_Format[cameraNumber] = "PGM";
      ODEBUG("Format Image : PGM");
    }
  else if (aFormat=="RAW")
    {
      m_Format[cameraNumber] = "RAW";
      ODEBUG("Format Image : RAW");
    }
  else
    {
      return ERROR_UNKNOWN_FORMAT;
    }

  return RESULT_OK;
}


int HRP2IEEE1394DCImagesInputMethod::pSetParameter
(std::string aParameter, std::string aValue)
{
  std::string CameraPrefix;
  unsigned char IsACamera = 0;
  unsigned int lpos = 0;

  CameraPrefix = aParameter.substr(0,4);
  if ((CameraPrefix=="LEFT") ||
      (CameraPrefix=="CYCL") ||
      (CameraPrefix=="WIDE") )
    {
      IsACamera = 1;
      lpos = 5;
    }
  else if (CameraPrefix=="RIGH")
    {
      CameraPrefix = aParameter.substr(0,5);
      if (CameraPrefix=="RIGHT")
	{
	  IsACamera = 1;
	  lpos=6;
	}
    }
  else if (CameraPrefix=="vvv:")
    {
      lpos=4;
      std::string ProfileName =
	aParameter.substr (lpos,aParameter.length()-lpos);
      ReadConfigurationFileVVVFormat(aValue,ProfileName);
    }
  else if (CameraPrefix=="vsp:")
    {
      lpos=4;
      std::string ProfileName =
	aParameter.substr(lpos,aParameter.length()-lpos);
      ReadConfigurationFileVSPFormat(aValue,ProfileName);
    }

  if (IsACamera)
    {
      ODEBUG("Is a camera");
      unsigned char IsFeature=0;

      std::string lFeature = aParameter.substr(lpos,aParameter.length()-lpos);
      for(unsigned int i=0;i<m_Features.size();i++)
	if (lFeature == m_Features[i])
	  {
	    IsFeature=1;
	    break;
	  }

      if (IsFeature)
	{
	  ODEBUG(" Modificatio of camera parameters: "
		 << CameraPrefix << " " << lFeature << " " << aValue);
	  SetCameraFeatureValue(CameraPrefix,lFeature,aValue);
	}
    }

  if (aParameter=="Format")
    {
      unsigned int lindex=0; // left by default.
      if (CameraPrefix=="RIGH")
	lindex =1 ;
      else if (CameraPrefix=="CYCL")
	lindex =2 ;
      else if (CameraPrefix=="WIDE")
	lindex =3 ;

      SetFormat(aValue,lindex);
    }
  return 0;
}

void HRP2IEEE1394DCImagesInputMethod::InitializeBoard() throw(const char*)
{
  ODEBUG("Start InitializeBoard");

  m_HandleDC1394 = dc1394_new ();
  if (!m_HandleDC1394)
    throw "failed to get a dc1394 handle.";

  dc1394::CameraList list (*m_HandleDC1394);

  m_DC1394Cameras.reserve (list.ids.size ());
  BOOST_FOREACH (const dc1394camera_id_t& cameraId, list.ids)
    {
      dc1394camera_t* camera =
	dc1394_camera_new_unit (m_HandleDC1394, cameraId.guid, cameraId.unit);
      if (camera)
	{
	  m_DC1394Cameras.push_back (camera);
	  //dc1394_reset_bus (camera);
	}
      else
	dc1394_log_warning
	  ("Failed to initialize camera with guid %llx",
	   cameraId.guid);
    }

  if( !DetectTheBestVisionSystemProfile() )
    throw "no profile available for the connected cameras";

  m_AtLeastOneCameraPresent = m_DC1394Cameras.size () > 0;

  m_VideoFrames.resize (m_DC1394Cameras.size (), 0);

  const int nCameras = 4;
  m_BoardImagesWidth.resize (nCameras);
  m_BoardImagesHeight.resize (nCameras);

  bool reallocate = !m_ImagesWidth.empty () || !m_ImagesHeight.empty ();
  if (!reallocate)
    m_ImagesWidth.resize (nCameras), m_ImagesHeight.resize (nCameras);
  else
    if (m_ImagesWidth.size () != m_DC1394Cameras.size ())
      {
	ODEBUG("Please recall manually the size of the grabber !");
      }

  m_GrabbingPeriod.resize (m_DC1394Cameras.size ());

  InitializeCameras();

  m_TmpImage.resize (nCameras);
  for(unsigned k = 0; k < m_ImagesWidth.size (); ++k)
    {
      ODEBUG(" Size of m_TmpImage[" << k << "]:"
	     << m_BoardImagesWidth[k] << " * "
	     << m_BoardImagesHeight[k] << " * "
	     << nCameras);
      size_t size = m_BoardImagesWidth[k] * m_BoardImagesHeight[k] * nCameras;
      m_TmpImage[k] = new unsigned char[size];
    }

  m_LastGrabbingTime.resize (m_DC1394Cameras.size ());
  m_Format.resize (m_DC1394Cameras.size ());
  for (unsigned int li = 0; li < m_DC1394Cameras.size (); ++li)
    {
      m_LastGrabbingTime[li] = -1.0;
      m_Format[li] = "RGB";
    }

  m_GrabbingPeriod.resize (nCameras);

  for (unsigned int i = 0;i < m_DC1394Cameras.size (); ++i)
    FromFrameRateToTime (i);

  m_Computing = 0;
  ODEBUG("End of InitializeBoard");
}


void HRP2IEEE1394DCImagesInputMethod::DecideBasicFeatureOnCamera
(dc1394camera_t &aCamera, dc1394video_mode_t &res, dc1394framerate_t &fps,
 const unsigned int& CameraNb)
{
  ODEBUG3("Vendor name :" << aCamera.vendor
	  << " aCamera name " << aCamera.model);

  if (m_CurrentVisionSystemProfileID!=-1)
    {
      IEEE1394DCCameraParameters *aCam =
	m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
	->m_CameraParameters[CameraNb];

      if (aCam->GetFPS()== "60fps")
	fps = DC1394_FRAMERATE_60;
      if (aCam->GetFPS()== "30fps")
	fps = DC1394_FRAMERATE_30;
      else if (aCam->GetFPS()== "15fps")
	fps = DC1394_FRAMERATE_15;

      if (aCam->GetFormat()=="640x480-Y(mono)")
	{
	  res = DC1394_VIDEO_MODE_640x480_MONO8;
	  m_BoardImagesWidth[CameraNb]= 640;
	  m_BoardImagesHeight[CameraNb]= 480;
	}
      else if (aCam->GetFormat()=="320x240-YUV422")
	{
	  res = DC1394_VIDEO_MODE_320x240_YUV422;
	  m_BoardImagesWidth[CameraNb]= 320;
	  m_BoardImagesHeight[CameraNb]= 240;
	}

    }
  if (!strcmp(aCamera.vendor,"Unibrain"))
    {
      if (!strcmp(aCamera.model,"Fire-i 1.2"))
	{
	  if (m_CurrentVisionSystemProfileID==-1)
	    {
	      res = DC1394_VIDEO_MODE_320x240_YUV422;
	      fps = DC1394_FRAMERATE_30;
	      m_BoardImagesWidth[CameraNb]= 320;
	      m_BoardImagesHeight[CameraNb]= 240;
	    }
	  m_ModeRaw2RGB = YUV422_TO_RGB;
	}
    }

  if (!strcmp(aCamera.vendor,"Point Grey Research"))
    {

      if ((!strcmp(aCamera.model,"Flea FLEA-COL"))
	  ||(!strcmp(aCamera.model,"Flea2 FL2-03S2C"))
	  )
	{
	  if (m_CurrentVisionSystemProfileID==-1)
	    {
	      res = DC1394_VIDEO_MODE_320x240_YUV422;
	      fps = DC1394_FRAMERATE_30;
	      m_BoardImagesWidth[CameraNb]= 320;
	      m_BoardImagesHeight[CameraNb]= 240;
	    }
	  m_ModeRaw2RGB = BAYER_TO_RGB;
	  ODEBUG("Went through here");
	}
    }
  ODEBUG("ModelRaw2RGB:" << m_ModeRaw2RGB);

}

void HRP2IEEE1394DCImagesInputMethod::InitializeCamera
(IEEE1394DCCameraParameters &CamParams)
{
  unsigned int CamId = CamParams.GetCameraNumberInUserSemantic();
  std::string iCamera, aFeature, aValue;
  if (CamId==0 )
    iCamera = "LEFT";
  else if (CamId==1)
    iCamera = "RIGHT";
  else if (CamId==2)
    iCamera = "CYCL";
  else if (CamId==3)
    iCamera = "WIDE";

  std::ostringstream oss;


  aFeature="BRIGHTNESS";
  ODEBUG("Checking - Brightness: " << CamParams.GetBrightness());
  oss << CamParams.GetBrightness();
  aValue = oss.str();
  ODEBUG("Checking 2 - Brightness: " << aValue);
  SetCameraFeatureValue(iCamera,aFeature,aValue);

  aFeature="AUTO_EXPOSURE";
  oss.str("");
  oss << CamParams.GetExposure();
  aValue = oss.str();
  SetCameraFeatureValue(iCamera,aFeature,aValue);

  aFeature="WHITE_BALANCE";
  oss.str("");
  unsigned int WB[2];
  CamParams.GetWhiteBalance(WB);
  oss << WB[0];
  aValue = oss.str();
  SetCameraFeatureValue(iCamera,aFeature,aValue);

  aFeature="GAMMA";
  oss.str("");
  oss << CamParams.GetExposure();
  aValue = oss.str();
  SetCameraFeatureValue(iCamera,aFeature,aValue);

  aFeature="SHUTTER";
  oss.str("");
  oss << CamParams.GetShutter();
  ODEBUG("Shutter=" <<aValue);
  aValue = oss.str();
  SetCameraFeatureValue(iCamera,aFeature,aValue);

  aFeature="GAIN";
  oss.str("");
  oss << CamParams.GetGain();
  ODEBUG("Checking - Gain: " << CamParams.GetGain());
  aValue = oss.str();
  ODEBUG("Gain=" <<aValue);
  SetCameraFeatureValue(iCamera,aFeature,aValue);


}

void HRP2IEEE1394DCImagesInputMethod::InitializeCameras() throw(const char*)
{
  VisionSystemProfile *aVSP =
    m_VisionSystemProfiles[m_CurrentVisionSystemProfileID];

  ODEBUG("Begin InitializeCameras()");
  if (!m_AtLeastOneCameraPresent)
    return;

  for (unsigned int i = 0; i < m_DC1394Cameras.size(); i++)
    {
      std::ostringstream oss;

      oss << "0x";
      oss.flags(ios::hex | ios::fixed);
      oss.precision(16);
      oss.width(16);
      oss <<setfill('0');
      oss << (*m_DC1394Cameras[i]).guid;

      dc1394video_mode_t res=DC1394_VIDEO_MODE_320x240_YUV422;
      dc1394framerate_t fps=DC1394_FRAMERATE_30;

      DecideBasicFeatureOnCamera(*m_DC1394Cameras[i],res,fps,i);
      dc1394error_t err;
      unsigned int NUM_BUFFERS=8;

      ODEBUG("Speed");
      err=dc1394_video_set_iso_speed(m_DC1394Cameras[i], DC1394_ISO_SPEED_400);
      DC1394_ERR(err,"Could not set ISO speed");

      ODEBUG("Resolution");
      err=dc1394_video_set_mode(m_DC1394Cameras[i], res);
      DC1394_ERR(err,"Could not set video mode");

      ODEBUG("FPS");
      err=dc1394_video_set_framerate(m_DC1394Cameras[i], fps);
      DC1394_ERR(err,"Could not set framerate");

      for(unsigned int VSPCamId = 0;
	  VSPCamId<aVSP->m_CameraParameters.size (); ++VSPCamId)
	{
	  ODEBUG(aVSP->m_CameraParameters[VSPCamId]->GetGUID()
		 << " " << oss.str());
	  if (aVSP->m_CameraParameters[VSPCamId]->GetGUID()==oss.str())
	    InitializeCamera(*aVSP->m_CameraParameters[VSPCamId]);
	}

      ODEBUG("NbBuffers");
      err = dc1394_capture_setup
	(m_DC1394Cameras[i],NUM_BUFFERS, DC1394_CAPTURE_FLAGS_DEFAULT);
      if(err == DC1394_FAILURE)
	{
	  throw("Can not access to physical camera");
	}

      if (fps==DC1394_FRAMERATE_30)
  	{
	  m_GrabbingPeriod[i]=1.0/30.0;
	}
      else if (fps==DC1394_FRAMERATE_15)
	{
	  m_GrabbingPeriod[i]=1.0/15.0;
	}

    }
  ODEBUG("End InitializeCameras()");
}


void HRP2IEEE1394DCImagesInputMethod::StartContinuousShot()
{
  if (!m_AtLeastOneCameraPresent)
    return;

  dc1394error_t err;
  for(unsigned int i=0;i<m_DC1394Cameras.size();i++)
    {
      pthread_mutex_lock(&m_mutex_device);
      err=dc1394_video_set_transmission(m_DC1394Cameras[i], DC1394_ON);
      pthread_mutex_unlock(&m_mutex_device);
      DC1394_ERR(err,"Could not start camera iso transmission");
    }

}

void HRP2IEEE1394DCImagesInputMethod::StopContinuousShot()
{
  if (!m_AtLeastOneCameraPresent)
    return;

  for(unsigned int i=0;i<m_DC1394Cameras.size();i++)
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_video_set_transmission(m_DC1394Cameras[i], DC1394_OFF);
      dc1394_capture_stop(m_DC1394Cameras[i]);
      pthread_mutex_unlock(&m_mutex_device);
    }
}


void HRP2IEEE1394DCImagesInputMethod::StopBoard()
{
  ODEBUG("Beginning of StopBoard");
  for(unsigned int i=0;i<m_DC1394Cameras.size();i++)
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_capture_stop(m_DC1394Cameras[i]);
      dc1394_video_set_transmission(m_DC1394Cameras[i], DC1394_OFF);
      dc1394_reset_bus(m_DC1394Cameras[i]);
      dc1394_camera_free(m_DC1394Cameras[i]);
      pthread_mutex_unlock(&m_mutex_device);

    }
  if (m_HandleDC1394!=0)
    dc1394_free (m_HandleDC1394);
  CleanMemory();

  ODEBUG("End of StopBoard");
}

unsigned int HRP2IEEE1394DCImagesInputMethod::GetNumberOfCameras()
  const
{
  return m_DC1394Cameras.size();
}

void HRP2IEEE1394DCImagesInputMethod::FromFrameRateToTime
(const unsigned int& CameraNumber)
  const
{

}


double HRP2IEEE1394DCImagesInputMethod::NextTimeForGrabbing
(const unsigned int& CameraNumber)
{
  return m_LastGrabbingTime[CameraNumber]+ m_GrabbingPeriod[CameraNumber];
}

bool
HRP2IEEE1394DCImagesInputMethod::CameraPresent()
  const
{
  return m_AtLeastOneCameraPresent;
}

void HRP2IEEE1394DCImagesInputMethod::ReadConfigurationFileVVVFormat
(std::string aFileName, std::string ProfileName)
{
  std::ifstream aif;
  unsigned int lBoardNumber;
  unsigned int lNbOfCameras;

  VisionSystemProfile* aVSP = 0;

  aif.open((const char *) aFileName.c_str (), ifstream::in);
  if (aif.is_open ())
    {
      aVSP = new VisionSystemProfile ();
      aVSP->m_Name = ProfileName;
      aVSP->m_FileNameDescription = aFileName;
      ODEBUG("Profile Name : " << aVSP->m_Name);
      aif >> lBoardNumber;
      aif >> lNbOfCameras;

      aVSP->m_CameraParameters.resize(lNbOfCameras);

      for(unsigned int i=0;i<lNbOfCameras;i++)
	{
	  std::string lGUID,lFormat,tmp,lFPS;
	  unsigned int lBrightness, lExposure;
	  aVSP->m_CameraParameters[i] = new IEEE1394DCCameraParameters();
	  aVSP->m_CameraParameters[i]->SetCameraNumberInUserSemantic(i);
	  aVSP->m_CameraParameters[i]->SetBoardNumber(lBoardNumber);

	  aif >> lGUID;
	  aVSP->m_CameraParameters[i]->SetGUID(lGUID);
	  ODEBUG("GUID:" << lGUID);

	  aif >> lFormat;
	  aVSP->m_CameraParameters[i]->SetFormat(lFormat);
	  ODEBUG("Format:" << lFormat);

	  aif >> lFPS;
	  aVSP->m_CameraParameters[i]->SetFPS(lFPS);
	  ODEBUG("FPS:" << lFPS);

	  aif >> tmp;
	  if (tmp=="BRIGHTNESS")
	    {
	      aif >> lBrightness;
	      aVSP->m_CameraParameters[i]->SetBrightness(lBrightness);
	      ODEBUG("Brightness:" << lBrightness);
	    }
	  aif >> tmp;
	  if (tmp=="AUTO_EXPOSURE")
	    {
	      aif >> lExposure;
	      aVSP->m_CameraParameters[i]->SetExposure(lExposure);
	      ODEBUG("Exposure:" << lExposure);
	    }
	  aif >> tmp;
	  if (tmp=="WHITE_BALANCE")
	    {
	      unsigned int lWhiteBalance[2];
	      aif >> lWhiteBalance[0];
	      aif >> lWhiteBalance[1];
	      aVSP->m_CameraParameters[i]->SetWhiteBalance(lWhiteBalance);
	      ODEBUG("WhiteBalance : " << lWhiteBalance[0]
		     << " "  << lWhiteBalance[1]);
	    }
	  aif >> tmp;
	  if (tmp=="GAMMA")
	    {
	      unsigned int lGamma;
	      aif >> lGamma;
	      aVSP->m_CameraParameters[i]->SetGamma(lGamma);
	      ODEBUG("Gamma : " << lGamma);
	    }

	  aif >> tmp;
	  if (tmp=="SHUTTER")
	    {
	      unsigned int lShutter;
	      aif >> lShutter;
	      aVSP->m_CameraParameters[i]->SetShutter(lShutter);
	      ODEBUG("Shutter : " << lShutter);
	    }

	  aif >> tmp;
	  if (tmp=="GAIN")
	    {
	      unsigned int lGain;
	      aif >> lGain;
	      aVSP->m_CameraParameters[i]->SetGain(lGain);
	      ODEBUG("Gain : " << lGain);
	    }

	}

      aif.close();

      m_VisionSystemProfiles.insert(m_VisionSystemProfiles.end(),
				    aVSP);
    }
}

void HRP2IEEE1394DCImagesInputMethod::ReadConfigurationFileVSPFormat
(std::string aFileName, std::string ProfileName)
{
  std::ifstream aif;
  unsigned int lBoardNumber;
  unsigned int lNbOfCameras;

  VisionSystemProfile *aVSP;

  aif.open((const char *) aFileName.c_str(),ifstream::in);
  if (aif.is_open())
    {
      aVSP = new VisionSystemProfile();
      aVSP->m_Name = ProfileName;
      aVSP->m_FileNameDescription = aFileName;
      ODEBUG("Profile Name : " << aVSP->m_Name);
      aif >> lBoardNumber;
      aif >> lNbOfCameras;

      aVSP->m_CameraParameters.resize(lNbOfCameras);

      for(unsigned int i=0;i<lNbOfCameras;i++)
	{
	  std::string lGUID,lFormat,tmp,lFPS;
	  unsigned int lBrightness, lExposure;
	  aVSP->m_CameraParameters[i] = new IEEE1394DCCameraParameters();

	  std::string Semantic;
	  aif >> Semantic;

	  int iCamera=0;

	  if (Semantic=="LEFT")
	    iCamera = 0;
	  else if (Semantic=="RIGHT")
	    iCamera = 1;
	  else if (Semantic=="CYCL")
	    iCamera = 2;
	  else if (Semantic=="WIDE")
	    iCamera = 3;

	  m_MapFromSemanticToRealCamera[iCamera] = i;

	  aVSP->m_CameraParameters[i]->SetCameraNumberInUserSemantic(iCamera);
	  aVSP->m_CameraParameters[i]->SetBoardNumber(lBoardNumber);

	  aif >> lGUID;
	  aVSP->m_CameraParameters[i]->SetGUID(lGUID);
	  ODEBUG("GUID:" << lGUID);

	  aif >> lFormat;
	  aVSP->m_CameraParameters[i]->SetFormat(lFormat);
	  ODEBUG("Format:" << lFormat);

	  aif >> lFPS;
	  aVSP->m_CameraParameters[i]->SetFPS(lFPS);
	  ODEBUG("FPS:" << lFPS);

	  aif >> tmp;
	  if (tmp=="BRIGHTNESS")
	    {
	      aif >> lBrightness;

	      aVSP->m_CameraParameters[i]->SetBrightness(lBrightness);
	      std::stringstream out;
	      out << lBrightness;
	      ODEBUG("Brightness:" << lBrightness);
	    }
	  aif >> tmp;
	  if (tmp=="AUTO_EXPOSURE")
	    {
	      aif >> lExposure;
	      aVSP->m_CameraParameters[i]->SetExposure(lExposure);
	      std::stringstream out;
	      out << lExposure;

	      ODEBUG("Exposure:" << lExposure);
	    }
	  aif >> tmp;
	  if (tmp=="WHITE_BALANCE")
	    {
	      unsigned int lWhiteBalance[2];
	      aif >> lWhiteBalance[0];
	      aif >> lWhiteBalance[1];
	      aVSP->m_CameraParameters[i]->SetWhiteBalance(lWhiteBalance);
	      ODEBUG("WhiteBalance : " << lWhiteBalance[0]
		     << " " <<lWhiteBalance[1]);
	    }
	  aif >> tmp;
	  if (tmp=="GAMMA")
	    {
	      unsigned int lGamma;
	      aif >> lGamma;
	      aVSP->m_CameraParameters[i]->SetGamma(lGamma);
	      std::stringstream out;
	      out << lGamma;

	      ODEBUG("Gamma : " << lGamma);
	    }

	  aif >> tmp;
	  if (tmp=="SHUTTER")
	    {
	      unsigned int lShutter;
	      aif >> lShutter;
	      aVSP->m_CameraParameters[i]->SetShutter(lShutter);
	      std::stringstream out;
	      out << lShutter;

	      ODEBUG("Shutter : " << lShutter);
	    }

	  aif >> tmp;
	  if (tmp=="GAIN")
	    {
	      unsigned int lGain;
	      aif >> lGain;
	      aVSP->m_CameraParameters[i]->SetGain(lGain);
	      std::stringstream out;
	      out << lGain;

	      ODEBUG("Gain : " << lGain);
	    }

	}

      aif.close();

      m_VisionSystemProfiles.insert(m_VisionSystemProfiles.end(),
				    aVSP);
    }
}

int HRP2IEEE1394DCImagesInputMethod::GetSemanticOfCamera
(const unsigned int& lCameraIndexOnComputer)
{
  if ((lCameraIndexOnComputer>=0) &&
      ((unsigned) lCameraIndexOnComputer <
       m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
       ->m_CameraParameters.size ()))

    return m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]->
      m_CameraParameters[lCameraIndexOnComputer]
      ->GetCameraNumberInUserSemantic();

  return -1;
}

unsigned int
HRP2IEEE1394DCImagesInputMethod::GetCameraId(const unsigned int& SemanticCamera,
                                             unsigned int& CameraNumber)
  const
{
  CameraNumber = UNDEFINED_CAMERA_NUMBER;

  //FIXME: Remove hard-coded camera number
  if(SemanticCamera > 4)
    {
      ODEBUG("Camera semantic #" << SemanticCamera << " is not defined");
      return HRP2IEEE1394DCImagesInputMethod::ERROR_UNDEFINED_SEMANTIC_CAMERA;
    }
  if(m_MapFromSemanticToRealCamera[ SemanticCamera ] == -1)
    {
      return ERROR_NO_CAMERA_ASSIGNED;
    }
  CameraNumber = m_MapFromSemanticToRealCamera[ SemanticCamera ];
  if(CameraNumber >= m_DC1394Cameras.size())
    {
      CameraNumber = UNDEFINED_CAMERA_NUMBER;
      return ERROR_CAMERA_MISSING;
    }
  return RESULT_OK;
}

bool HRP2IEEE1394DCImagesInputMethod::DetectTheBestVisionSystemProfile()
{
  int IndexBestCandidate=-1;
  unsigned int ScoreBestCandidate=0;
  vector<unsigned int> lScoreCandidates;
  lScoreCandidates.resize(m_VisionSystemProfiles.size());

  ODEBUG("Nb of file .vsp: " << m_VisionSystemProfiles.size());
  ODEBUG("Nb cam on the bus" << m_DC1394Cameras.size());
  // Loop in the list of vision system profiles
  for(unsigned int i=0;i<m_VisionSystemProfiles.size();i++)
    {
      ODEBUG("Detection: " << m_VisionSystemProfiles[i]->m_Name);
      lScoreCandidates[i] = 0;

      // For each camera inside the profile
      for(unsigned int j = 0;
	  j<m_VisionSystemProfiles[i]->m_CameraParameters.size(); ++j)
	{
	  std::string sVSPCameraGUID = m_VisionSystemProfiles[i]
	    ->m_CameraParameters[j]->GetGUID();
	  uint64_t VSPCameraGUID;
	  sscanf(sVSPCameraGUID.c_str(),"%llx", &VSPCameraGUID);
	  for(unsigned int k=0;k<m_DC1394Cameras.size();k++)
	    {
	      ODEBUG("Test :" << hex << VSPCameraGUID << " and "
		      << hex << m_DC1394Cameras[k]->guid);
	      if (VSPCameraGUID==m_DC1394Cameras[k]->guid)
		lScoreCandidates[i]++;
	    }
	}
      if (lScoreCandidates[i]>ScoreBestCandidate)
	{
	  IndexBestCandidate = (int)i;
	  ScoreBestCandidate = (int)lScoreCandidates[i];
	}
    }

  ODEBUG3(ScoreBestCandidate << " " << IndexBestCandidate);
  if ((IndexBestCandidate>-1) && (ScoreBestCandidate>0))
    {
      m_CurrentVisionSystemProfileID = IndexBestCandidate;
    }

  if (m_CurrentVisionSystemProfileID==-1)
    {
      ODEBUG("No profile found.");
      return false;
    }

  // Check if two profiles do not have the same score.
  for(unsigned int k=0;k<lScoreCandidates.size();k++)
    {
      if ((lScoreCandidates[k]==(unsigned int)ScoreBestCandidate) &&
	  (m_CurrentVisionSystemProfileID!=(int)k))
	{
	  // in such case, it is not possible to decide which
	  // profile is best.
	  ODEBUG("Unable to find proper profile.");
	  return false;
	}
    }

  // If there is less cameras in the profile than
  // connected, the ones not inside the profile should be
  // dismissed.
  if (m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
      ->m_CameraParameters.size()
      != m_DC1394Cameras.size())
    {
      if (m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
	  ->m_CameraParameters.size()
	  < m_DC1394Cameras.size())
	{
	  // Try to find the camera connected inside the list
	  // of profiled camera.
	  std::vector<dc1394camera_t *>::iterator it_DC1394Camera =
	    m_DC1394Cameras.begin();
	  while(it_DC1394Camera!=m_DC1394Cameras.end())
	    {
	      bool FoundTheCamera=false;

	      for(unsigned int k=0;k<m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
		    ->m_CameraParameters.size();k++)
		{
		  std::string sVSPCameraGUID = m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
		    ->m_CameraParameters[k]->GetGUID();
		  uint64_t VSPCameraGUID=0;
		  sscanf(sVSPCameraGUID.c_str(),"%llx", &VSPCameraGUID);
		  if (VSPCameraGUID==(*it_DC1394Camera)->guid)
		    {
		      FoundTheCamera=true;
		      break;
		    }
		}

	      // If the camera is not find in the profile
	      if(!FoundTheCamera)
		{
		  // then remove it.
		  vector<dc1394camera_t *>::iterator toDelete = it_DC1394Camera;
		  m_DC1394Cameras.erase(toDelete);
		}
	      else
		{
		  it_DC1394Camera++;
		}
	    }

	}
      else
	{
	  ODEBUG("The vision system profile selected did"
		 " not find all the listed cameras among the connected ones.");
	  ODEBUG("The system might work in a deprecated mode.");
	}
    }

  // Make sure that the order in the internal representation is the
  // same than in the profile one.
  std::vector<dc1394camera_t *> NewDC1394Cameras;
  for(unsigned int k=0;k<m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
	->m_CameraParameters.size();k++)
    {
      std::string sVSPCameraGUID =
	m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
	->m_CameraParameters[k]->GetGUID();
      uint64_t VSPCameraGUID=0;
      sscanf(sVSPCameraGUID.c_str(),"%llx", &VSPCameraGUID);
      ODEBUG("Order " << k << " : " <<sVSPCameraGUID << " " << VSPCameraGUID);
      // Try to find the profiled camera  inside the list
      // of connected camera.
      vector<dc1394camera_t *>::iterator it_DC1394Camera;
      for(it_DC1394Camera = m_DC1394Cameras.begin();
	  it_DC1394Camera!=m_DC1394Cameras.end();
	  it_DC1394Camera++)
	{
	  if (VSPCameraGUID==(*it_DC1394Camera)->guid)
	    {
	      NewDC1394Cameras.insert
		(NewDC1394Cameras.end(), (*it_DC1394Camera));
	      break;
	    }
	}
    }

  m_DC1394Cameras = NewDC1394Cameras;

  ODEBUG("Current Vision System Profile:"
	 << m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]->m_Name);
  return true;
}
