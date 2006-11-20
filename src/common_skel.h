// **********************************************************************
//
// Generated by the Orbix/E IDL-to-C++ Translator
//
// Copyright (c) 2003
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

// Version: 2.2.0

#ifndef ___common_skel_h__
#define ___common_skel_h__

#include <common.h>

#ifndef OPENHRP_API
#   ifdef OB_DLL
#      define OPENHRP_API __declspec(dllimport)
#   else
#      define OPENHRP_API /**/
#   endif
#endif


#ifndef OBEC_INTEGER_VERSION
#   error No Orbix/E version defined! Is <OBE/CORBA.h> included?
#endif

#ifndef OBEC_NO_VERSION_CHECK
#   if (OBEC_INTEGER_VERSION != 2020000L)
#       error Orbix/E version mismatch!
#   endif
#endif

//
// IDL:SensorInfo:1.0
//
class OPENHRP_API POA_SensorInfo : virtual public PortableServer::ServantBase
{
    POA_SensorInfo(const POA_SensorInfo&);
    void operator=(const POA_SensorInfo&);

protected:

    //
    // IDL:SensorInfo/id:1.0
    //
    void
    _OB_get_id(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:SensorInfo/name:1.0
    //
    void
    _OB_get_name(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:SensorInfo/type:1.0
    //
    void
    _OB_get_type(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:SensorInfo/relPos:1.0
    //
    void
    _OB_get_relPos(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:SensorInfo/relAttitude:1.0
    //
    void
    _OB_get_relAttitude(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:SensorInfo/maxValue:1.0
    //
    void
    _OB_get_maxValue(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

public:

    POA_SensorInfo() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::SensorInfo_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:SensorInfo/id:1.0
    //
    virtual CORBA::Long
    id(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:SensorInfo/name:1.0
    //
    virtual char*
    name(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:SensorInfo/type:1.0
    //
    virtual SensorType
    type(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:SensorInfo/relPos:1.0
    //
    virtual ValueMatrix*
    relPos(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:SensorInfo/relAttitude:1.0
    //
    virtual ValueMatrix*
    relAttitude(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:SensorInfo/maxValue:1.0
    //
    virtual ValueMatrix*
    maxValue(
    )throw(CORBA::SystemException) = 0;
};

//
// IDL:ParserModelObject:1.0
//
class OPENHRP_API POA_ParserModelObject : virtual public PortableServer::ServantBase
{
    POA_ParserModelObject(const POA_ParserModelObject&);
    void operator=(const POA_ParserModelObject&);

protected:

    //
    // IDL:ParserModelObject/positionGiven:1.0
    //
    void
    _OB_get_positionGiven(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/rotateJoint:1.0
    //
    void
    _OB_get_rotateJoint(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/mass:1.0
    //
    void
    _OB_get_mass(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/jointValue:1.0
    //
    void
    _OB_get_jointValue(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/jointVel:1.0
    //
    void
    _OB_get_jointVel(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/jointAcc:1.0
    //
    void
    _OB_get_jointAcc(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/absPos:1.0
    //
    void
    _OB_get_absPos(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relPos:1.0
    //
    void
    _OB_get_relPos(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relComPos:1.0
    //
    void
    _OB_get_relComPos(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/absComPos:1.0
    //
    void
    _OB_get_absComPos(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relComAcc:1.0
    //
    void
    _OB_get_relComAcc(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/omega:1.0
    //
    void
    _OB_get_omega(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/omega_d:1.0
    //
    void
    _OB_get_omega_d(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relAcc:1.0
    //
    void
    _OB_get_relAcc(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/absVel:1.0
    //
    void
    _OB_get_absVel(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/absComVel:1.0
    //
    void
    _OB_get_absComVel(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relForce:1.0
    //
    void
    _OB_get_relForce(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relTorque:1.0
    //
    void
    _OB_get_relTorque(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/absAttitude:1.0
    //
    void
    _OB_get_absAttitude(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/relAttitude:1.0
    //
    void
    _OB_get_relAttitude(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/inertia:1.0
    //
    void
    _OB_get_inertia(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/dh:1.0
    //
    void
    _OB_get_dh(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/ulimit:1.0
    //
    void
    _OB_get_ulimit(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/llimit:1.0
    //
    void
    _OB_get_llimit(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/uvlimit:1.0
    //
    void
    _OB_get_uvlimit(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/lvlimit:1.0
    //
    void
    _OB_get_lvlimit(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/rotorInertia:1.0
    //
    void
    _OB_get_rotorInertia(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/rotorResistor:1.0
    //
    void
    _OB_get_rotorResistor(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/gearRatio:1.0
    //
    void
    _OB_get_gearRatio(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/torqueConst:1.0
    //
    void
    _OB_get_torqueConst(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/encoderPulse:1.0
    //
    void
    _OB_get_encoderPulse(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/bboxCenter:1.0
    //
    void
    _OB_get_bboxCenter(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/bboxSize:1.0
    //
    void
    _OB_get_bboxSize(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/name:1.0
    //
    void
    _OB_get_name(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/jointType:1.0
    //
    void
    _OB_get_jointType(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/jointId:1.0
    //
    void
    _OB_get_jointId(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/jointAxis:1.0
    //
    void
    _OB_get_jointAxis(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/mother:1.0
    //
    void
    _OB_get_mother(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/sister:1.0
    //
    void
    _OB_get_sister(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/daughter:1.0
    //
    void
    _OB_get_daughter(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/sensors:1.0
    //
    void
    _OB_get_sensors(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

public:

    POA_ParserModelObject() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::ParserModelObject_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserModelObject/positionGiven:1.0
    //
    virtual CORBA::Boolean
    positionGiven(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/rotateJoint:1.0
    //
    virtual CORBA::Boolean
    rotateJoint(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/mass:1.0
    //
    virtual CORBA::Double
    mass(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/jointValue:1.0
    //
    virtual CORBA::Double
    jointValue(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/jointVel:1.0
    //
    virtual CORBA::Double
    jointVel(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/jointAcc:1.0
    //
    virtual CORBA::Double
    jointAcc(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/absPos:1.0
    //
    virtual ValueMatrix*
    absPos(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relPos:1.0
    //
    virtual ValueMatrix*
    relPos(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relComPos:1.0
    //
    virtual ValueMatrix*
    relComPos(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/absComPos:1.0
    //
    virtual ValueMatrix*
    absComPos(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relComAcc:1.0
    //
    virtual ValueMatrix*
    relComAcc(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/omega:1.0
    //
    virtual ValueMatrix*
    omega(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/omega_d:1.0
    //
    virtual ValueMatrix*
    omega_d(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relAcc:1.0
    //
    virtual ValueMatrix*
    relAcc(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/absVel:1.0
    //
    virtual ValueMatrix*
    absVel(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/absComVel:1.0
    //
    virtual ValueMatrix*
    absComVel(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relForce:1.0
    //
    virtual ValueMatrix*
    relForce(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relTorque:1.0
    //
    virtual ValueMatrix*
    relTorque(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/absAttitude:1.0
    //
    virtual ValueMatrix*
    absAttitude(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/relAttitude:1.0
    //
    virtual ValueMatrix*
    relAttitude(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/inertia:1.0
    //
    virtual ValueMatrix*
    inertia(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/dh:1.0
    //
    virtual Value*
    dh(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/ulimit:1.0
    //
    virtual Value*
    ulimit(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/llimit:1.0
    //
    virtual Value*
    llimit(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/uvlimit:1.0
    //
    virtual Value*
    uvlimit(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/lvlimit:1.0
    //
    virtual Value*
    lvlimit(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/rotorInertia:1.0
    //
    virtual CORBA::Double
    rotorInertia(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/rotorResistor:1.0
    //
    virtual CORBA::Double
    rotorResistor(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/gearRatio:1.0
    //
    virtual CORBA::Double
    gearRatio(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/torqueConst:1.0
    //
    virtual CORBA::Double
    torqueConst(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/encoderPulse:1.0
    //
    virtual CORBA::Double
    encoderPulse(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/bboxCenter:1.0
    //
    virtual Value*
    bboxCenter(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/bboxSize:1.0
    //
    virtual Value*
    bboxSize(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/name:1.0
    //
    virtual char*
    name(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/jointType:1.0
    //
    virtual char*
    jointType(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/jointId:1.0
    //
    virtual CORBA::Short
    jointId(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/jointAxis:1.0
    //
    virtual char*
    jointAxis(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/mother:1.0
    //
    virtual CORBA::Long
    mother(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/sister:1.0
    //
    virtual CORBA::Long
    sister(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/daughter:1.0
    //
    virtual CORBA::Long
    daughter(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserModelObject/sensors:1.0
    //
    virtual SensorInfoSeq*
    sensors(
    )throw(CORBA::SystemException) = 0;
};

//
// IDL:ParserCharObject:1.0
//
class OPENHRP_API POA_ParserCharObject : virtual public PortableServer::ServantBase
{
    POA_ParserCharObject(const POA_ParserCharObject&);
    void operator=(const POA_ParserCharObject&);

protected:

    //
    // IDL:ParserCharObject/name:1.0
    //
    void
    _OB_get_name(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserCharObject/info:1.0
    //
    void
    _OB_get_info(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserCharObject/modelObjectSeq:1.0
    //
    void
    _OB_get_modelObjectSeq(
        OBEC_UpcallImpl* _ob_in,
        CORBA_Environment* _ob_cev
    );

public:

    POA_ParserCharObject() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::ParserCharObject_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ParserCharObject/name:1.0
    //
    virtual char*
    name(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserCharObject/info:1.0
    //
    virtual stringSeq*
    info(
    )throw(CORBA::SystemException) = 0;

    //
    // IDL:ParserCharObject/modelObjectSeq:1.0
    //
    virtual ParserModelObjectSeq*
    modelObjectSeq(
    )throw(CORBA::SystemException) = 0;
};

//
// IDL:ModelInfo:1.0
//
class OPENHRP_API POA_ModelInfo : virtual public PortableServer::ServantBase
{
    POA_ModelInfo(const POA_ModelInfo&);
    void operator=(const POA_ModelInfo&);

protected:

    //
    // IDL:ModelInfo/getUrl:1.0
    //
    void
    _OB_op_getUrl(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ModelInfo/getCharObject:1.0
    //
    void
    _OB_op_getCharObject(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ModelInfo/getJointList:1.0
    //
    void
    _OB_op_getJointList(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ModelInfo/readTriangles:1.0
    //
    void
    _OB_op_readTriangles(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ModelInfo/destroy:1.0
    //
    void
    _OB_op_destroy(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

public:

    POA_ModelInfo() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::ModelInfo_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:ModelInfo/getUrl:1.0
    //
    virtual char* getUrl(
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:ModelInfo/getCharObject:1.0
    //
    virtual ::ParserCharObject_ptr getCharObject(
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:ModelInfo/getJointList:1.0
    //
    virtual ::ModelInfo::NameSeq* getJointList(
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:ModelInfo/readTriangles:1.0
    //
    virtual ::ModelInfo::floatSeq* readTriangles(
        const char* jointName,
        CORBA::Long count
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:ModelInfo/destroy:1.0
    //
    virtual void destroy(
    ) throw(CORBA::SystemException) = 0;
};

//
// IDL:OpenHRPObject:1.0
//
class OPENHRP_API POA_OpenHRPObject : virtual public PortableServer::ServantBase
{
    POA_OpenHRPObject(const POA_OpenHRPObject&);
    void operator=(const POA_OpenHRPObject&);

protected:

    //
    // IDL:OpenHRPObject/shutdown:1.0
    //
    void
    _OB_op_shutdown(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

public:

    POA_OpenHRPObject() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::OpenHRPObject_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:OpenHRPObject/shutdown:1.0
    //
    virtual void shutdown(
    ) throw(CORBA::SystemException) = 0;
};

//
// IDL:CommandReceiver:1.0
//
class OPENHRP_API POA_CommandReceiver : virtual public PortableServer::ServantBase
{
    POA_CommandReceiver(const POA_CommandReceiver&);
    void operator=(const POA_CommandReceiver&);

protected:

    //
    // IDL:CommandReceiver/sendMsg:1.0
    //
    void
    _OB_op_sendMsg(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

public:

    POA_CommandReceiver() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::CommandReceiver_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:CommandReceiver/sendMsg:1.0
    //
    virtual void sendMsg(
        const char* msg
    ) throw(CORBA::SystemException) = 0;
};

//
// IDL:Plugin:1.0
//
class OPENHRP_API POA_Plugin : virtual public POA_CommandReceiver
{
    POA_Plugin(const POA_Plugin&);
    void operator=(const POA_Plugin&);

protected:

    //
    // IDL:Plugin/start:1.0
    //
    void
    _OB_op_start(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:Plugin/stop:1.0
    //
    void
    _OB_op_stop(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

public:

    POA_Plugin() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::Plugin_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:Plugin/start:1.0
    //
    virtual void start(
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:Plugin/stop:1.0
    //
    virtual void stop(
    ) throw(CORBA::SystemException) = 0;
};

//
// IDL:PluginManager:1.0
//
class OPENHRP_API POA_PluginManager : virtual public POA_CommandReceiver
{
    POA_PluginManager(const POA_PluginManager&);
    void operator=(const POA_PluginManager&);

protected:

    //
    // IDL:PluginManager/load:1.0
    //
    void
    _OB_op_load(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:PluginManager/create:1.0
    //
    void
    _OB_op_create(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:PluginManager/getPluginNames:1.0
    //
    void
    _OB_op_getPluginNames(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

public:

    POA_PluginManager() { }

    virtual CORBA::Boolean
    _is_a(
        const char* _ob_repid
    ) throw(CORBA::SystemException);

    virtual CORBA::RepositoryId
    _primary_interface(
        const PortableServer::ObjectId& _ob_oid,
        PortableServer::POA_ptr _ob_poa
    );

    ::PluginManager_ptr
    _this(
    );

    virtual void
    _OB_dispatch(
        OBEC_UpcallImpl* _ob_up,
        CORBA_Environment* _ob_cev
    );

    //
    // IDL:PluginManager/load:1.0
    //
    virtual void load(
        const char* name
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:PluginManager/create:1.0
    //
    virtual ::Plugin_ptr create(
        const char* pclass,
        const char* name,
        const char* args
    ) throw(CORBA::SystemException) = 0;

    //
    // IDL:PluginManager/getPluginNames:1.0
    //
    virtual ::stringSeq* getPluginNames(
    ) throw(CORBA::SystemException) = 0;
};

#endif
