#include "GameScene_Shadow.h"
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

enum {
    kTagTitleLabel = 1,
    kTagSubtitleLabel = 2,
    kTagStencilNode = 100,
    kTagClipperNode = 101,
    kTagContentNode = 102,
};

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Shadow::GameScene_Shadow()
{
    srand(time(NULL));
    m_bDone = false;
}

GameScene_Shadow::~GameScene_Shadow()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Shadow::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Shadow *layer = GameScene_Shadow::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Shadow::handleInit(float screenWidth, float screenHeight)
{
    auto s = Director::getInstance()->getWinSize();
    
    auto clipper = ClippingNode::create();
    clipper->setContentSize(Size(screenWidth, screenHeight));
    clipper->setAnchorPoint(Vec2(0.5, 0.5));
    clipper->setPosition( Vec2(screenWidth / 2, screenHeight / 2) );
    clipper->setAlphaThreshold(0.05f);
    addChild(clipper);
    
    auto stencil = Sprite::create("overlay.png");
    stencil->setAnchorPoint( Vec2(0.5, 0.5) );
    stencil->setPosition( Vec2(clipper->getContentSize().width / 2, clipper->getContentSize().height / 2) );
    stencil->setVisible(false);
    clipper->setStencil(stencil);
    
    clipper->addChild(stencil);
    
    m_pBg = Sprite::create("overlay.png");
    m_pBg->retain();
    
    m_pCircle = Sprite::create("circle.png");
    m_pCircle->retain();
    
}

void GameScene_Shadow::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
       
    }
}

bool GameScene_Shadow::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_pCircle->setPosition(pTouch->getLocation());
    
    return true;
}

void GameScene_Shadow::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    m_pCircle->setPosition(pTouch->getLocation());
}

void GameScene_Shadow::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
}

void GameScene_Shadow::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Shadow::onGameOver()
{
    m_bDone = true;
    
    DelayTime * pDelay = DelayTime::create(0.5f);
    CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_Shadow::restartGame));
    runAction(CCSequence::createWithTwoActions(pDelay, pFillDone));
}

void GameScene_Shadow::restartGame(cocos2d::Node * pSender)
{
    switchScene(GameScene_Shadow::scene());
}