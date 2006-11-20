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
// IDL:LinkData:1.0
//
final public class LinkDataHelper
{
    public static void
    insert(org.omg.CORBA.Any any, double[] val)
    {
        org.omg.CORBA.portable.OutputStream out = any.create_output_stream();
        write(out, val);
        any.read_value(out.create_input_stream(), type());
    }

    public static double[]
    extract(org.omg.CORBA.Any any)
    {
        if(any.type().equivalent(type()))
            return read(any.create_input_stream());
        else
            throw new org.omg.CORBA.BAD_OPERATION();
    }

    private static org.omg.CORBA.TypeCode typeCode_;

    public static org.omg.CORBA.TypeCode
    type()
    {
        if(typeCode_ == null)
        {
            org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init();
            typeCode_ = orb.create_alias_tc(id(), "LinkData", orb.create_array_tc(3, orb.get_primitive_tc(org.omg.CORBA.TCKind.tk_double)));
        }

        return typeCode_;
    }

    public static String
    id()
    {
        return "IDL:LinkData:1.0";
    }

    public static double[]
    read(org.omg.CORBA.portable.InputStream in)
    {
        double[] _ob_v;
        int len0 = 3;
        _ob_v = new double[len0];
        in.read_double_array(_ob_v, 0, len0);
        return _ob_v;
    }

    public static void
    write(org.omg.CORBA.portable.OutputStream out, double[] val)
    {
        int len0 = val.length;
        if(len0 != 3)
             throw new org.omg.CORBA.MARSHAL();
        out.write_double_array(val, 0, len0);
    }
}
