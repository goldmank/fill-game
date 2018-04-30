#ifndef __SYSTEM_UTILS_ANDROID__
#define __SYSTEM_UTILS_ANDROID__

#include "../../../Classes/SystemUtils.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

class SystemUtilsAndroid : public SystemUtils
{
public:

	// =========================================================== 
    // Constructor
    // ===========================================================

	SystemUtilsAndroid();

	// =========================================================== 
    // Public methods
    // ===========================================================
	
	void showLeaderboard();
	void sendScore(int score);
	void purchase(int count);
	void shareScore(int score);
	void rateUs();
	void onExit();
	void trackPage(const char * page);
    void trackEvent(const char * event, const char * paramName, const char * paramValue);
    void trackTimedEvent(const char * event);
    void showAd();
    void hideAd();
    void showLoadingScreen();
	void hideLoadingScreen();
	
private:

	// =========================================================== 
    // Private methods
    // ===========================================================
	
	void runVoidMethod(const char * methodName);
	int runIntMethod(const char * methodName);
	void runIntParamMethod(const char * methodName, int param);
	void runMultiIntParamMethod(const char * methodName, int param1, int param2, int param3);
};

#endif // __SYSTEM_UTILS_ANDROID__
