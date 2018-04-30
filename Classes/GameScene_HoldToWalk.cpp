#include "GameScene_HoldToWalk.h"
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

GameScene_HoldToWalk::GameScene_HoldToWalk()
{
    srand(time(NULL));
    m_bDone = false;
    m_bMove = false;
    m_fMoveSpeed = 0;
}

GameScene_HoldToWalk::~GameScene_HoldToWalk()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_HoldToWalk::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_HoldToWalk *layer = GameScene_HoldToWalk::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_HoldToWalk::handleInit(float screenWidth, float screenHeight)
{
    m_pGameLayer = Layer::create();
    addChild(m_pGameLayer);
    
    createPlayer();
    
    float y = getScreenHeight() * 0.6f;
    for (int i = 0; i < 100; i++)
    {
        float x = getScreenWidth() * 0.1f + rand() % (int)(getScreenWidth() * 0.8f);
        addEnemy(x, y);
        y += m_pPlayer->boundingBox().size.height * 2 + rand() % (int)(getScreenHeight() * 0.1f);
    }
}

void GameScene_HoldToWalk::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        if (m_bMove)
        {
            m_fMoveSpeed += dt * 15;
        }
        else
        {
            m_fMoveSpeed -= dt * 25;
            if (m_fMoveSpeed < 0)
            {
                m_fMoveSpeed = 0;
            }
        }
        m_pGameLayer->setPositionY(m_pGameLayer->getPositionY() - m_fMoveSpeed);
        
        
        float px = m_pPlayer->getPositionX() - m_pGameLayer->getPositionX();
        float py = m_pPlayer->getPositionY() - m_pGameLayer->getPositionY();
        for (int i = 0; i < m_pGameLayer->getChildrenCount(); i++)
        {
            Node * pEnemy = (Node *)m_pGameLayer->getChildren().at(i);
            switch (pEnemy->getTag())
            {
                case -1 :
                {
                    float newX = pEnemy->getPositionX() - dt * 300;
                    if (newX < pEnemy->boundingBox().size.width / 2)
                    {
                        newX = pEnemy->boundingBox().size.width / 2;
                        pEnemy->setTag(1);
                    }
                    pEnemy->setPositionX(newX);
                } break;
                case 1 :
                {
                    float newX = pEnemy->getPositionX() + dt * 300;
                    if (newX > getScreenWidth() - pEnemy->boundingBox().size.width / 2)
                    {
                        newX = getScreenWidth() - pEnemy->boundingBox().size.width / 2;
                        pEnemy->setTag(-1);
                    }
                    pEnemy->setPositionX(newX);
                } break;
            }
            
            float dx = fabsf(pEnemy->getPositionX() - px);
            float dy = fabsf(pEnemy->getPositionY() - py);
            if (dx < m_pPlayer->boundingBox().size.width / 2 && dy < m_pPlayer->boundingBox().size.height / 2)
            {
                onGameOver();
                break;
            }
        }
    }
}

bool GameScene_HoldToWalk::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_bMove = true;
    
    return true;
}

void GameScene_HoldToWalk::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
}

void GameScene_HoldToWalk::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    m_bMove = false;
}

void GameScene_HoldToWalk::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_HoldToWalk::onGameOver()
{
    m_bDone = true;
    
    DelayTime * pDelay = DelayTime::create(0.5f);
    CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_HoldToWalk::restartGame));
    runAction(CCSequence::createWithTwoActions(pDelay, pFillDone));
}

void GameScene_HoldToWalk::restartGame(cocos2d::Node * pSender)
{
    switchScene(GameScene_HoldToWalk::scene());
}

void GameScene_HoldToWalk::addEnemy(float x, float y)
{
    Sprite * pEnemy = Sprite::create("circle.png");
    pEnemy->setScale(0.2f);
    pEnemy->setColor(ccc3(255, 255, 0));
    pEnemy->setPosition(x, y);
    pEnemy->setTag(rand() % 2 == 0 ? 1 : -1);
    m_pGameLayer->addChild(pEnemy);
}

void GameScene_HoldToWalk::createPlayer()
{
    m_pPlayer = Sprite::create("circle.png");
    m_pPlayer->setScale(0.3f);
    m_pPlayer->setColor(ccc3(255, 0, 0));
    m_pPlayer->setPosition(getScreenWidth() / 2, getScreenHeight() * 0.2f);
    addChild(m_pPlayer);
}