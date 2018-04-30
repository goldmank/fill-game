/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package com.flambo.fill;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;

public class Fill extends Cocos2dxActivity
{	
	// ===========================================================
    // Fields
    // ===========================================================
	
	private static Fill sInstance;
	private FrameLayout mBannerAdLayout;
	private FrameLayout mLoadingOverlay;
	private boolean mStopped;
	
	// ===========================================================
    // Activity methods
    // ===========================================================
	
	@Override
    protected void onCreate(Bundle savedInstanceState)
    {
		super.onCreate(savedInstanceState);
		sInstance = this;		
	}
	
	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		sInstance = null;
	}
	
	@Override
	protected void onStart()
	{
		super.onStart();		
		mStopped = false;
	}
	 
	@Override
	protected void onStop()
	{
		super.onStop();		
		mStopped = true;
	}
    
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) 
	{
		super.onActivityResult(requestCode, resultCode, data);
	}
    	
    // ===========================================================
    // Cocos2dxActivity methods
    // ===========================================================
    
	@Override
    public Cocos2dxGLSurfaceView onCreateView()
    {
    	Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
    	// Fill should create stencil buffer
    	glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
    	
    	return glSurfaceView;
    }
			
	@Override
	protected void handleContainerInit(FrameLayout container)
	{
		LayoutInflater inflater = LayoutInflater.from(this);
		View overlay = inflater.inflate(R.layout.overlay, null);
		
		mBannerAdLayout = (FrameLayout)overlay.findViewById(R.id.banner);
		
		mLoadingOverlay = (FrameLayout)overlay.findViewById(R.id.loading);
		mLoadingOverlay.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				
			}
		});
		
		container.addView(overlay);
	}
	
	// ===========================================================
    // Static methods
    // ===========================================================
	
	public static void showLeaderboard(int currScore)
	{
		if (null != sInstance)
		{
		}
	}
	
	public static void sendScore(int score)
	{
		if (null != sInstance)
		{			
		}
	}
		
	public static void purchase(int count)
	{		
	}
	
	public static void shareScore(int score)
	{
		if (null != sInstance)
		{
			sInstance.connectAndShare(score);
		}
	}
		
	public static void rateUs()
	{
		if (null != sInstance)
		{
			try {
				Intent intent = new Intent(Intent.ACTION_VIEW);
				intent.setData(Uri.parse("market://details?id=" + sInstance.getPackageName()));
				intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				sInstance.startActivity(intent);
			} catch (Exception e) {}	
		}		
	}
	
	public static void onExit()
	{
		if (null != sInstance)
		{
			sInstance.finish();
		}
	}
	
	public static void trackPage(String page)
	{
	}
	
	public static void trackEvent(String page, String paramName, String paramValue)
	{
	}
	
	public static void trackTimedEvent(String event)
	{
	}
	
	public static void showAd()
	{	
	}
	
	public static void hideAd()
	{		
	}
	
	public static void showLoadingScreen()
	{
		if (null != sInstance)
		{
			sInstance.runOnUiThread(new Runnable() {
				
				@Override
				public void run() {
					if (View.VISIBLE == sInstance.mLoadingOverlay.getVisibility()) 
					{
						return;
					}
					
					sInstance.mLoadingOverlay.setVisibility(View.VISIBLE);		
				}
			});								
		}
	}
	
	public static void hideLoadingScreen()
	{
		if (null != sInstance)
		{
			sInstance.runOnUiThread(new Runnable() {
				
				@Override
				public void run() {
					if (View.VISIBLE != sInstance.mLoadingOverlay.getVisibility()) 
					{
						return;
					}
					
					sInstance.mLoadingOverlay.setVisibility(View.GONE);		
				}
			});	
			
		}
	}
	
	// ===========================================================
    // Private methods
    // ===========================================================
		
	private ViewGroup getBannerAdLayout() 
	{
		return mBannerAdLayout;
	}
	
	private void connectAndShare(int score)
	{
		try {
    		Logger.l("share");
			Intent sendIntent = new Intent();
			sendIntent.setAction(Intent.ACTION_SEND);
			sendIntent.putExtra(Intent.EXTRA_SUBJECT, "Invite to Block Rush");
			sendIntent.putExtra(Intent.EXTRA_EMAIL, new String[] {});
			sendIntent.putExtra(Intent.EXTRA_TEXT, "Hey, I Just scored " + score + " in Block Rush. Think you can beat it?!  http://www.flambostudios.com/blocks/");
			sendIntent.setType("text/plain");
			startActivity(sendIntent);
		} catch (Exception e) {}
	}
		
	// ===========================================================
    // Library initialization
    // ===========================================================
    
	private static native void nativeLoadingLeaderboardDone(String[] array);
	
    static {
        System.loadLibrary("cocos2dcpp");
    }
}
