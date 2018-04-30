#include "AppDelegate.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SystemUtils.h"
#include "Pref.h"
#include "GameScene_2Balls.h"
#include "MainScene.h"
#include "GameScene_Cops.h"
#include "GameScene_PixelRun.h"
#include "GameScene_Mayhem.h"
#include "GameScene_SwipeToCircle.h"
#include "GameScene_HoldToWalk.h"
#include "GameScene_Shadow.h"
#include "GameScene_ZombieRun.h"
#include "GameScene_BuildNDestroy.h"
#include "GameScene_RunUp.h"
#include "GameScene_FlyGuy.h"
#include "GameScene_Cowboy.h"
#include "GameScene_DontShootYourself.h"
#include "GameScene_Earth.h"
#include "GameScene_Split.h"
#include "GameScene_FightLine.h"
#include "GameScene_ConnectTheDots.h"
#include "GameScene_FruitNinja.h"
#include "GameScene_MapPuzzle.h"
#include "GameScene_3Sticks.h"
#include "GameScene_Map2PlayersFight.h"
#include "GameScene_MrRun.h"
#include "GameScene_TilePatternMatch.h"
#include "GameScene_Rumble3D.h"
#include "GameScene_Rope.h"

using namespace cocos2d;
using namespace CocosDenshion;

typedef struct tagResource
{
    cocos2d::CCSize size;
    char directory[100];
} Resource;

static Resource smallResource = { cocos2d::CCSizeMake(640, 960), "iphone" };
//static Resource smallResource = { cocos2d::CCSizeMake(960, 640), "iphone" };
static cocos2d::CCSize designResolutionSize = smallResource.size;

AppDelegate::AppDelegate(SystemUtils * pSystemUtils)
{
	SystemUtils::initInstance(pSystemUtils);
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }
    
	// initialize director
    CCDirector* pDirector = director;//CCDirector::getInstance();
    CCEGLView* pEGLView = pDirector->getOpenGLView();
    pDirector->setOpenGLView(pEGLView);
    
	// set FPS. the default value is 1.0/60 if you don't call this
	pDirector->setAnimationInterval(1.0 / 60);
	
    // Set the design resolution
	pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionFixedWidth);
    
    CCSize frameSize = pEGLView->getFrameSize();

    std::vector<std::string> resDirOrders;
    resDirOrders.push_back(smallResource.directory);
    CCFileUtils::sharedFileUtils()->setSearchResolutionsOrder(resDirOrders);
    
    pDirector->setContentScaleFactor(1);//designResolutionSize.height / frameSize.height);
    
    //CCScene *pScene = MainScene::scene();// GameScene_2Balls::scene(1);
    CCScene *pScene = GameScene_2Balls::scene(1);
    
    // run
    pDirector->runWithScene(pScene);
     	
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->pause();
    
    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->resume();
    
    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
