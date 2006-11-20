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
// IDL:PluginManager:1.0
//
public class _PluginManagerStub extends org.omg.CORBA.portable.ObjectImpl
                                implements PluginManager
{
    private static final String[] _ob_ids_ =
    {
        "IDL:PluginManager:1.0",
        "IDL:CommandReceiver:1.0"
    };

    public String[]
    _ids()
    {
        return _ob_ids_;
    }

    //
    // IDL:PluginManager/load:1.0
    //
    public void
    load(String _ob_a0)
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
                    out = _request("load", true);
                    out.write_string(_ob_a0);
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

    //
    // IDL:PluginManager/create:1.0
    //
    public Plugin
    create(String _ob_a0,
           String _ob_a1,
           String _ob_a2)
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
                    out = _request("create", true);
                    out.write_string(_ob_a0);
                    out.write_string(_ob_a1);
                    out.write_string(_ob_a2);
                    in = _invoke(out);
                    Plugin _ob_r = PluginHelper.read(in);
                    return _ob_r;
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

    //
    // IDL:CommandReceiver/sendMsg:1.0
    //
    public void
    sendMsg(String _ob_a0)
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
                    out = _request("sendMsg", true);
                    out.write_string(_ob_a0);
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
