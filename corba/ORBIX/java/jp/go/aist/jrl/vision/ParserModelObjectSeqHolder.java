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
// IDL:ParserModelObjectSeq:1.0
//
final public class ParserModelObjectSeqHolder implements org.omg.CORBA.portable.Streamable
{
    public ParserModelObject[] value;

    public
    ParserModelObjectSeqHolder()
    {
    }

    public
    ParserModelObjectSeqHolder(ParserModelObject[] initial)
    {
        value = initial;
    }

    public void
    _read(org.omg.CORBA.portable.InputStream in)
    {
        value = ParserModelObjectSeqHelper.read(in);
    }

    public void
    _write(org.omg.CORBA.portable.OutputStream out)
    {
        ParserModelObjectSeqHelper.write(out, value);
    }

    public org.omg.CORBA.TypeCode
    _type()
    {
        return ParserModelObjectSeqHelper.type();
    }
}
