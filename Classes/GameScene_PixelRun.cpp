#include "GameScene_PixelRun.h"
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

#define ROTATE_SPEED (30)

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_PixelRun::GameScene_PixelRun()
{
    srand(time(NULL));
    m_bDone = false;
    m_fAddEnemyCounter = 0;
}

GameScene_PixelRun::~GameScene_PixelRun()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_PixelRun::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_PixelRun *layer = GameScene_PixelRun::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_PixelRun::handleInit(float screenWidth, float screenHeight)
{
    buildRoads();
    
    m_pEnemies = Layer::create();
    addChild(m_pEnemies);
    
    addPlayer();
}

void GameScene_PixelRun::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        moveRoads(dt);
        moveEnemies(dt);
        checkCollisions();
        checkEnemyExit();
        
        m_fAddEnemyCounter += dt;
        if (m_fAddEnemyCounter > 2)
        {
            m_fAddEnemyCounter = 0;
            addEnemy(rand() % 2);
        }
    }
}

bool GameScene_PixelRun::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    return true;
}

void GameScene_PixelRun::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
}

void GameScene_PixelRun::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    updatePlayerRoad((m_pPlayer->getTag() + 1) % 2);
}

void GameScene_PixelRun::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_PixelRun::buildRoads()
{
    m_pRoads[0] = Sprite::create("circle.png");
    m_pRoads[0]->setPosition(Vec2(getScreenWidth() / 2, getScreenHeight() / 2));
    m_pRoads[0]->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    m_pRoads[0]->setScale(1.2f);
    addChild(m_pRoads[0]);
    
    m_pRoads[1] = Sprite::create("circle.png");
    m_pRoads[1]->setPosition(Vec2(getScreenWidth() / 2, getScreenHeight() / 2));
    m_pRoads[1]->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    m_pRoads[1]->setScale(1.0f);
    addChild(m_pRoads[1]);
    
    for (int i = 0; i < 2; i++)
    {
        m_pRoads[i]->setTag((int)(m_pRoads[i]->boundingBox().size.height / 2));
        m_roadStartPos[i].x = m_pRoads[i]->getPositionX();
        m_roadStartPos[i].y = m_pRoads[i]->getPositionY() + m_pRoads[i]->boundingBox().size.height / 2;
    }
    
    Sprite * pCenter = Sprite::create("circle.png");
    pCenter->setPosition(Vec2(getScreenWidth() / 2, getScreenHeight() / 2));
    pCenter->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pCenter->setScale(0.8f);
    addChild(pCenter);
}

void GameScene_PixelRun::moveRoads(float dt)
{
    for (int i = 0; i < 2; i++)
    {
        m_pRoads[i]->setRotation(m_pRoads[i]->getRotation() + dt * ROTATE_SPEED);
    }
    
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pEnemy = (Node *)m_pEnemies->getChildren().at(i);
        float rotation = pEnemy->getRotation() - dt * ROTATE_SPEED;
        pEnemy->setRotation(rotation);
        
        int road = pEnemy->getTag();
        float a = CC_DEGREES_TO_RADIANS(rotation);
        float r = m_pRoads[road]->getTag() - pEnemy->boundingBox().size.width / 2;
        float newX = m_pRoads[road]->getPositionX() + cos(a) * r;
        float newY = m_pRoads[road]->getPositionY() + sin(a) * r;
        pEnemy->setPosition(newX, newY);
    }
}

void GameScene_PixelRun::moveEnemies(float dt)
{
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pEnemy = (Node *)m_pEnemies->getChildren().at(i);
        float rotation = pEnemy->getRotation() - dt * ROTATE_SPEED;
        pEnemy->setRotation(rotation);
        
        int road = pEnemy->getTag();
        float a = CC_DEGREES_TO_RADIANS(rotation);
        float r = m_pRoads[road]->getTag() - pEnemy->boundingBox().size.width * 0.6f;
        float newX = m_pRoads[road]->getPositionX() + cos(a) * r;
        float newY = m_pRoads[road]->getPositionY() + sin(a) * r;
        pEnemy->setPosition(newX, newY);
    }
}

void GameScene_PixelRun::addPlayer()
{
    m_pPlayer = Sprite::create("circle.png");
    m_pPlayer->setColor(ccc3(0, 255, 0));
    m_pPlayer->setScale(0.08f);
    addChild(m_pPlayer);
    
    updatePlayerRoad(0);
}

void GameScene_PixelRun::updatePlayerRoad(int road)
{
    m_pPlayer->setTag(road);
    
    float a = CC_DEGREES_TO_RADIANS(-90);
    float r = m_pRoads[road]->getTag() - m_pPlayer->boundingBox().size.width * 0.6f;
    float newX = m_pRoads[road]->getPositionX() + cos(a) * r;
    float newY = m_pRoads[road]->getPositionY() + sin(a) * r;
    m_pPlayer->setPosition(newX, newY);
}

void GameScene_PixelRun::addEnemy(int road)
{
    Sprite * pEnemy = Sprite::create("circle.png");
    pEnemy->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pEnemy->setRotation(90);
    pEnemy->setScale(0.08f);
    pEnemy->setTag(road);
    m_pEnemies->addChild(pEnemy);
}

void GameScene_PixelRun::checkCollisions()
{
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pEnemy = (Node *)m_pEnemies->getChildren().at(i);
        if (pEnemy->getTag() == m_pPlayer->getTag())
        {
            float dx = abs(m_pPlayer->getPositionX() - pEnemy->getPositionX());
            float dy = abs(m_pPlayer->getPositionY() - pEnemy->getPositionY());
            if (dx < m_pPlayer->boundingBox().size.width && dy < m_pPlayer->boundingBox().size.height)
            {
                onGameOver();
                return;
            }
        }
    }
}

void GameScene_PixelRun::checkEnemyExit()
{
    std::vector<Node *> toRemove;
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pEnemy = (Node *)m_pEnemies->getChildren().at(i);
        if (pEnemy->getRotation() < -250)
        {
            toRemove.push_back(pEnemy);
        }
    }
    for (int i = 0; i < toRemove.size(); i++)
    {
        m_pEnemies->removeChild(toRemove[i]);
    }
}

void GameScene_PixelRun::onGameOver()
{
    m_bDone = true;
    
    DelayTime * pDelay = DelayTime::create(0.5f);
    CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_PixelRun::restartGame));
    runAction(CCSequence::createWithTwoActions(pDelay, pFillDone));
}

void GameScene_PixelRun::restartGame(cocos2d::Node * pSender)
{
    switchScene(GameScene_PixelRun::scene());
}