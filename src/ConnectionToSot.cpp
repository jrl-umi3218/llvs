#include <iostream>
#include <fstream>
#include "LowLevelVisionServer.h"
#include "ConnectionToSot.h"

// Debug macros
//#define _DEBUG_MODE_ON_
#include "Debug.h"

void * ConnectionToSotThread(void *arg)
{
  llvs::ConnectionToSot *aCST = (llvs::ConnectionToSot *)arg;

  if (aCST!=0)
    {
      while(!aCST->GetEndOfThreadLoop())
	{
#if 0
	  double waistposition[3];
	  double waistattitude[3];
	    
	  aCST->ReadWaistSignals(waistposition,
				 waistattitude);

	  usleep(23000);
	  ODEBUG3("Starting again. ( " 
		 << waistposition[0] << " , "
		 << waistposition[1] << " , "
		 << waistposition[2] << " ) ( "
		 << waistattitude[0] << " , "
		 << waistattitude[1] << " , "
		 << waistattitude[2] << " ) "); 
#else
	  double headprpy[6];
	  double waistprpy[6];

	  aCST->ReadHeadRPYSignals(headprpy);
	  aCST->ReadWaistRPYSignals(waistprpy);

	  usleep(23000);
#if 0
	  ODEBUG3("headprpy ( " 
		 << headprpy[0] << " , "
		 << headprpy[1] << " , "
		 << headprpy[2] << " ) ( "
		 << headprpy[3] << " , "
		 << headprpy[4] << " , "
		 << headprpy[5] << " ) "); 
	  
	  ODEBUG3("waistprpy ( " 
		 << waistprpy[0] << " , "
		 << waistprpy[1] << " , "
		 << waistprpy[2] << " ) ( "
		 << waistprpy[3] << " , "
		 << waistprpy[4] << " , "
		 << waistprpy[5] << " ) "); 
#endif	  

#endif
	}
    }
  ODEBUG("Went out of the thread.");
  return 0;
}

using namespace llvs;

ConnectionToSot::ConnectionToSot(LowLevelVisionServer *aLLVS)
{
  m_LLVS = aLLVS;
  m_EndOfThreadLoop = false;

  m_CircularBuffer = new double[33*7*3600];
  if (m_CircularBuffer!=0)
    ODEBUG3("Mem alloc done.");

  m_CircularBufferIndex = 0;
  m_CircularBufferIndexMax = 33*3600;
}

void ConnectionToSot::DumpCircularBuffer(string aFilename)
{
  ODEBUG("Dumping data: "<<m_CircularBufferIndexMax);
  ofstream aof;
  aof.open((char *)aFilename.c_str(),ofstream::out);
  aof.precision(40);
  for(unsigned int i=0;i<m_CircularBufferIndexMax;i++)
    {
      aof << m_CircularBuffer[i]<< " ";
      if ((i%7==0) &&(i!=0))
	aof <<endl;
    }
  aof.close();
}

ConnectionToSot::~ConnectionToSot()
{
  delete [] m_CircularBuffer ;

}

bool ConnectionToSot::GetEndOfThreadLoop() const 
{
  return m_EndOfThreadLoop;
}

void ConnectionToSot::StartThreadOnConnectionSot()
{
  pthread_t aThread;
  m_EndOfThreadLoop = false;
  /* Thread creation */
  {
    pthread_attr_t Thread_Attr;
    
    pthread_attr_init(&Thread_Attr);
    pthread_create(&aThread, &Thread_Attr,ConnectionToSotThread, (void *)this);
    
    ODEBUG("OmniORB thread:" << pthread_self());
  }

}

void ConnectionToSot::StopThreadOnConnectionSot()
{
  m_EndOfThreadLoop = true;
}


bool ConnectionToSot::SetCorbaReference()
{
  std::vector<std::string> lServiceName,lServiceKind;
  lServiceName.resize(2);
  lServiceKind.resize(2);
  lServiceName[0]= "sot";lServiceName[1]= "coshell";
  lServiceKind[0]="context";lServiceKind[1]="servant";
  
  CORBA::Object_ptr obj = m_LLVS->getObjectReference(lServiceName,lServiceKind);
  ODEBUG( "Able to get the reference for :" << lServiceName[0] << " " 
	  << lServiceKind[0] );

  if (CORBA::is_nil(obj))
   {
      cerr << "Unable to find object: " << lServiceName[0] << " " << lServiceKind[0] <<endl;
      return false;
    }

  ODEBUG( "Before narrowing");
  
  try
    {
      m_SOT_Server_Command = hppCorbaServer::SOT_Server_Command::_narrow(obj);
    }
  catch(...)
    {
       cerr << "Unable to narrow :" << lServiceName[0]
	   << " " << lServiceKind[0] 
	   << "CORBAReference : "<< m_SOT_Server_Command;
      return false;
    }

  if (CORBA::is_nil(m_SOT_Server_Command))
    {
      ODEBUG3("Unable to narrow :" << lServiceName[0]
	      << " " << lServiceKind[0] 
	      << "CORBAReference : "<< m_SOT_Server_Command);
      exit(0);
    }
  ODEBUG( "After narrowing");
  
  return true;
}

void ConnectionToSot::WriteVelocityReference(double velref[3])
{
  ODEBUG("Enter WriteVelocityReference ");

  try
    {
      struct timeval ats;
	    

      hppCorbaServer::DoubleSeq_var DSvelref;
      DSvelref = new hppCorbaServer::DoubleSeq;
      DSvelref->length(6);
      for(unsigned int li=0;li<3;li++)
	DSvelref[li]= velref[li];
      DSvelref._retn();
      
      m_SOT_Server_Command->writeOutputVectorSignal(m_VelRefSignalRank,
						    DSvelref);
      

    }
  catch(...)
    {
      cout << "Unable to connect to Sot. "<< endl;
      
    }
  ODEBUG("Go out of WriteVelocityReference ");

}

void ConnectionToSot::ReadWaistSignals(double waistposition[3],
				       double waistattitude[3])
{
  ODEBUG("Enter ReadWaistSignals ");

  try
    {
      struct timeval ats;
	    

      hppCorbaServer::DoubleSeq_var DSwaistpos, DSwaistatt;
      m_SOT_Server_Command->readInputVectorSignal(m_WaistPositionSignalRank,
						  DSwaistpos);
      
      m_SOT_Server_Command->readInputVectorSignal(m_WaistAttitudeSignalRank,
						  DSwaistatt);

      gettimeofday(&ats,0);
      m_CircularBuffer[m_CircularBufferIndex++] = (double)ats.tv_sec + 0.000001 * (double)ats.tv_usec;


      if (DSwaistpos->length()==3)
	{
	  for(unsigned li=0;li<3;li++)
	    {
	      waistposition[li] = 
		m_CircularBuffer[m_CircularBufferIndex++] = DSwaistpos[li];
	    }
	}

      if (DSwaistatt->length()==3)
	{
	  for(unsigned li=0;li<3;li++)
	    {
	      waistattitude[li] = 
		m_CircularBuffer[m_CircularBufferIndex++] = DSwaistatt[li];
	    }
	}

     
      if (m_CircularBufferIndex>=m_CircularBufferIndexMax)
	m_CircularBufferIndex = 0;

    }
  catch(...)
    {
      cout << "Unable to connect to Sot. "<< endl;
      
    }
  ODEBUG("Go out of  ReadWaistSignals ");
}

void ConnectionToSot::ReadHeadRPYSignals(double headposerpy[6])
{
  ODEBUG("Enter ReadWaistSignals ");

  try
    {
      struct timeval ats;
	    

      hppCorbaServer::DoubleSeq_var DShead;
      m_SOT_Server_Command->readInputVectorSignal(m_HeadPRPYSignalRank,
						  DShead);
      
      if (DShead->length()==6)
	for(unsigned int li=0;li<6;li++)
	  headposerpy[li]= DShead[li];
    }
  catch(...)
    {
      cout << "Unable to connect to Sot. "<< endl;
      
    }
  ODEBUG("Go out of  ReadHeadSignals ");
}


void ConnectionToSot::ReadWaistRPYSignals(double waistposerpy[6])
{
  ODEBUG("Enter ReadWaistSignals ");

  try
    {
      struct timeval ats;
	    

      hppCorbaServer::DoubleSeq_var DSwaist;
      m_SOT_Server_Command->readInputVectorSignal(m_WaistPRPYSignalRank,
						  DSwaist);
      
      if (DSwaist->length()==6)
	for(unsigned int li=0;li<6;li++)
	  waistposerpy[li]= DSwaist[li];
    }
  catch(...)
    {
      cout << "Unable to connect to Sot. "<< endl;
      
    }
  ODEBUG("Go out of  ReadHeadSignals ");
}


bool ConnectionToSot::Init()
{
  bool status;
  status = SetCorbaReference();

  ODEBUG("Status: " << status);
  if (!status)
    return status;

  ODEBUG("Test");
  if( CORBA::is_nil(m_SOT_Server_Command) ) 
    {
      cerr << "Failed to narrow the root naming context." << endl;
      return false;
    }

  string CstSignaux[4]={"waistpositionabsolute",
			"waistattitudeabsolute",
			"Head",
			"Waist"};

  string OutSignal[1] = {"VelRef"};
  ODEBUG("Before creating the signals: " << status);
  for(unsigned int li=0;li<4;li++)
    {

      /*      
      aCS->length(CstSignaux[li].size());
      for(unsigned int j=0;j<CstSignaux[li].size();j++)
      aCS[j] = CstSignaux[li][j];*/

      try{
	if (li==0)
	  m_WaistPositionSignalRank = m_SOT_Server_Command->createInputVectorSignal(CstSignaux[li].c_str());
	else if (li==1)
	  m_WaistAttitudeSignalRank = m_SOT_Server_Command->createInputVectorSignal(CstSignaux[li].c_str());
	else if (li==2)
	  m_HeadPRPYSignalRank = m_SOT_Server_Command->createInputVectorSignal(CstSignaux[li].c_str());
	else if (li==3)
	  m_WaistPRPYSignalRank = m_SOT_Server_Command->createInputVectorSignal(CstSignaux[li].c_str());

	m_VelRefSignalRank = m_SOT_Server_Command->createOutputVectorSignal(OutSignal[0].c_str());

      }
      catch(...)
	{
	  cerr << "Tried to create signal " << CstSignaux[li] << endl;
	  exit(-1);
	}

      ODEBUG("Creation of signals: " << CstSignaux[li]);
    }
  ODEBUG("After creating the signals: " );
  
#if 0
#define NBCOMMANDS 4
  string SotCommand[4]= {
    "plug pg.waistpositionabsolute coshell.waistpositionabsolute",
    "plug pg.waistattitudeabsolute coshell.waistattitudeabsolute",
    "OpenHRP.periodicCall addSignal pg.waistpositionabsolute",
    "OpenHRP.periodicCall addSignal pg.waistattitudeabsolute"};
#else
#define NBCOMMANDS 11
  string SotCommand[11]= {
    "plug ffposition_from_pg.out coshell.waistpositionabsolute",
    "plug ffattitude_from_pg.out coshell.waistattitudeabsolute",
    "OpenHRP.periodicCall addSignal ffposition_from_pg.out",
    "OpenHRP.periodicCall addSignal ffattitude_from_pg.out",
    "new MatrixHomoToPoseRollPitchYaw dhhtp",
    "plug dyn.head dhhtp.in",
    "plug dhhtp.out coshell.Head",
    "new MatrixHomoToPoseRollPitchYaw dwhtp",
    "plug dyn.Waist dwhtp.in",
    "plug dwhtp.out coshell.Waist",
    "plug coshell.VelRef pg.velocitydes"
  };
#endif

  for(unsigned int li=0;li<NBCOMMANDS;li++)
    {

      try 
	{ 

	  hppCorbaServer::StringStreamer_var CoshellOutput;
	  m_SOT_Server_Command->runAndRead(SotCommand[li].c_str(),CoshellOutput); 
	  ODEBUG("Launched " << SotCommand[li].c_str());
	  string lans;
	  lans.resize(CoshellOutput->length());
	  for(unsigned int i=0;i<CoshellOutput->length();i++)
	    { lans[i]=CoshellOutput[i]; }
	  ODEBUG("Out " << lans);

	}
      catch(...)
	{
	  ODEBUG3("Unable to contact the sot server.");
	  return false;
	}
    }

  ODEBUG("Launched all the commands");
  
  return true;
}
