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
// IDL:dsequence:1.0
//
final public class dsequenceHolder implements org.omg.CORBA.portable.Streamable
{
    public double[] value;

    public
    dsequenceHolder()
    {
    }

    public
    dsequenceHolder(double[] initial)
    {
        value = initial;
    }

    public void
    _read(org.omg.CORBA.portable.InputStream in)
    {
        value = dsequenceHelper.read(in);
    }

    public void
    _write(org.omg.CORBA.portable.OutputStream out)
    {
        dsequenceHelper.write(out, value);
    }

    public org.omg.CORBA.TypeCode
    _type()
    {
        return dsequenceHelper.type();
    }
}
