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
// IDL:CBREP_Segment:1.0
//
final public class CBREP_SegmentHolder implements org.omg.CORBA.portable.Streamable
{
    public CBREP_Segment value;

    public
    CBREP_SegmentHolder()
    {
    }

    public
    CBREP_SegmentHolder(CBREP_Segment initial)
    {
        value = initial;
    }

    public void
    _read(org.omg.CORBA.portable.InputStream in)
    {
        value = CBREP_SegmentHelper.read(in);
    }

    public void
    _write(org.omg.CORBA.portable.OutputStream out)
    {
        CBREP_SegmentHelper.write(out, value);
    }

    public org.omg.CORBA.TypeCode
    _type()
    {
        return CBREP_SegmentHelper.type();
    }
}
