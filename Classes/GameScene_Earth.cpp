#include "GameScene_Earth.h"
#include "SimpleAudioEngine.h"
#include "Pref.h"
#include "SystemUtils.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define BORDER_SIZE (getScreenHeight() * 0.01f)
#define TAG_RIGHT_WALL (-2)
#define TAG_TOP_WALL (-3)

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

#define CAT_GROUND  0x0001
#define CAT_PLAYER  0x0004
#define CAT_ENEMY   0x0008
#define CAT_DEAD    0x0016

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Earth::GameScene_Earth()
{
    srand(time(NULL));
    m_fLastAddedEnemy = 0;
}

GameScene_Earth::~GameScene_Earth()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Earth::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Earth *layer = GameScene_Earth::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Earth::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
    buildPlayer();
}

void GameScene_Earth::handleUpdate(float dt)
{
    //m_pPlayerBody->SetTransform(b2Vec2(m_pPlayer->getPositionX()/PTM_RATIO, m_pPlayer->getPositionY()/PTM_RATIO), 0);
    
    updateWorld(dt);
    if (getTime() - m_fLastAddedEnemy > 2)
    {
        m_fLastAddedEnemy = getTime();
        //addEnemy();
    }
    
    float da = dt / 2;
    float angle = m_pGroundBody->GetAngle();
    for (int i = 0; i < m_groundItems.size(); i++)
    {
        Sprite * pItem = m_groundItems[i];
        float dx = pItem->getPositionX() - m_pGround->getPositionX();
        float dy = pItem->getPositionY() - m_pGround->getPositionY();
        float d = sqrtf(dx*dx + dy*dy);
        float a = atan2f(dy, dx);
        
        a += da;
        
        float x = m_pGround->getPositionX() + cos(a) * d;
        float y = m_pGround->getPositionY() + sin(a) * d;
        pItem->setRotation(90 + CC_RADIANS_TO_DEGREES(-a));
        pItem->setPosition(x, y);
    }
    
    m_pGroundBody->SetTransform(m_pGroundBody->GetPosition(), angle + da);
}

bool GameScene_Earth::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
    return true;
}

void GameScene_Earth::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
}

void GameScene_Earth::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    CCLOG("touch end");
    
}

void GameScene_Earth::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Earth::buildWorld()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, -20.0f);
    m_pWorld = new b2World(gravity);
    
    m_pGround = Sprite::create("earth_land.png");
    m_pGround->setPosition(getScreenWidth() / 2, getScreenHeight() * 0.25f - m_pGround->boundingBox().size.height / 2);
    addChild(m_pGround);
    
    // build ground
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.userData = m_pGround;
    bodyDef.position.Set(m_pGround->getPositionX()/PTM_RATIO, m_pGround->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (m_pGround->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 0.0f;
    shapeDef.filter.categoryBits = CAT_GROUND;
    shapeDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
    pBody->CreateFixture(&shapeDef);
    
    m_pGroundBody = pBody;
    
    Sprite * pItem = Sprite::create("earth_items.png");
    pItem->setAnchorPoint(Vec2(0.5f, 0));
    float d = m_pGround->boundingBox().size.height / 2 - pItem->boundingBox().size.height * 0.12f;
    float x = m_pGround->getPositionX() + cos(M_PI_2) * d;
    float y = m_pGround->getPositionY() + sin(M_PI_2) * d;
    pItem->setPosition(x, y);
    addChild(pItem);
    
    //m_groundItems.push_back(pItem);
}

void GameScene_Earth::buildPlayer()
{
    m_pPlayer = Sprite::create("player.png");
    m_pPlayer->setPosition(getScreenWidth() / 2, getScreenHeight() / 2);
    addChild(m_pPlayer, 10);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = m_pPlayer;
    bodyDef.position.Set(m_pPlayer->getPositionX()/PTM_RATIO, m_pPlayer->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (m_pPlayer->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 0.2f;
    shapeDef.filter.categoryBits = CAT_PLAYER;
    shapeDef.filter.maskBits = CAT_GROUND | CAT_ENEMY;
    pBody->CreateFixture(&shapeDef);
    
    m_pPlayerBody = pBody;
}

void GameScene_Earth::addEnemy()
{
    float px = m_pPlayer->getPositionX();
    float py = m_pPlayer->getPositionY();
    float a = M_PI + m_pPlayerBody->GetAngle();
    CCLOG("%.2f, %.2f, %.2f, %.2f", px, py, a);
    
    Sprite * pEnemy = Sprite::create("enemy.png");
    pEnemy->setPosition(px + cos(a) * m_pPlayer->boundingBox().size.width * 0.5f, py + sin(a) * m_pPlayer->boundingBox().size.height * 0.5f);
    addChild(pEnemy, 10);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = pEnemy;
    bodyDef.position.Set(pEnemy->getPositionX()/PTM_RATIO, pEnemy->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pEnemy->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    shapeDef.filter.categoryBits = CAT_ENEMY;
    shapeDef.filter.maskBits = CAT_GROUND | CAT_PLAYER;
    pBody->CreateFixture(&shapeDef);
    
    float v = 5;
    float vx = cos(a) * v;
    float vy = sin(a) * v;
    pBody->SetLinearVelocity(b2Vec2(vx, vy));
}

void GameScene_Earth::updateWorld(float dt)
{
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    m_pWorld->Step(dt, velocityIterations, positionIterations);
    
    // Iterate over the bodies in the physics world
    for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
    {
        if (NULL != b->GetUserData())
        {
            Node* sprite = (Node*)b->GetUserData();
            sprite->setPosition(ccp(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }
    }
}

void GameScene_Earth::removeSprite(cocos2d::Node * pNode)
{
    removeChild(pNode);
}