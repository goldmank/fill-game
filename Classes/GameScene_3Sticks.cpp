#include "GameScene_3Sticks.h"
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

#define GAME_HORIZONTAL 1

#define TYPE_ENEMY 1
#define TYPE_STAR  2

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_3Sticks::GameScene_3Sticks()
{
    srand(time(NULL));
    m_fLastEnemyAddTime = 0;
    m_fLastStarAddTime = 0;
    m_bDone = false;
    m_iScore = 0;
    m_iStars = 0;
}

GameScene_3Sticks::~GameScene_3Sticks()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_3Sticks::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_3Sticks *layer = GameScene_3Sticks::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_3Sticks::handleInit(float screenWidth, float screenHeight)
{
    float pad = screenWidth * 0.02f;
    float y = screenHeight * 0.97f;
    m_pScore = CCLabelTTF::create("0", fontList[0], screenWidth * 0.05f);
    m_pScore->setPositionX(pad + m_pScore->boundingBox().size.width / 2);
    m_pScore->setPositionY(y);
    m_pScore->setColor(Color3B(255, 255, 255));
    addChild(m_pScore);
    
    m_pStars = CCLabelTTF::create("0", fontList[0], screenWidth * 0.05f);
    m_pStars->setPositionX(screenWidth - pad - m_pStars->boundingBox().size.width / 2);
    m_pStars->setPositionY(y);
    m_pStars->setColor(Color3B(255, 255, 255));
    addChild(m_pStars);
    
#ifdef GAME_HORIZONTAL
    Sprite * pStick = Sprite::create("stick_h.png");
    pStick->setPosition(screenWidth / 2, screenHeight / 2);
    addChild(pStick);
    m_objectsVec.push_back(pStick);
    
    Sprite * pStick2 = Sprite::create("stick_h.png");
    pStick2->setPosition(screenWidth /2, screenHeight * 0.7f);
    addChild(pStick2);
    m_objectsVec.push_back(pStick2);
    
    Sprite * pStick3 = Sprite::create("stick_h.png");
    pStick3->setPosition(screenWidth /2, screenHeight * 0.3f);
    addChild(pStick3);
    m_objectsVec.push_back(pStick3);
#else
    Sprite * pStick = Sprite::create("stick.png");
    pStick->setPosition(screenWidth / 2, screenHeight / 2);
    addChild(pStick);
    m_objectsVec.push_back(pStick);
    
    Sprite * pStick2 = Sprite::create("stick.png");
    pStick2->setPosition(screenWidth * 0.25f, screenHeight / 2);
    addChild(pStick2);
    m_objectsVec.push_back(pStick2);
    
    Sprite * pStick3 = Sprite::create("stick.png");
    pStick3->setPosition(screenWidth * 0.75f, screenHeight / 2);
    addChild(pStick3);
    m_objectsVec.push_back(pStick3);
#endif
}

void GameScene_3Sticks::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    
    m_fLastEnemyAddTime += dt;
    if (m_fLastEnemyAddTime > 4)
    {
        m_fLastEnemyAddTime = 0;
        addObject(TYPE_ENEMY, rand() % 2 == 0 ? 1 : -1, 90);//60 + rand() % 60);
    }
    
    m_fLastStarAddTime += dt;
    if (m_fLastStarAddTime > 3)
    {
        m_fLastStarAddTime = 0;
        addObject(TYPE_STAR, rand() % 2 == 0 ? 1 : -1, 120);//60 + rand() % 60);
    }
    
    checkCollisions();
}

bool GameScene_3Sticks::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pTouchedObject = NULL;
    for (int i = 0; NULL == m_pTouchedObject && i < m_objectsVec.size(); i++)
    {
        if (isClicked(m_objectsVec[i], pTouch))
        {
            m_pTouchedObject = m_objectsVec[i];
            m_pTouchedObject->setColor(Color3B(255, 0, 0));
        }
    }
    
    return true;
}

void GameScene_3Sticks::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedObject)
    {
#ifdef GAME_HORIZONTAL
        float right = getScreenWidth() * 0.95f;
        float left = getScreenWidth() * 0.05f;
        
        float dx = pTouch->getDelta().x;
        float newX = m_pTouchedObject->getPositionX() + dx;
        if (newX + m_pTouchedObject->boundingBox().size.width / 2 > right)
        {
            newX = right - m_pTouchedObject->boundingBox().size.width / 2;
        }
        else if (newX - m_pTouchedObject->boundingBox().size.width / 2 < left)
        {
            newX = left + m_pTouchedObject->boundingBox().size.width / 2;
        }
        m_pTouchedObject->setPositionX(newX);
#else
        float top = getScreenHeight() * 0.95f;
        float bottom = getScreenHeight() * 0.05f;
        
        float dy = pTouch->getDelta().y;
        float newY = m_pTouchedObject->getPositionY() + dy;
        if (newY + m_pTouchedObject->boundingBox().size.height / 2 > top)
        {
            newY = top - m_pTouchedObject->boundingBox().size.height / 2;
        }
        else if (newY - m_pTouchedObject->boundingBox().size.height / 2 < bottom)
        {
            newY = bottom + m_pTouchedObject->boundingBox().size.height / 2;
        }
        m_pTouchedObject->setPositionY(newY);
#endif
    }
}

void GameScene_3Sticks::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedObject)
    {
        if (isClicked(m_pTouchedObject, pTouch))
        {
            
        }
        m_pTouchedObject->setColor(Color3B(255, 255, 255));
        m_pTouchedObject = NULL;
    }
}

void GameScene_3Sticks::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

cocos2d::Sprite * GameScene_3Sticks::addObject(int type, int direction, float speed)
{
    Sprite * pEnemy = NULL;
    switch (type)
    {
        case TYPE_ENEMY : pEnemy = Sprite::create("player.png"); break;
        case TYPE_STAR : pEnemy = Sprite::create("star.png"); break;
    }
    pEnemy->setTag(type);
    
    
#ifdef GAME_HORIZONTAL
    pEnemy->setPositionX(getScreenWidth() * 0.1f + rand() % (int)(getScreenWidth() * 0.8f));
    float freeArea = getScreenWidth() * 0.1f;
    if (fabsf(pEnemy->getPositionX() - getScreenWidth() / 2) < freeArea)
    {
        float d = rand() % 2 == 0 ? 1 : -1;
        pEnemy->setPositionX(pEnemy->getPositionX() + d * freeArea);
    }
    if (direction > 0)
    {
        pEnemy->setPositionY(getScreenHeight() + pEnemy->boundingBox().size.height);
        pEnemy->setRotation(90);
    }
    else
    {
        pEnemy->setPositionY(-pEnemy->boundingBox().size.height);
        pEnemy->setRotation(270);
    }
    
    float d = getScreenHeight() / speed;
    MoveBy * pMove = MoveBy::create(d, Vec2(0, -1 * direction * (getScreenHeight() + pEnemy->boundingBox().size.height)));
#else
    pEnemy->setPositionY(getScreenHeight() * 0.1f + rand() % (int)(getScreenHeight() * 0.8f));
    float freeArea = getScreenHeight() * 0.1f;
    if (fabsf(pEnemy->getPositionY() - getScreenHeight() / 2) < freeArea)
    {
        float d = rand() % 2 == 0 ? 1 : -1;
        pEnemy->setPositionY(pEnemy->getPositionY() + d * freeArea);
    }
    if (direction > 0)
    {
        pEnemy->setPositionX(getScreenWidth() + pEnemy->boundingBox().size.width);
        pEnemy->setRotation(180);
    }
    else
    {
        pEnemy->setPositionX(-pEnemy->boundingBox().size.width);
    }
    
    float d = getScreenWidth() / speed;
    MoveBy * pMove = MoveBy::create(d, Vec2(-1 * direction * (getScreenWidth() + pEnemy->boundingBox().size.width), 0));
#endif
    CCFiniteTimeAction* pMoveDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_3Sticks::onObjectMoveDone));
    pEnemy->runAction(Sequence::createWithTwoActions(pMove, pMoveDone));
    
    m_enemiesVec.push_back(pEnemy);
    
    addChild(pEnemy);
    
    return pEnemy;
}

void GameScene_3Sticks::onObjectMoveDone(cocos2d::Node * pSender)
{
    int index = -1;
    for (int i = 0; -1 == index && i < m_enemiesVec.size(); i++)
    {
        if (m_enemiesVec[i] == pSender)
        {
            index = i;
        }
    }
    m_enemiesVec.erase(m_enemiesVec.begin() + index);
    removeChild(pSender);
    
    m_iScore++;
    char count[5];
    sprintf(count, "%d", m_iScore);
    float x = m_pScore->getPositionX() - m_pScore->boundingBox().size.width / 2;
    m_pScore->setString(count);
    m_pScore->setPositionX(x + m_pScore->boundingBox().size.width / 2);
}

void GameScene_3Sticks::checkCollisions()
{
    for (int i = 0; !m_bDone && i < m_objectsVec.size(); i++)
    {
        Sprite * pObject = m_objectsVec[i];
        Rect projectileRect = Rect(
                                   pObject->getPosition().x - (pObject->getContentSize().width/2),
                                   pObject->getPosition().y - (pObject->getContentSize().height/2),
                                   pObject->getContentSize().width,
                                   pObject->getContentSize().height);
        
        std::vector<Sprite *>::iterator iter = m_enemiesVec.begin();
        while (m_enemiesVec.end() != iter)
        {
            Sprite * pEnemy = *iter;
            Rect targetRect = Rect(
                                   pEnemy->getPosition().x - (pEnemy->getContentSize().width/2),
                                   pEnemy->getPosition().y - (pEnemy->getContentSize().height/2),
                                   pEnemy->getContentSize().width,
                                   pEnemy->getContentSize().height);
            
            bool removeObject = false;
            if (projectileRect.intersectsRect(targetRect))
            {
                if (TYPE_ENEMY == pEnemy->getTag())
                {
                    onEnemyHit();
                }
                else if (TYPE_STAR == pEnemy->getTag())
                {
                    onStarCollect();
                    removeObject = true;
                }
            }
            
            if (removeObject)
            {
                removeChild(*iter);
                iter = m_enemiesVec.erase(iter);
            }
            else
            {
                iter++;
            }
        }
    }
    
    if (m_bDone)
    {
        switchScene(GameScene_3Sticks::scene());
    }
}

void GameScene_3Sticks::onEnemyHit()
{
    m_bDone = true;
}

void GameScene_3Sticks::onStarCollect()
{
    m_iStars++;
    char count[5];
    sprintf(count, "%d", m_iStars);
    float x = m_pStars->getPositionX() + m_pStars->boundingBox().size.width / 2;
    m_pStars->setString(count);
    m_pStars->setPositionX(x - m_pStars->boundingBox().size.width / 2);
}