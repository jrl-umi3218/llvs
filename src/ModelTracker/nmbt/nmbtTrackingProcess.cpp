/** @doc This object implements a visual process to get a disparity map.

   Copyright (c) 2003-2010,
   @author Torea Foissotte

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
#include "ModelTracker/nmbtTrackingProcess.h"

#if (LLVS_HAVE_VISP>0)

#include <sstream>

#include <visp/vpConfig.h>
#include <visp/vpXmlParserCamera.h>
#include <visp/vpImageIo.h>
#include <visp/vpPixelMeterConversion.h>

#include <visp/vpDisplayX.h>

// Debug macro
#include <llvs/tools/Debug.h>

/*!-------------------------------------
Default constructor
 -------------------------------------*/

HRP2nmbtTrackingProcess::HRP2nmbtTrackingProcess():
  m_me_modified(false),
  m_inputVispImage(0x0),
  m_ObjectCoG(4),
  m_projectedObjectCoG(3),
  m_inputImagesLoaded(false),
  m_cameraParamLoaded(false),
  m_modelLoaded(false),
  m_trackerTrackSuccess(false),
  m_initPoseLoaded(false),
  m_logData(false)


{
  m_ProcessName="nmbtTrackingProcess";
  SetDefaultParam();
  m_projectedObjectCoG[0] = 0.0;
  m_projectedObjectCoG[1] = 0.0;
  m_projectedObjectCoG[2] = 1.0;

  m_ObjectCoG[0] = 0.0;
  m_ObjectCoG[1] = 0.0  ;
  m_ObjectCoG[2] = 0.0;
  m_ObjectCoG[3] = 1.0;

}



/*!-------------------------------------
Destructor
------------------------------------- */
HRP2nmbtTrackingProcess:: ~HRP2nmbtTrackingProcess()
{



}

/*!-------------------------------------
Set tracker parameters : moving edge parameters
-------------------------------------*/
void HRP2nmbtTrackingProcess::SetMovingEdge(vpMe &_me)
{
  m_tracker.setMovingEdge(_me);
}

/*!-------------------------------------
Set tracker parameters : gain of the virtual visual servoing
-------------------------------------*/
void HRP2nmbtTrackingProcess::SetLambda(const double _lambda)
{
  m_tracker.setLambda(_lambda);
}

/*!-------------------------------------
Set tracker parameters : camera parameters
-------------------------------------*/
void HRP2nmbtTrackingProcess::SetCameraParameters(const vpCameraParameters & _cam)
{
  m_cam=_cam;m_tracker.setCameraParameters(_cam);
}


/*!
 Set Image Height
*/
void  HRP2nmbtTrackingProcess::SetHeight(const int&_height)
{
  m_imageHeight= _height;
}

/*!
  Set Image Width
*/
void  HRP2nmbtTrackingProcess::SetWidth(const int&_width)
{
  m_imageWidth=_width;
}

/*! Get tracker parameters : camera parameters */
void  HRP2nmbtTrackingProcess::GetCameraParameters(vpCameraParameters & cam)
{
  m_tracker.getCameraParameters(cam);
}

/*! Get tracker parameters : cMo camera /object pose */
void  HRP2nmbtTrackingProcess::GetcMo(vpHomogeneousMatrix &cMo)
{
  m_tracker.getPose(cMo);
}

/*! Get the image */
void  HRP2nmbtTrackingProcess::GetInputVispImages(vpImage<unsigned char> & I)
{
  I=*(m_inputVispImage);
}

/*! Get the inputcMo */
void  HRP2nmbtTrackingProcess::GetInputcMo(vpHomogeneousMatrix & inputcMo)
{
  inputcMo=this->m_inputcMo;
}

/*! Get the inputcMo */
void  HRP2nmbtTrackingProcess::GetOutputcMo(vpHomogeneousMatrix & outputcMo)
{
  outputcMo=this->m_outputcMo;
}

/*! Get Image Height*/
void  HRP2nmbtTrackingProcess::GetHeight(int&height)
{
  height= m_imageHeight;
}

/*! Get Image Width*/
void  HRP2nmbtTrackingProcess::GetWidth(int&_width)
{
  _width = m_imageWidth;
}

void HRP2nmbtTrackingProcess::GetProjectedObj(vpColVector &avec)
{
  avec = m_projectedObjectCoG;
}

/*!-------------------------------------
   Set Default Parameters
    - cMo  : init pose of the object in the image
    - path : path to file .wrl, the vrml model
-------------------------------------*/
int HRP2nmbtTrackingProcess::SetDefaultParam()
{
  ODEBUG ("Enter the function SetDefault Param " << m_pathVrml.c_str() );
  //-------------------------------
  // create the path to the box
  //-------------------------------
  string defaultPath ( "./data/model/ElectricWallFar/ElectricWallFar");
  m_pathPose =defaultPath;
  m_pathVrml = defaultPath+".wrl";



  // load the model and set the flag model loaded to true

  ODEBUG (" Path to model is : " << m_pathVrml.c_str() <<endl) ;
  LoadModel( m_pathVrml.c_str());
  m_modelLoaded = true;


  //-------------------------------
  // Load the init box pose
  //-------------------------------

  // read the pose
  LoadPose();
  m_tracker.setPose(m_inputcMo) ;
  m_initPoseLoaded = true;

  //---------------------------------
  // Load the default camera parameters
  //---------------------------------


  string camParamPath ("./data/ViSP/hrp2CamParam/hrp2.xml");
  m_pathCam =camParamPath;

  // init cam name
  m_nameCam = "cam1394_3_rectif";

  // init proje Type
  m_projType= vpCameraParameters::perspectiveProjWithoutDistortion;

  // init image dim
  m_imageWidth= 320;
  m_imageHeight= 240;

  // parse the cam
  ParseCamParam();

  // set the tracker cam parameters
  m_tracker.setCameraParameters(m_cam) ;
  m_cameraParamLoaded = true;

  //---------------------------------
  // everything is ok
  //---------------------------------
  return 0;


}


/*!-------------------------------------

Parse camera parameters

----------------------------------------*/
int HRP2nmbtTrackingProcess::ParseCamParam()
{

#if defined(VISP_HAVE_XML2)
  //create a parser
  vpXmlParserCamera parser;
  parser.parse(m_cam,
		m_pathCam.c_str(),
		m_nameCam.c_str(),
		m_projType,
		m_imageWidth,
		m_imageHeight);

  ODEBUG("camera parameter:\n"<< m_cam);
  ODEBUG("camera Path:\n"<< m_pathCam.c_str());
   m_cameraParamLoaded=true;
   return 0;
#else

   cerr << "Error: No parser available cannot parse the camera file"<<end;
   return -1;

#endif


}

/*------------------------------------

Read cMo From File
Load the last poses from files
--------------------------------------*/
int HRP2nmbtTrackingProcess::LoadPose()
{

  // create the file name
  ostringstream tmp_stream;
  tmp_stream << m_pathPose<<".0.pos";

  // open the file
  fstream finitpos ;
  finitpos.open(tmp_stream.str().c_str()) ;
  if(finitpos.fail() ){
    cout << "cannot read " <<tmp_stream.str()<<endl << "Init Failed"  << endl;
    return -1;
  }

  // if the file can be read
  vpPoseVector initpos ;
  finitpos >> initpos[0]; // tx
  finitpos >> initpos[1]; // ty
  finitpos >> initpos[2]; // tz
  finitpos >> initpos[3]; // thetaU x
  finitpos >> initpos[4]; // thetaU y
  finitpos >> initpos[5]; // thetaU z

  // build cMo
  m_inputcMo.buildFrom(initpos) ;

  //close file
  finitpos.close();

  // evrything went well
  return 0;
}

/*-------------------------------------

Set input image

 --------------------------------------*/

void HRP2nmbtTrackingProcess::SetInputVispImages(vpImage<unsigned char> * _I)
{
  m_inputVispImage=_I;
  m_inputImagesLoaded=true;

  // if the dimension are different from the stored
  // one update the dimension and the calib param
  if (m_imageWidth!=(int) m_inputVispImage->getWidth())
    {
      m_imageWidth  = m_inputVispImage->getWidth();
      m_imageHeight = m_inputVispImage->getWidth();
      ParseCamParam();
      m_tracker.setCameraParameters(m_cam) ;
    }
}


/*!-------------------------------------
 Sets the tracker parameters

The parameter names can be :

parameters for the moving edge tracking
VPME_MASK_SIZE        correlation mask size
VPME_RANGE            length of the research area
VPME_THRESHOLD        threshold for point selection,
                      the higher, the less points
VPME_SAMPLE_STEP      set the number of smaple on a line
VPME_MU1              mu1
VPME_MU2              mu2

PATH_MODEL            path name to the place where
                      the model are stored
PATH_CAM              path name to the cam xml file

TRAC_LAMBDA           lambda parameter gain of the
                      virtual visual servoing

CAME_PROJ_TYP         camera projection type : "withDistorsion" or "withoutDistorsion"
CAME_NAME             camera name

-------------------------------------*/
int HRP2nmbtTrackingProcess::pSetParameter(std::string aParameter, std::string aValue)
{
  // use of the generic function to add the parameter in the parameter list
  // A parameter can be or cannot be associated with a value,
  // thus an empty string for Value is correct.
  // If the parameter already exist is value is overwritten.
  // If this is valid the index parameter >=0 is returned,
  // -1 otherwise.
  //int outputVBPSetParameters = HRP2VisionBasicProcess::SetParameter(aParameter,aValue);

  // get the 4 first parameter to find the parameter type
  // get 4 letters starting from the letter number 0
  string paramType = aParameter.substr(0,4);
  string paramId;
  if (aParameter.length()>7)
    {
      // get the  first parameter to find the parameter type
      // get 3 letters starting from the letter number 5
      paramId = aParameter.substr(5,3);
    }

  //cout << "paramType: " << paramType   << endl;
  //cout << "paramId: " << paramId   << endl;
  bool isAPathParam(false);
  bool isAVpMeParam(false);
  bool isATrackerParam(false);
  bool isACameraParam(false);


  if (paramType=="VPME")
    {
      isAVpMeParam = true;
    }
  else if(paramType=="PATH")
    {
      isAPathParam = true;
    }
  else if(paramType=="TRAC")
    {
      isATrackerParam =true;
    }
  else if(paramType=="CAME")
    {
      isACameraParam =true;
    }
  else if(paramType=="DATA") // enable logging
    {
      m_logData = true;
    } 
  else if(paramType=="INTE") // current state
    {
       m_internalState=aValue; 
    }
  else
    {
      //cout << "Warning : unknown parameter :"<< aParameter << endl;
      return -1;
    }

//--------VPME------------//
  if(isAVpMeParam)
    {
      //static unsigned int paramnb=0;
      ODEBUG(" ENTER VPME CASE " << paramnb++);


      // convert the string aValue into a double
      std::istringstream i(aValue);
      double value;
      i >> value;

      //fill the appropriate vpMe field
      if (paramId=="MAS")//"VPME_MASK_SIZE"
	{
	  m_me.setMaskSize((int)value);
	  ODEBUG(" ENTER setMaskSize CASE value : "<<value );

	}
      else if (paramId=="RAN")//"VPME_RANGE"
	{
	  m_me.setRange((int)value);
	  ODEBUG(" ENTER setRange CASE value : "<<value );
	}
      else if (paramId=="THR")//"VPME_THRESHOLD"
	{
	  m_me.setThreshold(value);
	  ODEBUG(" ENTER THRESHOLD value : "<<value );
	}
      else if (paramId=="SAM")//"VPME_SAMPLE_STEP"
	{
	  m_me.setSampleStep(value);
	  ODEBUG(" ENTER SAM value : "<<value );
	}
      else if (paramId=="MU1")//"VPME_MU1 "
	{
	  m_me.setMu1(value);
	  ODEBUG(" ENTER MU1 value : "<<value );
	}
      else if (paramId=="MU2")//"VPME_MU2 "
	{
	  m_me.setMu2(value);
	  ODEBUG(" ENTER MU2 value : "<<value );
	}
      else
	{
	  //cout << "Warning : unknown vpme parameter :"<< paramId << endl;
	  return -1;

	}
      // If the tracker is not working
      if (!m_Computing)
	{
	  // the modification is realized
	  // NOW !
	  m_tracker.setMovingEdge(m_me);
	}
      else
	// Otherwise it is left to the time
	// where the tracker can makes
	// the parameter changes.
	m_me_modified = true;

      //m_me.print();
      //m_tracker.getMovingEdge()->print();
    }

//-------- PATH ------------//
  else if(isAPathParam)
    {
     if (paramId=="MOD")//"PATH_MODEL"
       {
 	 LoadModel(aValue);
         m_pathVrml = aValue;
       }
     else if (paramId =="CAM")//"PATH_CAM"
       {
         m_pathCam = aValue;
         ParseCamParam();
       }
     else
       {
	 //cout << "Warning : unknown path parameter :"<< paramId << endl;
	 return -1;
       }
    }
  //-------- TRACKER ------------//
  else if(isATrackerParam)
    {
     if (paramId=="LAM")//"TRAC_LAMBDA"
       {
          std::istringstream i(aValue);
	  double value;
	  i >> value;
	  m_tracker.setLambda(value);
       }
     else
       {
	 //cout << "Warning : unknown path parameter :"<< paramId << endl;
	 return -1;
       }
    }

  //------- CAMERA--------------//
  else if(isACameraParam )
    {
      if (paramId=="PRO")//"CAME_PROJ_TYP"
       {
         if(aValue=="withDistorsion")
	   {
	     m_projType=vpCameraParameters::perspectiveProjWithDistortion;
	     ParseCamParam();
	   }
	 else if(aValue=="withoutDistorsion")
	   {
	     m_projType=vpCameraParameters::perspectiveProjWithoutDistortion;
	     ParseCamParam();
	   }
	 else
           {
	     //cout << "Warning : unknown distorsion type :"<< aValue << endl;
             return -2;
           }

       }
      else if(paramId=="NAM")//"CAME_NAME"
       {
	 m_nameCam = aValue;
	 ParseCamParam();
       }
      else
       {
	 //cout << "Warning : unknown path parameter :"<< paramId << endl;
	 return -1;
       }

    }

  //return(outputVBPSetParameters);
  return 0;
}

/*!-------------------------------------
Set cMo
------------------------------------- */
void HRP2nmbtTrackingProcess:: SetcMo(const vpHomogeneousMatrix & cMo)
{
  m_inputcMo=cMo;
  m_tracker.setPose(m_inputcMo);
  m_initPoseLoaded = true;
  ODEBUG("SetcMo: " << m_inputcMo);
}


/*!-------------------------------------
Initialize the process : initialize the Tracker


Using the current pointed image
and the pose m_inputcMo,

The lines of the model are projected on the
image plane and some points on the lines are then used to define
patches.

This Patches will be used to track the line in the image.


-------------------------------------*/
int HRP2nmbtTrackingProcess:: pInitializeTheProcess()
{
  ODEBUG("Initialize the process : NMBT MODEL TRACKING .");
  m_outputcMo.setIdentity();
  m_trackerTrackSuccess = false;
  m_tracker.init(*m_inputVispImage,m_inputcMo );

  cout << m_inputcMo << endl;
  try
    {
      m_tracker.track(*m_inputVispImage) ;
    }
  catch(std::string a) // tracking got lost
    {
      
      std::cerr << std::endl;
      std::cerr << "-----    -----   Failed with exception \""
	       << a << "\"     -----    -----" << std::endl;
      std::cerr << std::endl;
      
      // set the tracking flag
      m_trackerTrackSuccess= false;
      
     // set the cMo matrix to identity
      m_outputcMo.setIdentity();
      
      // return a negative value
      return -1;
    }
 
  vpHomogeneousMatrix cMoCurr;
  m_tracker.getPose(cMoCurr);
  cout << m_inputcMo << endl;
  
  ODEBUG("End of initialize the process.");
  return 0;
}


/*!----------------------------------------
Start the process
The tracker is initialize in this process
------------------------------------------*/
int HRP2nmbtTrackingProcess::pStartProcess()
{
  ODEBUG("Go through pStartProcess NMBT MODEL TRACKING" );
  return 0;

}


/*!----------------------------------------
Stop the process
The tracker is quietly killed in this process
------------------------------------------*/
int HRP2nmbtTrackingProcess::pStopProcess()
{
  ODEBUG("Go through pStopProcess NMBT MODEL TRACKING" );
  int r=0;
  m_trackerTrackSuccess = false;
  ODEBUG("Went through pStopProcess NMBT MODEL TRACKING");
  return r;

}

/*!-------------------------------------
Realize the process
the tracker has previously been initialised with:
a cMo that is the init transform between the camera and the object
a pointer on an Image
some parameter for the tracking
the object model

-------------------------------------*/
int HRP2nmbtTrackingProcess::pRealizeTheProcess()
{

  ODEBUG("Go through pRealizeTheProcess NMBT MODEL TRACKING" );
  if (m_Verbosity>3)
    {
      std::cout << "input cMo before:" << std::endl << m_inputcMo << std::endl;
      std::cout << "output cMo before:" << std::endl << m_outputcMo << std::endl;
    }
  vpHomogeneousMatrix trackercMo;
  m_tracker.getPose(trackercMo);
  if (m_Verbosity>3)
    std::cout << "tracker cMo before:" << std::endl << trackercMo << std::endl;

  m_trackerTrackSuccess = false;

  unsigned int r=0;

  if(m_inputImagesLoaded)
    {

      try
	{
	  m_tracker.track(*m_inputVispImage) ;
	}
      catch(std::string a) // tracking got lost
	{

	  if (m_Verbosity>5)
	    {
	      std::cerr << std::endl;
	      std::cerr << "-----    -----   Failed with exception \"" << a << "\"     -----    -----" << std::endl;
	      std::cerr << std::endl;
	    }

	  // set the tracking flag
	  m_trackerTrackSuccess= false;

	  // set the cMo matrix to identity
	  m_outputcMo.setIdentity();

	  // return a negative value
	  r=-1;


	}
      if (r==0)
	{
	  // tracking succeed
	  m_trackerTrackSuccess= true;

	  // set the resulting transform between the object and the image
	  m_tracker.getPose(m_outputcMo);

	  // Compute the center of the object projected in the camera image plane.
		  
	  vpColVector intermediate = m_outputcMo * m_ObjectCoG;
	  vpColVector intermediate2(3);
	  intermediate2[0] = intermediate[0];
	  intermediate2[1] = intermediate[1];
	  intermediate2[2] = intermediate[2];

	  vpColVector projectedCoG = m_cam.get_K() * intermediate2;

	  m_projectedObjectCoG[0] = projectedCoG[0]/projectedCoG[2];
	  m_projectedObjectCoG[1] = projectedCoG[1]/projectedCoG[2];
	  m_projectedObjectCoG[2] = projectedCoG[2];

	  // For the control law,
	  // convert image pixel reference
	  // in metric values.
	  vpImagePoint ip(m_projectedObjectCoG[1],
			  m_projectedObjectCoG[0]);

	  vpPixelMeterConversion::convertPoint(m_cam,ip,
					       m_projectedObjectCoG[0],
					       m_projectedObjectCoG[1]);



	}
#if 0
      static vpDisplayX display(*m_inputVispImage,0,0,"Tracking Server");
      vpDisplay::display(*m_inputVispImage);

      m_tracker.display(*m_inputVispImage,m_outputcMo,m_cam, vpColor::green,2);
#endif

      if (m_me_modified)
	{
	  m_tracker.setMovingEdge(m_me);
	  m_me_modified = false;
	}

      if (m_Verbosity>2)
	{
	  std::cout << "input cMo after:" << std::endl << m_inputcMo << std::endl;
	  std::cout << "output cMo after:" << std::endl << m_outputcMo << std::endl;
	  vpHomogeneousMatrix trackercMo;
	  m_tracker.getPose(trackercMo);
	  std::cout << "tracker cMo after:" << std::endl << trackercMo << std::endl;
	}

      if (m_logData)
	logData();
      return r;
    }
  else
    {
      cerr << "Error :HRP2nmbtTrackingProcess::RealizeTheProcess>>  No image " <<endl;

      if (m_logData)
	logData();

      return -1;
    }
}



/*!-------------------------------------
 Cleanup the process
-------------------------------------*/
int HRP2nmbtTrackingProcess::pCleanUpTheProcess()
{
  m_tracker.resetTracker();
  return 0;
}

/*!-------------------------------------
 Load the Model
----------------------------------------*/
int HRP2nmbtTrackingProcess::LoadModel( const std::string & pathToModel)
{
  //TODO
  // add a test to check that the 3 last letter are wrl
  // add a test to check if the file exists
  // return an exception when one of these test fail
  //
  ODEBUG(" Path to model is : " << pathToModel <<endl) ;
  m_tracker.loadModel(pathToModel.c_str());

  return 0;
}


/*!--------------------------------------
  Convert LLVS image To VISP Image
-----------------------------------------*/
int HRP2nmbtTrackingProcess::ConvertLLVSImageToViSPImage()
{

  cout << "ConvertLLVSImageToViSPImage >> To be implemented" <<endl;

  return 0;
}



/*!--------------------------------------
  Convert LLVS image To VISP Image
-----------------------------------------*/
int HRP2nmbtTrackingProcess::SetInputImages()
{

  //ConvertLLVSImageToViSPImage();
  cout << "SetInputImages >> To be implemented" <<endl;
  //SetInputVispImages(vpImage<unsigned char> * _I);
  return 0;
}

void HRP2nmbtTrackingProcess::logData()
{
  static std::ofstream file("dump-nmbtTrackingProcess.dat");
  static unsigned index = 0;

  if (index == 0)
    file << "# index (0) | translation (1-3) | rotation (4-6)" << std::endl;

  vpThetaUVector thetaUVector(m_outputcMo);
  vpRxyzVector rxyzVector(thetaUVector);
  file
    << index << " "
    << m_outputcMo[0][3] << " "
    << m_outputcMo[1][3] << " "
    << m_outputcMo[2][3] << " "
    << rxyzVector[0] << " "
    << rxyzVector[1] << " "
    << rxyzVector[2] << std::endl;
  ++index;
}

#endif
