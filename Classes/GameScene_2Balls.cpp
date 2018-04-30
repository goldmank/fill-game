#include "GameScene_2Balls.h"

using namespace cocos2d;
using namespace CocosDenshion;

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_2Balls::GameScene_2Balls(int level)
: m_iSpeed(5)
, m_iLevel(level)
{
}

GameScene_2Balls::~GameScene_2Balls()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_2Balls::scene(int level)
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_2Balls *layer = GameScene_2Balls::create(level);
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// GameScene methods
// ===========================================================

void GameScene_2Balls::prepareEnemies(cocos2d::CCLayer * pEnemies)
{
    float w = getScreenWidth();
    float h = getScreenHeight();
    switch (m_iLevel)
    {
        case 1 :
        {
            m_iSpeed = 5;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.12f);
        } break;
        case 2 :
        {
            m_iSpeed = 10;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.12f);
        } break;
        case 3 :
        {
            m_iSpeed = 5;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.22f);
        } break;
        case 4 :
        {
            m_iSpeed = 10;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.22f);
        } break;
        case 5 :
        {
            m_iSpeed = 5;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.5f, 0.12f);
        } break;
        case 6 :
        {
            m_iSpeed = 10;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.5f, 0.12f);
        } break;
        case 7 :
        {
            m_iSpeed = 10;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.12f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.5f, 0.12f);
        } break;
        case 8 :
        {
            m_iSpeed = 10;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.5f, 0.22f);
        } break;
        case 9 :
        {
            m_iSpeed = 5;
            addEnemy(pEnemies, w * 0.5f, h * 0.75f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.25f, 0.22f);
            addEnemy(pEnemies, w * 0.75f, h * 0.5f, 0.22f);
            addEnemy(pEnemies, w * 0.25f, h * 0.5f, 0.22f);
            addEnemy(pEnemies, w * 0.5f, h * 0.5f, 0.22f);
        } break;
    }
    
}

void GameScene_2Balls::updateEnemies(cocos2d::CCLayer * pEnemies, float dt)
{
    fixEnemiesSpeed();
}

void GameScene_2Balls::onRestart(cocos2d::CCNode * pSender)
{
    switchScene(GameScene_2Balls::scene(m_iLevel));
}

void GameScene_2Balls::enemySlowdown(cocos2d::CCLayer * pEnemies, bool slowdown)
{
    
}

void GameScene_2Balls::addEnemy(cocos2d::CCLayer * pEnemies, float x, float y, float size)
{
    Sprite * pEnemy = Sprite::create("circle.png");
    pEnemy->setScale(size);
    pEnemy->setTag(rand() % 360);
    pEnemy->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pEnemy->setPosition(ccp(x, y));
    
    pEnemies->addChild(pEnemy);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = pEnemy;
    bodyDef.position.Set(pEnemy->getPositionX()/PTM_RATIO, pEnemy->getPositionY()/PTM_RATIO);
    b2Body * pBody = getWorld()->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pEnemy->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    pBody->CreateFixture(&shapeDef);
    
    int d = rand() % 2 == 0 ? 1 : -1;
    pBody->SetLinearVelocity(b2Vec2(d * m_iSpeed + rand() % m_iSpeed, m_iSpeed + rand() % m_iSpeed));
}

int GameScene_2Balls::getLevelNumber()
{
    return m_iLevel;
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_2Balls::fixEnemiesSpeed()
{
    for (b2Body* b = getWorld()->GetBodyList(); b; b = b->GetNext())
    {
        if (NULL != b->GetUserData())
        {
            b2Vec2 v = b->GetLinearVelocity();
            if (v.Length() < m_iSpeed)
            {
                v.x *= 2;
                v.y *= 2;
                b->SetLinearVelocity(v);
            }
        }
    }
}