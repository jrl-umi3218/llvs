/** @doc This object implements the abstract
    part of the acquisition of images.

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
#ifndef _HRP2_INPUT_METHOD_H_
#define _HRP2_INPUT_METHOD_H_


#include <string>
#include <sys/time.h>
using namespace std;

/*! This class defines the abstract class for the input method
   related to the low level HRP2 vision system.
   This will be used to input either files, either grabbed images,
   or event simulated ones.
   
   Copyright CNRS,JRL/AIST, 2004,
   Olivier Stasse

   07/12/2004: Modification for the new camera.
   16/05/2004: Creation
*/
class HRP2ImagesInputMethod
{
 public:
  
  /*! Constantes */
  static const int CAMERA_LEFT = 0;
  static const int CAMERA_RIGHT = 1;
  static const int CAMERA_UP = 2;
  static const int CAMERA_WIDE = 3;
  /*! Constructor */
  HRP2ImagesInputMethod();
  
  /*! Destructor */
  virtual ~HRP2ImagesInputMethod();

  /*! Takes a new image.
   * Input: 
   * unsigned char * Image : A pointer where to store the image.
   * int camera : Reference to the image itself.
   */
  virtual int GetSingleImage(unsigned char **Image, int camera, struct timeval &timestamp);

  /*! Set the size of the image willing to be grabbed. */
  virtual int SetImageSize(int lw, int lh, int CameraNumber);

  /*! Get the current image size for the appropriate camera */
  virtual int GetImageSize(int &lw, int &lh, int CameraNumber);

  /*! Get the current format of the image */
  virtual string GetFormat();

  /*! Set the level of verbosity */
  int SetLevelOfVerbosity(int VerbosityParameter);

  /*! Get the level of verbosity */
  int GetLevelOfVerbosity();

  /*! Get the number of camera */
  virtual int GetNumberOfCameras();

  /*! Get the next time for grabbing an image. */
  virtual double NextTimeForGrabbing(int CameraNumber);

 protected:
  /*! Members of the class storing the size of the images. 
     All the three images have the same size. 
  */
  unsigned int m_ImagesWidth[4], m_ImagesHeight[4];

  /*! Level of verbosity */
  int m_Verbosity;
};
  
#endif /* _HRP2_INPUT_METHOD_H_ */
