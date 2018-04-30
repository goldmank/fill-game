#include "GameScene_Rotate.h"

using namespace cocos2d;
using namespace CocosDenshion;

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Rotate::GameScene_Rotate()
: m_pBody(NULL)
{
}

GameScene_Rotate::~GameScene_Rotate()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Rotate::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Rotate *layer = GameScene_Rotate::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// GameScene methods
// ===========================================================

void GameScene_Rotate::prepareEnemies(cocos2d::CCLayer * pEnemies)
{
    CCSprite * pEnemy = CCSprite::create("circle.png");
    pEnemy->setScale(0.21f);
    pEnemy->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pEnemy->setPosition(ccp(getScreenWidth() / 2, getScreenHeight() * 0.75f));
    pEnemies->addChild(pEnemy);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.userData = pEnemy;
    bodyDef.position.Set(pEnemy->getPositionX()/PTM_RATIO, pEnemy->getPositionY()/PTM_RATIO);
    m_pBody = getWorld()->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pEnemy->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    m_pBody->CreateFixture(&shapeDef);
    
    float a = 0;
    float newX = getScreenWidth() / 2 + cos(a) * getScreenWidth() * 0.35f;
    float newY = getScreenHeight() / 2 + sin(a) * getScreenWidth() * 0.35f;
    m_pBody->SetTransform(b2Vec2(newX/PTM_RATIO, newY/PTM_RATIO), 0);
}

void GameScene_Rotate::updateEnemies(cocos2d::CCLayer * pEnemies, float dt)
{
    float a = getTime() * 2;
    float newX = getScreenWidth() / 2 + cos(a) * getScreenWidth() * 0.35f;
    float newY = getScreenHeight() / 2 + sin(a) * getScreenWidth() * 0.35f;
    m_pBody->SetTransform(b2Vec2(newX/PTM_RATIO, newY/PTM_RATIO), 0);
}