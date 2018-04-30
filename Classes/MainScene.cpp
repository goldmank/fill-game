#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "Pref.h"
#include "SystemUtils.h"
#include "GameScene_2Balls.h"

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

#define TAG_TIMED 1
#define TAG_MULITPLAYER 2
#define TAG_BLOCKED 3
#define TAG_ENDLESS 4

// ===========================================================
// Initialization & cleanup
// ===========================================================

MainScene::MainScene()
{
    srand(time(NULL));
}

MainScene::~MainScene()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* MainScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    MainScene *layer = MainScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void MainScene::handleInit(float screenWidth, float screenHeight)
{
    Sprite * pLogo = Sprite::create("logo.png");
    pLogo->setPosition(ccp(screenWidth / 2, screenHeight * 0.85f));
    addChild(pLogo);
    
    LayerColor * pDiv1 = LayerColor::create(ccc4(0, 0, 0, 200), screenWidth * 0.8f, screenWidth * 0.001f);
    pDiv1->setPosition(getScreenWidth() / 2 - pDiv1->boundingBox().size.width / 2, getScreenHeight() * 0.72f);
    addChild(pDiv1);
    
    Color3B c = Pref::getInstance()->getColor(COLOR_FILL);
    
    float btnY = getScreenHeight() * 0.49f;
    float btnSize = getScreenWidth() * 0.28f;
    float offset = btnSize * 0.16f;
    addButton(pDiv1->getPositionX() + offset, btnY, "ic_action_users.png", "Timed", btnSize, TAG_TIMED);
    addButton(pDiv1->getPositionX() + pDiv1->boundingBox().size.width - offset - btnSize, btnY, "ic_action_users.png", "Multiplayer", btnSize, TAG_MULITPLAYER);
    btnY -= btnSize * 1.7f;
    addButton(pDiv1->getPositionX() + offset, btnY, "ic_action_users.png", "Blocked", btnSize, TAG_BLOCKED);
    addButton(pDiv1->getPositionX() + pDiv1->boundingBox().size.width - offset - btnSize, btnY, "ic_action_users.png", "Endless", btnSize, TAG_ENDLESS);
    //addButton(pDiv1->getPositionX() + smallBtnSize * 0.5f, btnY + smallBtnSize * 1.5f, "ic_action_playback_play.png", smallBtnSize);
    
    LayerColor * pDiv2 = LayerColor::create(ccc4(0, 0, 0, 200), screenWidth * 0.8f, screenWidth * 0.001f);
    pDiv2->setPosition(pDiv1->getPositionX(), getScreenHeight() * 0.11f);
    addChild(pDiv2);
    
//    LayerColor * pLeaderboard = LayerColor::create(ccc4(c.r, c.g, c.b, 255), smallBtnSize, smallBtnSize);
//    pLeaderboard->setPositionX(getScreenWidth() * 0.3f);
//    pLeaderboard->setPositionY(btnY);
//    addChild(pLeaderboard);
//    
//    LayerColor * pShop = LayerColor::create(ccc4(c.r, c.g, c.b, 255), smallBtnSize, smallBtnSize);
//    pShop->setPositionX(getScreenWidth() * 0.5f);
//    pShop->setPositionY(btnY);
//    addChild(pShop);
//    
//    LayerColor * pAchivments = LayerColor::create(ccc4(c.r, c.g, c.b, 255), smallBtnSize, smallBtnSize);
//    pAchivments->setPositionX(getScreenWidth() * 0.7f);
//    pAchivments->setPositionY(btnY);
//    addChild(pAchivments);
}

void MainScene::handleUpdate(float dt)
{
   
}

bool MainScene::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pTouchedBtn = NULL;
    for (int i = 0; NULL == m_pTouchedBtn && i < getChildrenCount(); i++)
    {
        Node * pChild = getChildren().at(i);
        if (pChild->getTag() > 0)
        {
            if (isClicked(pChild, pTouch))
            {
                m_pTouchedBtn = pChild;
            }
        }
    }
    return true;
}

void MainScene::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedBtn && !isClicked(m_pTouchedBtn, pTouch))
    {
        
    }
}

void MainScene::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
   if (NULL != m_pTouchedBtn)
   {
       if (isClicked(m_pTouchedBtn, pTouch))
       {
           switchScene(GameScene_2Balls::scene(1));
       }
       m_pTouchedBtn = NULL;
   }
}

void MainScene::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

cocos2d::Node * MainScene::addButton(float x, float y, const char * icon, const char * text, float size, int tag)
{
    Color3B c = Pref::getInstance()->getColor(COLOR_FILL);
    
    float innerSize = size * 0.9f;
    LayerColor * pMulti = LayerColor::create(ccc4(c.r, c.g, c.b, 255), size, size);
    pMulti->setTag(tag);
    pMulti->setPositionX(x);
    pMulti->setPositionY(y);
    addChild(pMulti);
    
    LayerColor * pMultiInner = LayerColor::create(ccc4(c.r, c.g, c.b, 255), innerSize, innerSize);
    pMultiInner->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pMultiInner->setPositionX(pMulti->getPositionX() + pMulti->boundingBox().size.width / 2 - innerSize / 2);
    pMultiInner->setPositionY(pMulti->getPositionY() + pMulti->boundingBox().size.height / 2 - innerSize / 2);
    addChild(pMultiInner);
    
    pMulti->setUserData(pMultiInner);
    
    Sprite * pMultiIcon = Sprite::create(icon);
    pMultiIcon->setColor(c);
    pMultiIcon->setPositionX(pMultiInner->boundingBox().size.width / 2);
    pMultiIcon->setPositionY(pMultiInner->boundingBox().size.height / 2);
    pMultiInner->addChild(pMultiIcon);
    
    CCLabelTTF * pLabel = CCLabelTTF::create(text, fontList[0], size * 0.13f);
    pLabel->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pLabel->setPosition(x + size / 2, y - size * 0.13f);
    addChild(pLabel);
}