package com.flambo.fill;


import android.util.Log;

public class Logger
{
	// =========================================================== 
    // Constants
    // ===========================================================
	
	public static final String TAG = "blocks";
	public static final boolean DEBUG = false;
	private static final int SEVERITY = 5;//App.DEBUG_ALARMS ? 5 : 3;
	
	// =========================================================== 
    // Public methods
    // ===========================================================	
	
	/**
	 * Print the message sent to the logger only if the flag DEBUG is set to true.
	 * The message can be 130 char's long - longer message will be cut after 130 char's
	 * 
	 * @param tag - the tag in the logcat
	 * @param severity - level of message we want to show
	 * @param str - message to display, null string will not print a message
	 */
	public static void l(String tag, int severity, String str)
	{
		if (DEBUG && severity <= SEVERITY)
		{
			if (null == str)
			{
				return;
			}
			
			int startIndex = 0;
			while (startIndex < str.length())
			{
				int endIndex = startIndex + 130;
				if (endIndex >= str.length())
				{
					endIndex = str.length();
				}
				String logTag = TAG;
				if (null != tag)
				{
					logTag += "-" + tag;
				}
				Log.i(logTag, str.substring(startIndex, endIndex));
				
				startIndex = endIndex;
			}			
		}	
	}
	
	/**
	 * Printing with out the tag name, this will make the tag = Logger.TAG
	 * 
	 * @param severity - level of message we want to show
	 * @param str - message to display, null string will not print a message
	 */
	public static void l(int severity, String str)
	{
		l(null, severity, str);
	}
	
	/**
	 * Printing with out the tag name, this will make the tag = Logger.TAG
	 * and the severity to be set by the Logger. The message will be displayed considering the default severity
	 * 
	 * @param str - message to display, null string will not print a message
	 */
	public static void l(String str)
	{						
		l(0, str);
	}
	
	/** 
	 * Printing the exceptions this uses the Logger.l function. Each element in the exceptions is printed seperatly
	 * @param e
	 */
	public static void l(Exception e)
	{
		if (!DEBUG)
		{
			return;
		}
		
		Logger.l(e.toString());
		StackTraceElement[] elements = e.getStackTrace();
		if (null == elements)
		{
			return;
		}
		
		for(StackTraceElement element : elements)
		{
			if (null != element)
			{
				l(element.toString());
			}
		}
	}
}
