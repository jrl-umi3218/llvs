// **********************************************************************
//
// Generated by the Orbix/E IDL to Java Translator
//
// Copyright (c) 2003
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

// Version: 2.0.1

package jp.go.aist.jrl.vision;

//
// IDL:ParserModelObject:1.0
//
/**
 * ParserModelObjectインタフェース定義
 * @author  Ichitaro Kohara, MSTC
 * @date    2001.01.06
 * @version 1.0
 * @version 1.1
 **/

public interface ParserModelObjectOperations
{
    //
    // IDL:ParserModelObject/positionGiven:1.0
    //
    /***/

    boolean
    positionGiven();

    //
    // IDL:ParserModelObject/rotateJoint:1.0
    //
    /***/

    boolean
    rotateJoint();

    //
    // IDL:ParserModelObject/mass:1.0
    //
    /***/

    double
    mass();

    //
    // IDL:ParserModelObject/jointValue:1.0
    //
    /***/

    double
    jointValue();

    //
    // IDL:ParserModelObject/jointVel:1.0
    //
    /***/

    double
    jointVel();

    //
    // IDL:ParserModelObject/jointAcc:1.0
    //
    /***/

    double
    jointAcc();

    //
    // IDL:ParserModelObject/absPos:1.0
    //
    /***/

    double[]
    absPos();

    //
    // IDL:ParserModelObject/relPos:1.0
    //
    /***/

    double[]
    relPos();

    //
    // IDL:ParserModelObject/relComPos:1.0
    //
    /***/

    double[]
    relComPos();

    //
    // IDL:ParserModelObject/absComPos:1.0
    //
    /***/

    double[]
    absComPos();

    //
    // IDL:ParserModelObject/relComAcc:1.0
    //
    /***/

    double[]
    relComAcc();

    //
    // IDL:ParserModelObject/omega:1.0
    //
    /***/

    double[]
    omega();

    //
    // IDL:ParserModelObject/omega_d:1.0
    //
    /***/

    double[]
    omega_d();

    //
    // IDL:ParserModelObject/relAcc:1.0
    //
    /***/

    double[]
    relAcc();

    //
    // IDL:ParserModelObject/absVel:1.0
    //
    /***/

    double[]
    absVel();

    //
    // IDL:ParserModelObject/absComVel:1.0
    //
    /***/

    double[]
    absComVel();

    //
    // IDL:ParserModelObject/relForce:1.0
    //
    /***/

    double[]
    relForce();

    //
    // IDL:ParserModelObject/relTorque:1.0
    //
    /***/

    double[]
    relTorque();

    //
    // IDL:ParserModelObject/absAttitude:1.0
    //
    /***/

    double[]
    absAttitude();

    //
    // IDL:ParserModelObject/relAttitude:1.0
    //
    /***/

    double[]
    relAttitude();

    //
    // IDL:ParserModelObject/inertia:1.0
    //
    /***/

    double[]
    inertia();

    //
    // IDL:ParserModelObject/dh:1.0
    //
    /***/

    double[]
    dh();

    //
    // IDL:ParserModelObject/ulimit:1.0
    //
    /***/

    double[]
    ulimit();

    //
    // IDL:ParserModelObject/llimit:1.0
    //
    /***/

    double[]
    llimit();

    //
    // IDL:ParserModelObject/uvlimit:1.0
    //
    /***/

    double[]
    uvlimit();

    //
    // IDL:ParserModelObject/lvlimit:1.0
    //
    /***/

    double[]
    lvlimit();

    //
    // IDL:ParserModelObject/rotorInertia:1.0
    //
    /***/

    double
    rotorInertia();

    //
    // IDL:ParserModelObject/rotorResistor:1.0
    //
    /***/

    double
    rotorResistor();

    //
    // IDL:ParserModelObject/gearRatio:1.0
    //
    /***/

    double
    gearRatio();

    //
    // IDL:ParserModelObject/torqueConst:1.0
    //
    /***/

    double
    torqueConst();

    //
    // IDL:ParserModelObject/encoderPulse:1.0
    //
    /***/

    double
    encoderPulse();

    //
    // IDL:ParserModelObject/bboxCenter:1.0
    //
    /***/

    double[]
    bboxCenter();

    //
    // IDL:ParserModelObject/bboxSize:1.0
    //
    /***/

    double[]
    bboxSize();

    //
    // IDL:ParserModelObject/name:1.0
    //
    /***/

    String
    name();

    //
    // IDL:ParserModelObject/jointType:1.0
    //
    /***/

    String
    jointType();

    //
    // IDL:ParserModelObject/jointId:1.0
    //
    /***/

    short
    jointId();

    //
    // IDL:ParserModelObject/jointAxis:1.0
    //
    /***/

    String
    jointAxis();

    //
    // IDL:ParserModelObject/mother:1.0
    //
    /***/

    int
    mother();

    //
    // IDL:ParserModelObject/sister:1.0
    //
    /***/

    int
    sister();

    //
    // IDL:ParserModelObject/daughter:1.0
    //
    /***/

    int
    daughter();

    //
    // IDL:ParserModelObject/sensors:1.0
    //
    /***/

    SensorInfo[]
    sensors();
}
