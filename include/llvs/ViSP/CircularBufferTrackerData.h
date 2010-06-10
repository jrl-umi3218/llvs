/** @doc Class to store data information coming from 
    the model tracker.

   Copyright (c) 2010,
   @author O. Stasse

*/

#ifndef _CIRCULAR_BUFFER_MODEL_TRACKER_LLVS_H_
#define _CIRCULAR_BUFFER_MODEL_TRACKER_LLVS_H_

#include "CircularBuffer.t.h"

#if (LLVS_HAVE_NMBT>0)

#include <visp/vpImage.h>
#include <visp/vpHomogeneousMatrix.h>

#include "ModelTracker/nmbtTrackingProcess.h"


struct CBTrackerData
{
  vpImage<unsigned char> * image;
  timeval timestamp;
  vpHomogeneousMatrix cMo;

  CBTrackerData(){};
  CBTrackerData(const CBTrackerData &aCBTD)
  {
    image = aCBTD.image;
    timestamp = aCBTD.timestamp;
    cMo = aCBTD.cMo;
  }
  void operator=(const CBTrackerData &);
};

class CircularModelTrackerData:public CircularBuffer<CBTrackerData>
{
 public:
 CircularModelTrackerData(int SizeOfCB):
  CircularBuffer<CBTrackerData>(SizeOfCB),
    m_CBTrackerData(0)
      {
	for(unsigned int i=0;i<m_CircularBuffer.size();i++)
	  m_CircularBuffer[i].onedatum.image= new vpImage<unsigned char>(240,320); 

	m_ProcessName = "CircularModelTrackerData";
      }

  ~CircularModelTrackerData()
    {
      for(unsigned int i=0;i<m_CircularBuffer.size();i++)
	delete m_CircularBuffer[i].onedatum.image;
    }

  /* Give the tracker pointer. */
  void SetTrackerPointer(HRP2nmbtTrackingProcess * anmbt)
  {
    m_nmbt = anmbt;
  }
  
  /*  void setImageVisp(vpImage<unsigned char>* image)
  {
    m_CBTrackerData->image = image;
    }*/

 protected:
  int pRealizeProcess()
  {
    
    
  
    m_nmbt->GetOutputcMo(m_Datum->cMo);
    CircularBuffer<CBTrackerData>::pRealizeTheProcess();

    return 0;
  }
  
 private:
  HRP2nmbtTrackingProcess * m_nmbt;
  CBTrackerData * m_CBTrackerData;
};
#endif  // (LLVS_HAVE_NMBT>0)

#endif /* _CIRCULAR_BUFFER_MODEL_TRACKER_LLVS_H_ */