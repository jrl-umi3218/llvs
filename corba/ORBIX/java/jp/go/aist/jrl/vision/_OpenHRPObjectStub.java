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
// IDL:OpenHRPObject:1.0
//
public class _OpenHRPObjectStub extends org.omg.CORBA.portable.ObjectImpl
                                implements OpenHRPObject
{
    private static final String[] _ob_ids_ =
    {
        "IDL:OpenHRPObject:1.0",
    };

    public String[]
    _ids()
    {
        return _ob_ids_;
    }

    //
    // IDL:OpenHRPObject/shutdown:1.0
    //
    public void
    shutdown()
    {
        org.omg.CORBA.portable.Delegate _ob_delegate = _get_delegate();
        while(true)
        {
            synchronized(_ob_delegate)
            {
                org.omg.CORBA.portable.OutputStream out = null;
                org.omg.CORBA.portable.InputStream in = null;
                try
                {
                    out = _request("shutdown", false);
                    in = _invoke(out);
                    return;
                }
                catch(org.omg.CORBA.portable.RemarshalException _ob_ex)
                {
                    continue;
                }
                catch(org.omg.CORBA.portable.ApplicationException _ob_aex)
                {
                    final String _ob_id = _ob_aex.getId();
                    in = _ob_aex.getInputStream();

                    throw new org.omg.CORBA.UNKNOWN("Unexpected User Exception: " + _ob_id);
                }
            }
        }
    }
}
