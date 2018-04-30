#include "GameScene_SwipeToCircle.h"
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

#define ROTATE_SPEED (0.5f)
#define SWIPE_SPEED  (300)

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_SwipeToCircle::GameScene_SwipeToCircle()
{
    srand(time(NULL));
    m_bDone = false;
}

GameScene_SwipeToCircle::~GameScene_SwipeToCircle()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_SwipeToCircle::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_SwipeToCircle *layer = GameScene_SwipeToCircle::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_SwipeToCircle::handleInit(float screenWidth, float screenHeight)
{
    m_pCenter = Sprite::create("circle.png");
    m_pCenter->setScale(0.6f);
    m_pCenter->setPosition(screenWidth / 2, screenHeight / 2);
    addChild(m_pCenter);
    
    Color3B colors[] = { Pref::getInstance()->getColor(COLOR_ENEMY), Pref::getInstance()->getColor(COLOR_BORDER), Pref::getInstance()->getColor(COLOR_FILL) };
    float a = 0;
    int count = 15;
    for (int i = 0; i < count; i++)
    {
        Sprite * pCircle = Sprite::create("circle.png");
        pCircle->setTag(i % 3);
        pCircle->setScale(0.15f);
        pCircle->setPositionX(m_pCenter->getPositionX() + cos(a) * m_pCenter->boundingBox().size.width * 0.75f);
        pCircle->setPositionY(m_pCenter->getPositionY() + sin(a) * m_pCenter->boundingBox().size.width * 0.75f);        
        pCircle->setColor(colors[pCircle->getTag()]);
        addChild(pCircle);
        
        CircleData * pData = new CircleData();
        pData->m_fAngle = a;
        pData->m_fRadius = m_pCenter->boundingBox().size.width * 0.75f;
        pData->m_bMovingIn = false;
        pData->m_bHitCenter = false;
        pCircle->setUserData(pData);
        
        a += (M_PI * 2) / count;
    }
    
    int startColor = rand() % 3;
    m_pCenter->setTag(startColor);
    m_pCenter->setColor(colors[m_pCenter->getTag()]);
}

void GameScene_SwipeToCircle::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        std::vector<cocos2d::Node *> toRemove;
        
        for (int i = 0; i < getChildrenCount(); i++)
        {
            Node * pCircle = (Node *)getChildren().at(i);
            if (pCircle == m_pCenter)
            {
                continue;
            }
            
            CircleData * pData = (CircleData *)pCircle->getUserData();
            pData->m_fAngle += dt * ROTATE_SPEED;
            if (pData->m_bMovingIn)
            {
                pData->m_fRadius -= dt * SWIPE_SPEED;
                if (pData->m_fRadius < 0)
                {
                    pData->m_fRadius = 0;
                    toRemove.push_back(pCircle);
                }
            }
            
            pCircle->setPositionX(m_pCenter->getPositionX() + cos(pData->m_fAngle) * pData->m_fRadius);
            pCircle->setPositionY(m_pCenter->getPositionY() + sin(pData->m_fAngle) * pData->m_fRadius);
            
            if (!pData->m_bHitCenter && pData->m_fRadius < m_pCenter->boundingBox().size.width * 0.5f)
            {
                pData->m_bHitCenter = true;
                colorCenter(pCircle->getColor());
            }
        }
        
        for (int i = 0; i < toRemove.size(); i++)
        {
            removeChild(toRemove[i]);
        }
    }
}

bool GameScene_SwipeToCircle::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_pTouchedCircle = NULL;
    for (int i = 0; NULL == m_pTouchedCircle && i < getChildrenCount(); i++)
    {
        Node * pCircle = (Node *)getChildren().at(i);
        if (pCircle == m_pCenter)
        {
            continue;
        }
        
        float dx = fabsf(pTouch->getLocation().x - pCircle->getPositionX());
        float dy = fabsf(pTouch->getLocation().y - pCircle->getPositionY());
        float d = sqrtf(dx*dx + dy*dy);
        if (d < pCircle->boundingBox().size.width)
        {
            CircleData * pData = (CircleData *)pCircle->getUserData();
            pData->m_bMovingIn = true;
            break;
        }
    }
    
    return true;
}

void GameScene_SwipeToCircle::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
//    if (NULL != m_pTouchedCircle)
//    {
//        float dx = fabsf(pTouch->getLocation().x - m_pTouchedCircle->getPositionX());
//        float dy = fabsf(pTouch->getLocation().y - m_pTouchedCircle->getPositionY());
//        float d = sqrtf(dx*dx + dy*dy);
//        if (d < m_pTouchedCircle->boundingBox().size.width * 1.2f)
//        {
//            float dx = fabsf(pTouch->getLocation().x - m_pCenter->getPositionX());
//            float dy = fabsf(pTouch->getLocation().y - m_pCenter->getPositionY());
//            float d = sqrtf(dx*dx + dy*dy);
//            
//            CircleData * pData = (CircleData *)m_pTouchedCircle->getUserData();
//            pData->m_fRadius = d;
//        }
//        else
//        {
//            CircleData * pData = (CircleData *)m_pTouchedCircle->getUserData();
//            pData->m_bMovingIn = true;
//            m_pTouchedCircle = NULL;
//        }
//    }
}

void GameScene_SwipeToCircle::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
//    if (NULL != m_pTouchedCircle)
//    {
//        CircleData * pData = (CircleData *)m_pTouchedCircle->getUserData();
//        pData->m_bMovingIn = true;
//        m_pTouchedCircle = NULL;
//    }
}

void GameScene_SwipeToCircle::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_SwipeToCircle::onGameOver()
{
    m_bDone = true;
    
    DelayTime * pDelay = DelayTime::create(0.5f);
    CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_SwipeToCircle::restartGame));
    runAction(CCSequence::createWithTwoActions(pDelay, pFillDone));
}

void GameScene_SwipeToCircle::restartGame(cocos2d::Node * pSender)
{
    switchScene(GameScene_SwipeToCircle::scene());
}

void GameScene_SwipeToCircle::colorCenter(cocos2d::Color3B color)
{
    m_pCenter->setColor(color);
}