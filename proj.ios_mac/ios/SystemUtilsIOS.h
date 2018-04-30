#ifndef __SYSTEM_UTILS_WIN32__
#define __SYSTEM_UTILS_WIN32__

#include "SystemUtils.h"

class SystemUtilsIOS : public SystemUtils
{
public:

    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    SystemUtilsIOS();
    ~SystemUtilsIOS();
    
	// =========================================================== 
    // Public methods
    // ===========================================================
	
	void showLeaderboard();
	void sendScore(int score);
    void shareScore(int score);
	void purchase(int count);
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
    // Fields
    // ===========================================================
    
    bool m_bShowingLoading;
    void * m_pLoadingView;
    void * m_pLoading;
};

#endif // __SYSTEM_UTILS_WIN32__
