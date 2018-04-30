#include "GameScene_ZombieRun.h"
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

#define TYPE_MONSTER (0)

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_ZombieRun::GameScene_ZombieRun()
{
    srand(time(NULL));
    m_fLastAddedMonster = 0;
    m_bTouchDown = false;
    m_pDraggedMonster = NULL;
}

GameScene_ZombieRun::~GameScene_ZombieRun()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_ZombieRun::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_ZombieRun *layer = GameScene_ZombieRun::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_ZombieRun::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
}

void GameScene_ZombieRun::handleUpdate(float dt)
{
    updateWorld(dt);
    
    if (getTime() - m_fLastAddedMonster > 1)
    {
        m_fLastAddedMonster = getTime();
        addMonster();
    }
}

bool GameScene_ZombieRun::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_touchPos = pTouch->getLocation();
    m_touchStart = pTouch->getLocation();
    m_fTouchStartTime = getTime();
    m_bTouchDown = true;
    m_bTouchStart = true;
    return true;
}

void GameScene_ZombieRun::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_touchPos = pTouch->getLocation();
}

void GameScene_ZombieRun::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_bTouchDown = false;
    
    if (NULL != m_pDraggedMonster)
    {
        float dt = getTime() - m_fTouchStartTime;
        float dx = pTouch->getLocation().x - m_touchStart.x;
        float dy = pTouch->getLocation().y - m_touchStart.y;
        float d = sqrt(dx*dx + dy*dy);
        float v = (d / dt) * 0.05f;
        float a = atan2f(dy, dx);
        
        Sprite * pSprite = (Sprite *)m_pDraggedMonster->GetUserData();
        pSprite->setTag(2);
        m_pDraggedMonster->ApplyLinearImpulse(b2Vec2(v * cos(a), v * sin(a)), m_pDraggedMonster->GetWorldCenter(), true);
        m_pDraggedMonster = NULL;
    }
}

void GameScene_ZombieRun::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_ZombieRun::buildWorld()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    m_pWorld = new b2World(gravity);
    
    Sprite * pBase = Sprite::create("circle.png");
    pBase->setColor(ccc3(129, 35, 100));
    pBase->setScale(0.5f);
    pBase->setPosition(getScreenWidth() / 2, getScreenHeight() / 2);
    addChild(pBase);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.userData = pBase;
    bodyDef.position.Set(pBase->getPositionX()/PTM_RATIO, pBase->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pBase->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 0.0f;
    pBody->CreateFixture(&shapeDef);
}

void GameScene_ZombieRun::updateWorld(float dt)
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
            
            if (TYPE_MONSTER == sprite->getTag())
            {
                float dx = getScreenWidth() / 2 - sprite->getPositionX();
                float dy = getScreenHeight() / 2 - sprite->getPositionY();
                float a = atan2f(dy, dx);
                float v = -3;
                b->SetTransform(b->GetPosition(), a);
                b->SetLinearVelocity(b2Vec2(v * cos(M_PI + a), v * sin(M_PI + a)));
            }
        }
    }
    
    if (m_bTouchDown)
    {
        if (m_bTouchStart)
        {
            m_bTouchStart = false;
            m_pDraggedMonster = NULL;
            
            for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
            {
                if (NULL != b->GetUserData())
                {
                    Node* sprite = (Node*)b->GetUserData();
                    if (TYPE_MONSTER == sprite->getTag())
                    {
                        float dx = m_touchPos.x - sprite->getPositionX();
                        float dy = m_touchPos.y - sprite->getPositionY();
                        float d = sqrt(dx*dx + dy*dy);
                        if (d < sprite->boundingBox().size.width * 0.7f)
                        {
                            m_pDraggedMonster = b;
                            break;
                        }
                    }
                }
            }
        }
        else if (NULL != m_pDraggedMonster)
        {
            float x = m_touchPos.x / PTM_RATIO;
            float y = m_touchPos.y / PTM_RATIO;
            m_pDraggedMonster->SetTransform(b2Vec2(x, y), m_pDraggedMonster->GetAngle());
        }
    }
}


void GameScene_ZombieRun::addMonster()
{
    float a = rand() % 360;
    float rad = CC_DEGREES_TO_RADIANS(a);
    float x = getScreenWidth() / 2 + cos(rad) * (getScreenWidth() / 2);
    float y = getScreenHeight() / 2 + sin(rad) * (getScreenHeight() / 2);
    
    Sprite * pMonster = Sprite::create("player.png");
    pMonster->setScale(0.75f);
    pMonster->setPosition(x, y);
    pMonster->setTag(TYPE_MONSTER);
    addChild(pMonster);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = pMonster;
    bodyDef.angle = M_PI + rad;
    bodyDef.position.Set(pMonster->getPositionX()/PTM_RATIO, pMonster->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pMonster->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 0.0f;
    pBody->CreateFixture(&shapeDef);
    
    float v = -3;
    pBody->SetLinearVelocity(b2Vec2(v * cos(rad), v * sin(rad)));
}