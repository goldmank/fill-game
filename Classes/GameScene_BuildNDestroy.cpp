#include "GameScene_BuildNDestroy.h"
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

#define TAG_LAYER (1)
#define TAG_ENEMY (2)
#define TAG_PLAYER (3)
#define TAG_BULLET (4)

#define MOVE_SPPED (600)
#define JUMP_SPEED (200)
#define GRAVITY_Y  (20)
#define MAX_SPEED  (10)
#define NEW_BULLETE_TIME (0.1f)
#define NEW_ENEMY_TIME  (1)

#define CAT_GROUND  0x0001
#define CAT_WALL    0x0002
#define CAT_PLAYER  0x0004
#define CAT_ENEMY   0x0008
#define CAT_BULLET  0x0016

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_BuildNDestroy::GameScene_BuildNDestroy()
{
    srand(time(NULL));
    m_bDone = false;
    m_bLoad = false;
}

GameScene_BuildNDestroy::~GameScene_BuildNDestroy()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_BuildNDestroy::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_BuildNDestroy *layer = GameScene_BuildNDestroy::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_BuildNDestroy::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_BuildNDestroy::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_BuildNDestroy::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_BuildNDestroy::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_BuildNDestroy::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
}

void GameScene_BuildNDestroy::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        if (m_bLoad)
        {
            m_bLoad = false;
            capture();
            load();
        }
        updateWorld(dt);
        checkCollisions();
    }
}

bool GameScene_BuildNDestroy::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    
    
    return true;
}

void GameScene_BuildNDestroy::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    
}

void GameScene_BuildNDestroy::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    if (pTouch->getLocation().x > getScreenWidth() * 0.9f && pTouch->getLocation().y > getScreenHeight() * 0.9f)
    {
        m_bLoad = true;
    }
    else
    {
        addBox(pTouch->getLocation().x, pTouch->getLocation().y, 0);
    }
}

void GameScene_BuildNDestroy::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_BuildNDestroy::buildWorld()
{
    float w = getScreenWidth();
    float h = getScreenHeight();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -GRAVITY_Y);
    m_pWorld = new b2World(gravity);
    m_pWorld->SetContactListener(this);
    
    // Define the ground body
    Color3B border = Pref::getInstance()->getColor(COLOR_BG);
    LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), w, h * 0.05f);
    pGroundTile->setPosition(Vec2(0, 0));
    pGroundTile->setTag(TAG_LAYER);
    addChild(pGroundTile);
    
    b2BodyDef groundBodyDef;
    groundBodyDef.userData = pGroundTile;
    groundBodyDef.position.Set(
                               (pGroundTile->getPositionX() + pGroundTile->boundingBox().size.width / 2)/PTM_RATIO,
                               (pGroundTile->getPositionY() + pGroundTile->boundingBox().size.height / 2)/PTM_RATIO
                               );
    b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
    
    b2PolygonShape groundShape;
    groundShape.SetAsBox((pGroundTile->boundingBox().size.width/2)/PTM_RATIO, (pGroundTile->boundingBox().size.height/2)/PTM_RATIO);
    
    b2FixtureDef groundFixDef;
    groundFixDef.shape = &groundShape;
    groundFixDef.friction = 1.0f;
    pGround->CreateFixture(&groundFixDef);
    
    // add walls
    float wallsX[2];
    wallsX[0] = 0;
    wallsX[1] = getScreenWidth() - 5;
    for (int i = 0; i < 2; i++)
    {
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), 5, h);
        pGroundTile->setPosition(Vec2(wallsX[i], 0));
        pGroundTile->setTag(TAG_LAYER);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(
                                   (pGroundTile->getPositionX() + pGroundTile->boundingBox().size.width / 2)/PTM_RATIO,
                                   (pGroundTile->getPositionY() + pGroundTile->boundingBox().size.height / 2)/PTM_RATIO
                                   );
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox((pGroundTile->boundingBox().size.width/2)/PTM_RATIO, (pGroundTile->boundingBox().size.height/2)/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.friction = 1.0f;
        pGround->CreateFixture(&groundFixDef);
    }
}

void GameScene_BuildNDestroy::updateWorld(float dt)
{
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    m_pWorld->Step(dt, velocityIterations, positionIterations);
    
    // Iterate over the bodies in the physics world
    std::vector<b2Body *> outEnemies;
    for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
    {
        if (NULL != b->GetUserData())
        {
            Node* sprite = (Node*)b->GetUserData();
            sprite->setPosition(Vec2(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
            
            if (TAG_LAYER == sprite->getTag())
            {
                sprite->setPositionX(sprite->getPositionX() - sprite->boundingBox().size.width/2);
                sprite->setPositionY(sprite->getPositionY() - sprite->boundingBox().size.height/2);
            }                
        }
    }
}

void GameScene_BuildNDestroy::checkCollisions()
{
    std::vector<b2Body *> toRemove;
    bool playerHit = false;
    for (std::vector<MyContact>::iterator pos = m_contacts.begin(); pos != m_contacts.end(); ++pos)
    {
        b2Body *bodyA = (*pos).fixtureA->GetBody();
        b2Body *bodyB = (*pos).fixtureB->GetBody();
        if (NULL != bodyA->GetUserData() && NULL != bodyB->GetUserData())
        {
            Sprite * pSpriteA = (Sprite *)bodyA->GetUserData();
            Sprite * pSpriteB = (Sprite *)bodyB->GetUserData();
            if (TAG_ENEMY == pSpriteB->getTag() && TAG_PLAYER == pSpriteA->getTag())
            {
                playerHit = true;
            }
            else if (TAG_ENEMY == pSpriteA->getTag() && TAG_PLAYER == pSpriteB->getTag())
            {
                playerHit = true;
            }
            else if ((TAG_BULLET == pSpriteA->getTag() && TAG_ENEMY == pSpriteB->getTag()) || (TAG_BULLET == pSpriteB->getTag() && TAG_ENEMY == pSpriteA->getTag()))
            {
                toRemove.push_back(bodyA);
                toRemove.push_back(bodyB);
            }
        }
    }
}

void GameScene_BuildNDestroy::addBox(float x, float y, float angle)
{
    Sprite * pSprite = Sprite::create("rect.png");
    pSprite->setScale(0.3f);
    pSprite->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pSprite->setPosition(x, y);
    addChild(pSprite);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = pSprite;
    bodyDef.position.Set(pSprite->getPositionX()/PTM_RATIO, pSprite->getPositionY()/PTM_RATIO);
    bodyDef.angle = angle;
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox((pSprite->boundingBox().size.width/2)/PTM_RATIO, (pSprite->boundingBox().size.height/2)/PTM_RATIO);
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 1.0f;
    shapeDef.restitution = 0.0f;
    pBody->CreateFixture(&shapeDef);
}

void GameScene_BuildNDestroy::capture()
{
    m_bodies.clear();
    for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
    {
        if (NULL != b->GetUserData())
        {
            Node* sprite = (Node*)b->GetUserData();
            if (TAG_LAYER != sprite->getTag())
            {
                BodyDetails bd;
                bd.m_fAngle = b->GetAngle();
                bd.m_pos = b->GetPosition();
                m_bodies.push_back(bd);
            }
        }
    }
}

void GameScene_BuildNDestroy::load()
{
    if (NULL != m_pWorld)
    {
        delete m_pWorld;
        m_pWorld = NULL;
    }
    removeAllChildren();
    buildWorld();
    m_contacts.clear();
    for (int i = 0; i < m_bodies.size(); i++)
    {
        BodyDetails & bd = m_bodies[i];
        addBox(bd.m_pos.x * PTM_RATIO, bd.m_pos.y * PTM_RATIO, bd.m_fAngle);
    }
}