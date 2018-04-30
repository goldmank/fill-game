#include "GameScene_Rumble3D.h"
#include "SimpleAudioEngine.h"
#include "Pref.h"
#include "SystemUtils.h"

using namespace cocos2d;
using namespace CocosDenshion;

// ===========================================================
// Constants
// ===========================================================

static const char * fontList[] =
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC))
    "Simply Square JL",
#else
    "fonts/SimplySquare.ttf",
#endif
};

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Rumble3D::GameScene_Rumble3D()
{
    srand(time(NULL));
}

GameScene_Rumble3D::~GameScene_Rumble3D()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Rumble3D::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Rumble3D *layer = GameScene_Rumble3D::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Rumble3D::handleInit(float screenWidth, float screenHeight)
{
    auto s = Director::getInstance()->getWinSize();
    
    auto sprite = Sprite3D::create("StickFigurea.obj");
    sprite->setScale(5.f);
    sprite->setPosition(screenWidth / 2, screenHeight / 2);
    addChild(sprite);

    
//    auto animation = Animation3D::create("orc.c3b");
//    if (animation)
//    {
//        auto animate = Animate3D::create(animation);
//        sprite->runAction(RepeatForever::create(animate));
//    }
}

void GameScene_Rumble3D::handleUpdate(float dt)
{
    
}

bool GameScene_Rumble3D::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    return true;
}

void GameScene_Rumble3D::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
}

void GameScene_Rumble3D::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
}

void GameScene_Rumble3D::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================
