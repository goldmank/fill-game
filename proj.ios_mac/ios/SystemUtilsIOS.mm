#include "SystemUtilsIOS.h"
#include "cocos2d.h"
#import "AppController.h"
#include "Pref.h"

// ===========================================================
// Initialization & cleanup
// ===========================================================

SystemUtilsIOS::SystemUtilsIOS()
{
    m_bShowingLoading = false;
    m_pLoadingView = NULL;
    m_pLoading = NULL;
}

SystemUtilsIOS::~SystemUtilsIOS()
{
    
}

// ===========================================================
// Public methods
// ===========================================================

void SystemUtilsIOS::showLeaderboard()
{
    CCLOG("showLeaderboard");
}

void SystemUtilsIOS::sendScore(int score)
{
    CCLOG("sendScore");
}

void SystemUtilsIOS::shareScore(int score)
{
}

void SystemUtilsIOS::purchase(int count)
{
}

void SystemUtilsIOS::rateUs()
{
}

void SystemUtilsIOS::onExit()
{
}

void SystemUtilsIOS::trackPage(const char * page)
{
}

void SystemUtilsIOS::trackEvent(const char * event, const char * paramName, const char * paramValue)
{
}

void SystemUtilsIOS::trackTimedEvent(const char * event)
{
}

void SystemUtilsIOS::showAd()
{
}

void SystemUtilsIOS::hideAd()
{    
}

void SystemUtilsIOS::showLoadingScreen()
{
    if (m_bShowingLoading)
    {
        return;
    }
    
    m_bShowingLoading = true;
    
    cocos2d::CCDirector* pDirector = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCEGLView* pEGLView = pDirector->getOpenGLView();
    cocos2d::CCSize frameSize = pEGLView->getFrameSize();
    
    float w = frameSize.width;
    float h = frameSize.height;
    CCLOG("loading: %.2f, %.2f", w, h);
    UIView * loadingView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, w, h)];
    loadingView.backgroundColor = [UIColor colorWithWhite:0.5 alpha:0.5];
    UIActivityIndicatorView * loading = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    [loading setHidesWhenStopped:YES];
    [loadingView addSubview:loading];
    loading.frame = CGRectMake(320/2 - 37, 480/2 - 37, 74, 74);
        
    UIApplication* clientApp = [UIApplication sharedApplication];
    UIWindow* topWindow = [clientApp keyWindow];
    if (!topWindow)
    {
        topWindow = [[clientApp windows] objectAtIndex:0];
    }
    [topWindow addSubview:loadingView];
    
    [loading setHidden:NO];
    [loading startAnimating];
    
    m_pLoadingView = (void *)loadingView;
    m_pLoading = (void *)loading;
}

void SystemUtilsIOS::hideLoadingScreen()
{
    if (!m_bShowingLoading)
    {
        return;
    }
    
    UIView * loadingView = (UIView *)m_pLoadingView;
    UIActivityIndicatorView * loading = (UIActivityIndicatorView *)m_pLoading;
    [loading stopAnimating];
    [loadingView removeFromSuperview];
    
    m_bShowingLoading = false;
}