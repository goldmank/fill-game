#ifndef __SYSTEM_UTILS__
#define __SYSTEM_UTILS__

#include <stdio.h>
#include <string>

// ===========================================================
// LeaderboardItem
// ===========================================================

struct LeaderboardItem
{
    std::string userId;
    std::string name;
    std::string imageURL;
    int score;
};

class SystemUtils
{
public:
    
	// ===========================================================
    // Singleton methods
    // ===========================================================

	static void initInstance(SystemUtils * pSystemUtils);
	static SystemUtils * getInstance();

	// =========================================================== 
    // Public methods
    // ===========================================================
	
	virtual void showLeaderboard() {}
	virtual void sendScore(int score) {}
    virtual void shareScore(int score) {}
	virtual void purchase(int count) {}
	virtual void rateUs() {}
	virtual void onExit() {}
	virtual void trackPage(const char * page) {}
    virtual void trackEvent(const char * event, const char * paramName, const char * paramValue) {}
    virtual void trackTimedEvent(const char * event) {}
    virtual void showAd() {}
    virtual void hideAd() {}
    virtual void showLoadingScreen() {}
	virtual void hideLoadingScreen() {}
    
protected:

	// =========================================================== 
    // Constructor
    // ===========================================================

	SystemUtils();

private:

	static SystemUtils * s_pInstance;
};

#endif // __SYSTEM_UTILS__
