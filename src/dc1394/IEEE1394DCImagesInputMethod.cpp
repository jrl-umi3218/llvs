#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <sstream>

#include <dc1394/conversions.h>

using namespace std;


using std::showbase;

#define ODEBUG2(x)
#define ODEBUG3(x) cerr << "HPR2IEEE1394DCImagesInputMethod:" << x << endl
#define ODEBUG3_CONT(x) cerr << x 

#if 0
#define ODEBUG(x) cerr << "HPR2IEEE1394DCImagesInputMethod:" <<  x << endl
#define ODEBUG_CONT(x) cerr << "HPR2IEEE1394ImagesInputMethod:" <<  x << endl
#else
#define ODEBUG(x) 
#define ODEBUG_CONT(x) 
#endif



#include <dc1394/IEEE1394DCImagesInputMethod.h>

/************************************************************************
*  static functions							*
************************************************************************/


using namespace llvs;

VisionSystemProfile::VisionSystemProfile()
{
}

VisionSystemProfile::~VisionSystemProfile()
{
  for(unsigned int i=0;i<m_CameraParameters.size();i++)
    {
      if (m_CameraParameters[i]!=0)
	delete m_CameraParameters[i];
    }
}

/**************************************************************
 * class Images Input                                         *
 **************************************************************/
HRP2IEEE1394DCImagesInputMethod::HRP2IEEE1394DCImagesInputMethod() : HRP2ImagesInputMethod()
{

  m_CurrentVisionSystemProfileID = -1;
  pthread_mutexattr_t lmutattr;
  pthread_mutexattr_init(&lmutattr);
  pthread_mutex_init(&m_mutex_device,&lmutattr);

  m_mutex_device;

  m_AtLeastOneCameraPresent = false;

  HRP2VisionBasicProcess::m_ProcessName = "IEEE1394 Image grabbing";

  string ListOfVSPs[2] = {"vsp:default","vsp:unibrain"};
  string ListOfVSPsFN[2] = {"Default.vsp","Unibrain.vsp"};
  for(unsigned int i=0;i<2;i++)
    {
      string VisionSystemProfileDefault(ListOfVSPs[i]);
      string VSPDValue(ListOfVSPsFN[i]);
      SetParameter(VisionSystemProfileDefault,
		   VSPDValue);
    }
  
  /* File descriptor to the frame grabber. */
  ODEBUG("Through the constructor ");
  InitializeBoard();
  ODEBUG("Through the constructor");

    
  StartContinuousShot();
  ODEBUG("After setting the parameters.");
}

void HRP2IEEE1394DCImagesInputMethod::GetCameraFeatureValue(string aCamera, string aFeature, string &aValue)
{

  int iCamera=0;
  bool Is1394Feature=true;
  dc1394feature_info_t lFeature;

  if (aCamera=="LEFT")
    iCamera = 0;
  else if (aCamera=="RIGHT")
    iCamera = 1;
  else if (aCamera=="CYCL")
    iCamera = 2;
  else if (aCamera=="WIDE")
    iCamera = 3;

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

void HRP2IEEE1394DCImagesInputMethod::SetCameraFeatureValue(string aCamera, string aFeature, string aValue)
{
  dc1394feature_info_t lFeature;
  bool Is1394Feature=true;
  unsigned int iCamera;

  if (aCamera=="LEFT")
    iCamera = 0;
  else if (aCamera=="RIGHT")
    iCamera = 1;
  else if (aCamera=="CYCL")
    iCamera = 2;
  else if (aCamera=="WIDE")
    iCamera = 3;

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

  
  StopContinuousShot();
  u_int avalue;
  avalue = atoi(aValue.c_str());
  dc1394_feature_set_value(m_DC1394Cameras[iCamera],lFeature.id,avalue);
  StartContinuousShot();  
}

HRP2IEEE1394DCImagesInputMethod::~HRP2IEEE1394DCImagesInputMethod()
{
  if (!m_Computing)
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
	delete m_TmpImage[i];
    }

  m_BoardImagesWidth.clear();
  m_BoardImagesHeight.clear();
  m_ImagesWidth.clear();
  m_ImagesHeight.clear();
  m_TmpImage.clear();
  m_GrabbingPeriod.clear();

  m_HandleDC1394 = 0;
}

int HRP2IEEE1394DCImagesInputMethod::StartProcess()
{
  if (!m_Computing)
    {
      ODEBUG("StartProcess: Phase 1");
      HRP2VisionBasicProcess::StartProcess();
      ODEBUG("StartProcess: Phase 2");
      InitializeBoard();
      ODEBUG("StartProcess: Phase 3");
      StartContinuousShot();
      ODEBUG("StartProcess: Phase 4");
    }
  return 0;
}


int HRP2IEEE1394DCImagesInputMethod::StopProcess()
{
  HRP2VisionBasicProcess::StopProcess();
  StopContinuousShot();
  StopBoard();
  return 0;
}

int HRP2IEEE1394DCImagesInputMethod::Initialize()
{
  if (m_DC1394Cameras.size()==0)
    InitializeBoard();
  return 0;
}

int HRP2IEEE1394DCImagesInputMethod::Cleanup()
{
  HRP2VisionBasicProcess::StopProcess();
  if (m_DC1394Cameras.size()!=0)
    StopBoard();
  return 0;
}


int HRP2IEEE1394DCImagesInputMethod::GetSingleImage(unsigned char **Image, int camera, struct timeval &timestamp)
{
  if (m_Computing==0)
    return -1;

  int r=-1;
  if (m_Format[camera]=="PGM")
    r = GetImageSinglePGM(Image,camera,timestamp);
  else if (m_Format[camera]=="RAW")
    r = GetImageSingleRaw(Image,camera,timestamp);
  else if (m_Format[camera]=="RGB")
    r = GetImageSingleRGB(Image,camera,timestamp);
  return r;
}



int HRP2IEEE1394DCImagesInputMethod::GetImageSinglePGM(unsigned char **Image, int camera, struct timeval &timestamp)
{

  if ((camera<0) || ((unsigned int)camera> m_DC1394Cameras.size()))
    return -1;
  unsigned char * ImagesDst;

  struct timeval tval;
  double time1, time2;
#define LOCAL_TYPE unsigned char *

  ODEBUG("GetImageSinglePGM cam: " << camera);
  LOCAL_TYPE ImagesTab[1];
  
  if (m_TmpImage[0]==0)
    {
      ImagesTab[0] = (LOCAL_TYPE)*Image;

      try
	{
	  pthread_mutex_lock(&m_mutex_device);
	  if (dc1394_capture_dequeue(m_DC1394Cameras[camera], 
				     DC1394_CAPTURE_POLICY_WAIT, 
				     &m_VideoFrames[camera])
	      !=DC1394_SUCCESS)
	    dc1394_log_error("Failed to capture from camera %d", camera);
	  pthread_mutex_unlock(&m_mutex_device);
	  gettimeofday(&timestamp,0);
	}
      catch(std::exception &except)
	{
	  ODEBUG("Exception during snap: " << except.what() );
	}

    }
  else
    {

      ImagesTab[0] = (LOCAL_TYPE) m_TmpImage[0];

      ImagesDst = *Image;

      ODEBUG("Get Images " );
      try
	{
	  pthread_mutex_lock(&m_mutex_device);
	  if (dc1394_capture_dequeue(m_DC1394Cameras[camera], 
				     DC1394_CAPTURE_POLICY_WAIT, 
				     &m_VideoFrames[camera])
	      !=DC1394_SUCCESS)
	    dc1394_log_error("Failed to capture from camera %d", camera);
	  pthread_mutex_unlock(&m_mutex_device);
	  gettimeofday(&timestamp,0);
	}
      catch(std::exception &except)
	{
	  ODEBUG("Exception during snap: " << except.what() );
	}

      time1 = timestamp.tv_sec + 0.000001* timestamp.tv_usec;

      //m_Board->get_images(ImagesTab);
      ODEBUG("Get Images finito...");

      unsigned char *ImgSrc,*ImgDst;
      ImgDst =(unsigned char *) ImagesDst;
      

      int intervalw, intervalh, indexd, indexs;
      unsigned int BWidth, BHeight;
      BWidth = m_BoardImagesWidth[camera];
      BHeight = m_BoardImagesHeight[camera];
      intervalw = BWidth / m_ImagesWidth[camera];
      intervalh =  BHeight/ m_ImagesHeight[camera];
      
      
      ImgSrc = m_TmpImage[camera];
      
      for(unsigned int j=0;j<m_ImagesHeight[camera];j++)
	    {
	      for(unsigned int i=0;i<m_ImagesWidth[camera];i++)
		{
		  indexd = j * m_ImagesWidth[camera] + i ;
		  
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
		  
		  ImgDst[indexd] = (unsigned char ) (localsum/(intervalh*intervalw));
		}
	    }
    }

  
  if (m_VideoFrames[camera])
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_capture_enqueue (m_DC1394Cameras[camera], m_VideoFrames[camera]);
      pthread_mutex_unlock(&m_mutex_device);
    }


  //gettimeofday(&tval,0);
  //time2 = tval.tv_sec + 0.000001* tval.tv_usec;
  //ODEBUG3( time2 - time1);

  m_LastGrabbingTime[camera]= timestamp.tv_sec + 0.000001* timestamp.tv_usec;
  return 0;
}

int HRP2IEEE1394DCImagesInputMethod::GetImageSingleRGB(unsigned char **Image, int camera, struct timeval &timestamp)
{
  unsigned char * ImagesDst;

  struct timeval tval;
  double time1, time2;
#define LOCAL_TYPE unsigned char *

  ODEBUG("GetImageSingleRGB cam: " << camera);
  LOCAL_TYPE ImagesTab[1];

  pthread_mutex_lock(&m_mutex_device);  
  if (m_TmpImage[camera]==0)
    {
      ImagesTab[0] = (LOCAL_TYPE)*Image;
      
      try
	{
	  if (dc1394_capture_dequeue(m_DC1394Cameras[camera], DC1394_CAPTURE_POLICY_WAIT, &m_VideoFrames[camera])
	      !=DC1394_SUCCESS)
	    dc1394_log_error("Failed to capture from camera %d", camera);
	  gettimeofday(&timestamp,0);
	}
      catch(std::exception &except)
	{
	  ODEBUG("Exception during snap: " << except.what() );
	}
      ODEBUG("Before converting " << m_BoardImagesWidth[camera] << " " 
	      <<      m_BoardImagesHeight[camera] << " " );
      dc1394_convert_to_RGB8(m_VideoFrames[camera]->image,
			     *Image,
			     m_BoardImagesWidth[camera],
			     m_BoardImagesHeight[camera],
			     DC1394_BYTE_ORDER_UYVY,
			     DC1394_COLOR_CODING_YUV422,1);
      ODEBUG("After converting");
#if 1
      ofstream aof;
      char Buffer[128];
      sprintf(Buffer,"dump_Dst_%d.ppm",camera);
      aof.open(Buffer,ofstream::out);
      aof << "P6\n"<< m_ImagesWidth[camera] << " " << m_ImagesHeight[camera] << endl;
      aof << "255\n";
      
      unsigned char *pt =  *Image;
      for(unsigned int j=0;j<m_ImagesHeight[camera]*m_ImagesWidth[camera]*3;j++)
	aof << (unsigned char)*pt++;
      aof.close();
#endif

    }
  else
    {

      ImagesTab[0] = (LOCAL_TYPE) m_TmpImage[camera];

      ImagesDst = *Image;

      try
	{
	  if (dc1394_capture_dequeue(m_DC1394Cameras[camera], DC1394_CAPTURE_POLICY_WAIT, &m_VideoFrames[camera])
	      !=DC1394_SUCCESS)
	     dc1394_log_error("Failed to capture from camera %d", camera);
	  gettimeofday(&timestamp,0);
	}
      catch(std::exception &except)
	{
	  ODEBUG("Exception during snap: " << except.what() );
	}

      time1 = timestamp.tv_sec + 0.000001* timestamp.tv_usec;

      ODEBUG("Get Images finito...");

      unsigned char *ImgSrc,*ImgDst;
      ImgDst =(unsigned char *) ImagesDst;
      

      int intervalw, intervalh, indexd, indexs;
      unsigned int BWidth, BHeight;
      BWidth = m_BoardImagesWidth[camera];
      BHeight = m_BoardImagesHeight[camera];
      intervalw = BWidth / m_ImagesWidth[camera];
      intervalh =  BHeight/ m_ImagesHeight[camera];
            
      ImgSrc = m_VideoFrames[camera]->image;

 
      for(unsigned int j=0;j<m_ImagesHeight[camera];j++)
	{
	  for(unsigned int i=0;i<m_ImagesWidth[camera];i++)
	    {
	      indexd = j * m_ImagesWidth[camera]*3 + i*3 ;
	      
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
		ImgDst[indexd+n] = (unsigned char ) (localsum[n]/(intervalh*intervalw));
	    }
	}

#if 0
      ofstream aof;
      char Buffer[128];
      sprintf(Buffer,"dump_Dst_%d.ppm",camera);
      aof.open(Buffer,ofstream::out);
      aof << "P6\n"<< BWidth << " " << BHeight << endl;
      aof << "255\n";
      
      for(int j=0;j<m_ImagesHeight[camera]*m_ImagesWidth[camera]*3;j++)
	aof << (unsigned char) ImgDst[j];
      aof.close();
#endif

    }

  
  if (m_VideoFrames[camera])
    {
      dc1394_capture_enqueue (m_DC1394Cameras[camera], m_VideoFrames[camera]);
    }
  pthread_mutex_unlock(&m_mutex_device);  
  //gettimeofday(&tval,0);
  //time2 = tval.tv_sec + 0.000001* tval.tv_usec;
  //ODEBUG( time2 - time1);
  m_LastGrabbingTime[camera]= timestamp.tv_sec + 0.000001* timestamp.tv_usec;   
  ODEBUG("GetImageSingleRGB cam: " << camera << " Finished" );
  return 0;
}

int HRP2IEEE1394DCImagesInputMethod::GetImageSingleRaw(unsigned char **Image, int camera, struct timeval &timestamp)
{
  unsigned char * ImagesDst;

#define LOCAL_TYPE unsigned char *

  ODEBUG("GetImageSinglePGM cam: " << camera);
  LOCAL_TYPE ImagesTab[1];
  
  
  if (m_TmpImage[camera]==0)
    {
        ImagesTab[0] = (LOCAL_TYPE)*Image;

      try
	{
	  pthread_mutex_lock(&m_mutex_device);
	  if (dc1394_capture_dequeue(m_DC1394Cameras[camera], DC1394_CAPTURE_POLICY_WAIT, &m_VideoFrames[camera])
	      !=DC1394_SUCCESS)
	     dc1394_log_error("Failed to capture from camera %d", camera);
	  pthread_mutex_unlock(&m_mutex_device);
	  gettimeofday(&timestamp,0);
	}
      catch(std::exception &except)
	{
	  ODEBUG("Exception during snap: " << except.what() );
	}
    }
  else
    {

      ImagesTab[0] = (LOCAL_TYPE) m_TmpImage[camera];

      ImagesDst = *Image;

      ODEBUG("Get Images " );
      try
	{
	  pthread_mutex_lock(&m_mutex_device);
	  if (dc1394_capture_dequeue(m_DC1394Cameras[camera], DC1394_CAPTURE_POLICY_WAIT, &m_VideoFrames[camera])
	      !=DC1394_SUCCESS)
	     dc1394_log_error("Failed to capture from camera %d", camera);
	  pthread_mutex_unlock(&m_mutex_device);
	  gettimeofday(&timestamp,0);
	}
      catch(std::exception &except)
	{
	  ODEBUG("Exception during snap: " << except.what() );
	}

      //m_Board->get_images(ImagesTab);
      ODEBUG("Get Images finito...");

      unsigned char *ImgSrc,*ImgDst;
      ImgDst =(unsigned char *) ImagesDst;
      

      int intervalw, intervalh, indexd, indexs;
      unsigned int BWidth, BHeight;
      BWidth = m_BoardImagesWidth[camera];
      BHeight = m_BoardImagesHeight[camera];

      intervalw = BWidth / m_ImagesWidth[camera];
      intervalh =  BHeight/ m_ImagesHeight[camera];
      
      
      ImgSrc = m_TmpImage[camera];
      
      for(unsigned int j=0;j<m_ImagesHeight[camera];j+=2)
	    {
	      for(unsigned int i=0;i<m_ImagesWidth[camera];i+=2)
		{

		  indexd = j * m_ImagesWidth[camera] + i ;
		  
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

		  ImgDst[indexd+0] = (unsigned char ) ((float)localsum[0]/(float)(intervalh*intervalw));
		  ImgDst[indexd+1] = (unsigned char ) ((float)localsum[1]/(float)(intervalh*intervalw));
		  ImgDst[indexd+m_ImagesWidth[camera]+0] = (unsigned char ) ((float)localsum[2]/(float)(intervalh*intervalw));
		  ImgDst[indexd+m_ImagesWidth[camera]+1] = (unsigned char ) ((float)localsum[3]/(float)(intervalh*intervalw));

		}
	    }
    }
  if (m_VideoFrames[camera])
    {
      pthread_mutex_lock(&m_mutex_device);
      dc1394_capture_enqueue (m_DC1394Cameras[camera], m_VideoFrames[camera]);
      pthread_mutex_unlock(&m_mutex_device);
    }

  m_LastGrabbingTime[camera]= timestamp.tv_sec + 0.000001* timestamp.tv_usec;    
  return 0;
}


int HRP2IEEE1394DCImagesInputMethod::SetImageSize(int lw, int lh, int CameraNumber)
{
    
  if ((CameraNumber<0)|| ((unsigned int)CameraNumber>=m_DC1394Cameras.size()))
    return -1;

  m_ImagesWidth[CameraNumber] = lw;
  m_ImagesHeight[CameraNumber] = lh;
  ODEBUG("Debug images");
  ODEBUG("Allocation for m_TmpImage");
  if ((m_BoardImagesWidth[CameraNumber]!=lw) ||
      (m_BoardImagesHeight[CameraNumber]!=lh))
    {
      if (m_TmpImage[CameraNumber]==0)
	m_TmpImage[CameraNumber] = new unsigned char[m_BoardImagesWidth[CameraNumber] * 
						     m_BoardImagesHeight[CameraNumber] * 4];
    }
  
  return 0;
}


int HRP2IEEE1394DCImagesInputMethod::GetImageSize(int &lw, int &lh, int CameraNumber)
{
  lw = m_ImagesWidth[CameraNumber];
  lh = m_ImagesHeight[CameraNumber];
  return 0;
}


string HRP2IEEE1394DCImagesInputMethod::GetFormat(unsigned int CameraNumber)
{
  if ((CameraNumber>=0) && (CameraNumber<m_Format.size()))
      return m_Format[CameraNumber];
  string ErrorMsg("Error Format : wrong camera id.");
  return ErrorMsg;
}

int HRP2IEEE1394DCImagesInputMethod::SetFormat(string aFormat, unsigned int CameraNumber)
{
  if ((CameraNumber<0) || (CameraNumber>=m_Format.size()))
    return -1;

  if (aFormat=="RGB")
    {
      m_Format[CameraNumber] = "RGB";
      ODEBUG("Format Image : RGB");
      return 0;
    }
  else if (aFormat=="PGM")
    {
      m_Format[CameraNumber] = "PGM";
      ODEBUG("Format Image : PGM");
      return 0;
    }
  else if (aFormat=="RAW")
    {
      m_Format[CameraNumber] = "RAW";
      ODEBUG("Format Image : RAW");
      return 0;
    }
  
  return -1;
}


int HRP2IEEE1394DCImagesInputMethod::SetParameter(string aParameter, string aValue)
{
  HRP2VisionBasicProcess::SetParameter(aParameter,aValue);
  string CameraPrefix;
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
  else if (CameraPrefix=="vsp:")
    {
      lpos=4;
      string ProfileName = aParameter.substr(lpos,aParameter.length()-lpos);
      ReadConfigurationFileVVVFormat(aValue,ProfileName);
    }
  
  if (IsACamera)
    {
      unsigned char IsFeature=0;
      
      string lFeature = aParameter.substr(lpos,aParameter.length()-lpos);
      for(unsigned int i=0;i<m_Features.size();i++)
	if (lFeature == m_Features[i])
	  {
	    IsFeature=1;
	    break;
	  }
      
      if (IsFeature)
	{
	  ODEBUG(" Modificatio of camera parameters: " << CameraPrefix << " " << lFeature << " " << aValue);
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

void HRP2IEEE1394DCImagesInputMethod::InitializeBoard()
{
  ODEBUG("Start InitializeBoard");
  try 
    {

      /*! List of camera. */
      dc1394camera_list_t * list;
  
      m_HandleDC1394 =  dc1394_new ();
      dc1394error_t err;
      err = dc1394_camera_enumerate(m_HandleDC1394, &list);

      m_DC1394Cameras.resize(list->num);
      unsigned int j=0;
      for (unsigned i = 0; i < list->num; i++) {
        m_DC1394Cameras[j] = dc1394_camera_new (m_HandleDC1394, list->ids[i].guid);
        if (!m_DC1394Cameras[j]) {
	  dc1394_log_warning("Failed to initialize camera with guid %llx", list->ids[i].guid);
	  continue;
        }
	else
	  {
	    ODEBUG("Created " << i << " camera with guid "<<list->ids[i].guid);
	  }
        j++;
      }
      dc1394_camera_free_list (list);

    }
  catch(...)
    {
      ODEBUG("Unable to initialize the board correctly\n");
      return;
    }
  DetectTheBestVisionSystemProfile();

  if (m_DC1394Cameras.size()==0)
    m_AtLeastOneCameraPresent = false;
  else
    m_AtLeastOneCameraPresent = true;
    
  
  m_VideoFrames.resize(m_DC1394Cameras.size());
  for(unsigned int k=0;k<m_DC1394Cameras.size();k++)
    m_VideoFrames[k] = 0;

  m_BoardImagesWidth.resize(m_DC1394Cameras.size());
  m_BoardImagesHeight.resize(m_DC1394Cameras.size());
  m_ImagesWidth.resize(m_DC1394Cameras.size());
  m_ImagesHeight.resize(m_DC1394Cameras.size());
  m_TmpImage.resize(m_DC1394Cameras.size());
  m_GrabbingPeriod.resize(m_DC1394Cameras.size());
      

  InitializeCameras();

  m_LastGrabbingTime.resize(m_DC1394Cameras.size());
  m_Format.resize(m_DC1394Cameras.size());
  for(unsigned int li=0;li<m_DC1394Cameras.size();li++)
    {
      m_LastGrabbingTime[li]=-1.0;
      m_Format[li] = "RGB";
    }

  m_GrabbingPeriod.resize(m_DC1394Cameras.size());
 

  for(unsigned int i=0;i<m_DC1394Cameras.size();i++)
    FromFrameRateToTime(i);

  m_Computing = 1;
  ODEBUG("End of InitializeBoard");
}


void HRP2IEEE1394DCImagesInputMethod::DecideBasicFeatureOnCamera(dc1394camera_t &aCamera,
								 dc1394video_mode_t &res,
								 dc1394framerate_t &fps,
								 unsigned int CameraNb)
{
  ODEBUG("Vendor name :" << aCamera.vendor << " aCamera name " << aCamera.model);
  if (m_CurrentVisionSystemProfileID!=-1)
    {
      IEEE1394DCCameraParameters *aCam = m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
	->m_CameraParameters[CameraNb];
      
      if (aCam->GetFPS()== "30fps")
	fps = DC1394_FRAMERATE_30;
      else if (aCam->GetFPS()== "15fps")
	fps = DC1394_FRAMERATE_15;

      if (aCam->GetFormat()=="640x480-Y(mono)")
	{
	  res =     DC1394_VIDEO_MODE_640x480_MONO8;
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
  else if (!strcmp(aCamera.vendor,"Unibrain"))
    {
      if (!strcmp(aCamera.model,"Fire-i 1.2"))
	{
	  res = DC1394_VIDEO_MODE_320x240_YUV422;
	  fps = DC1394_FRAMERATE_30;
	  m_BoardImagesWidth[CameraNb]= 320;
	  m_BoardImagesHeight[CameraNb]= 240;
	}
    }
  else if (!strcmp(aCamera.vendor,""))
    {
      if (!strcmp(aCamera.model,"Fire-i 1.2"))
	{
	  res = DC1394_VIDEO_MODE_320x240_YUV422;
	  fps = DC1394_FRAMERATE_30;
	  m_BoardImagesWidth[CameraNb]= 320;
	  m_BoardImagesHeight[CameraNb]= 240;
	}
    }
}
void HRP2IEEE1394DCImagesInputMethod::InitializeCameras()
{
  ODEBUG("Begin InitializeCameras()");
  if (!m_AtLeastOneCameraPresent)
    return;

  for (unsigned int i = 0; i < m_DC1394Cameras.size(); i++) 
    {
      ODEBUG("Camera GUID:" << (*m_DC1394Cameras[i]).guid);
      dc1394video_mode_t res=DC1394_VIDEO_MODE_320x240_YUV422;
      dc1394framerate_t fps=DC1394_FRAMERATE_30;
      
      DecideBasicFeatureOnCamera(*m_DC1394Cameras[i],res,fps,i);
      dc1394error_t err;
      unsigned int NUM_BUFFERS=8;
      
      ODEBUG("Speed");
      pthread_mutex_lock(&m_mutex_device);      
      err=dc1394_video_set_iso_speed(m_DC1394Cameras[i], DC1394_ISO_SPEED_400);
      DC1394_ERR(err,"Could not set ISO speed");
      
      ODEBUG("Resolution");
      err=dc1394_video_set_mode(m_DC1394Cameras[i], res);
      DC1394_ERR(err,"Could not set video mode");
      
      ODEBUG("FPS");
      err=dc1394_video_set_framerate(m_DC1394Cameras[i], fps);
      DC1394_ERR(err,"Could not set framerate");
      
      ODEBUG("NbBuffers");
      err=dc1394_capture_setup(m_DC1394Cameras[i],NUM_BUFFERS, DC1394_CAPTURE_FLAGS_DEFAULT);
      DC1394_ERR(err,"Could not setup camera-\nmake sure \
                          that the video mode and framerate \
                          are\nsupported by your camera"); 
      pthread_mutex_unlock(&m_mutex_device);

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
      err=dc1394_video_set_transmission(m_DC1394Cameras[i], DC1394_ON);
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
      dc1394_camera_free(m_DC1394Cameras[i]);
      pthread_mutex_unlock(&m_mutex_device);

    }
  if (m_HandleDC1394!=0)
    dc1394_free (m_HandleDC1394);
  CleanMemory();
  
  ODEBUG("End of StopBoard");
}

unsigned int HRP2IEEE1394DCImagesInputMethod::GetNumberOfCameras()
{
  return m_DC1394Cameras.size();
}

void HRP2IEEE1394DCImagesInputMethod::FromFrameRateToTime(int CameraNumber)
{
  
}


double HRP2IEEE1394DCImagesInputMethod::NextTimeForGrabbing(int CameraNumber)
{
  return m_LastGrabbingTime[CameraNumber]+ m_GrabbingPeriod[CameraNumber];
}

bool HRP2IEEE1394DCImagesInputMethod::CameraPresent()
{
  return m_AtLeastOneCameraPresent;
}

void HRP2IEEE1394DCImagesInputMethod::ReadConfigurationFileVVVFormat(string aFileName,
								     string ProfileName)
{
  ifstream aif;
  unsigned int lBoardNumber;
  unsigned int lNbOfCameras;

  VisionSystemProfile *aVSP;

  aif.open((const char *)aFileName.c_str(),ifstream::in);
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
	  string lGUID,lFormat,tmp,lFPS;
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
	      ODEBUG("WhiteBalance : " << lWhiteBalance[0] << " " <<lWhiteBalance[1]);
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

bool HRP2IEEE1394DCImagesInputMethod::DetectTheBestVisionSystemProfile()
{
  int IndexBestCandidate=-1;
  unsigned int ScoreBestCandidate=0;
  vector<unsigned int> lScoreCandidates;
  lScoreCandidates.resize(m_VisionSystemProfiles.size());

  // Loop in the list of vision system profiles
  for(unsigned int i=0;i<m_VisionSystemProfiles.size();i++)
    {
      ODEBUG("Detection: " << m_VisionSystemProfiles[i]->m_Name);
      lScoreCandidates[i] = 0;

      // For each camera inside the profile 
      for(unsigned int j=0;j<m_VisionSystemProfiles[i]->m_CameraParameters.size();j++)
	{
	  string sVSPCameraGUID = m_VisionSystemProfiles[i]
	    ->m_CameraParameters[j]->GetGUID();
	  uint64_t VSPCameraGUID;
	  sscanf(sVSPCameraGUID.c_str(),"%llx", &VSPCameraGUID);

	  // Try to find the camera listed in the profile inside the list
	  // of connected camera.
	  for(unsigned int k=0;k<m_DC1394Cameras.size();k++)
	    {
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

  ODEBUG(ScoreBestCandidate << " " << IndexBestCandidate);
  if ((IndexBestCandidate>-1) && (ScoreBestCandidate>0))
    {
      m_CurrentVisionSystemProfileID = IndexBestCandidate;
    }

  if (m_CurrentVisionSystemProfileID==-1)
    {
      ODEBUG3("No profile found.");
      return false;
    }

  // Check if two profiles do not have the same score.
  for(unsigned int k=0;k<lScoreCandidates[k];k++)
    {
      if ((lScoreCandidates[k]==(unsigned int)ScoreBestCandidate) &&
	  (m_CurrentVisionSystemProfileID!=(int)k))
	{
	  // in such case, it is not possible to decide which
	  // profile is best.
	  ODEBUG3("Unable to find proper profile.");
	  return false;
	}
    }

  // If there is less cameras in the profile than
  // connected, the ones not inside the profile should be
  // dismissed.
  if (m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]->m_CameraParameters.size()
      != m_DC1394Cameras.size())
    {
      if (m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]->m_CameraParameters.size()
	  < m_DC1394Cameras.size())
	  {
	    // Try to find the camera connected inside the list
	    // of profiled camera.
	    vector<dc1394camera_t *>::iterator it_DC1394Camera;
	    for(it_DC1394Camera = m_DC1394Cameras.begin();
		it_DC1394Camera!=m_DC1394Cameras.end();
		it_DC1394Camera++)
	    {
	      
	      bool FoundTheCamera=false;
	      
	      for(unsigned int k=0;k<m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
		    ->m_CameraParameters.size();k++)
		{
		  string sVSPCameraGUID = m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
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
		  m_DC1394Cameras.erase(it_DC1394Camera);
		}
	    }
	    
	  }
      else
	{
	  ODEBUG3("The vision system profile elected did not find all the listed cameras ");
	  ODEBUG3("among the connected ones. The system might work in a deprecated mode.");
	}
	  
    }

  // Make sure that the order in the internal representation is the same than in the 
  // profile one.
  vector<dc1394camera_t *> NewDC1394Cameras;
  for(unsigned int k=0;k<m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
	->m_CameraParameters.size();k++)
    {
      string sVSPCameraGUID = m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]
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
	      NewDC1394Cameras.insert(NewDC1394Cameras.end(), (*it_DC1394Camera));
	      break;
	    }
	}
    }

  m_DC1394Cameras = NewDC1394Cameras;
	
   
  ODEBUG("Current Vision System Profile:" << m_VisionSystemProfiles[m_CurrentVisionSystemProfileID]->m_Name);
  return true;
}
