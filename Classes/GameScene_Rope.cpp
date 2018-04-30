#include "GameScene_Rope.h"
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

#define MOVE_SPPED (630)
#define JUMP_SPEED (200)
#define GRAVITY_Y  (20)

#define CAT_WALL    0x0001
#define CAT_PLAYER  0x0002
#define CAT_ENEMY   0x0004
#define CAT_GROUND  0x0008

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Rope::GameScene_Rope()
{
    srand(time(NULL));
    m_bDone = false;
    m_bTouchDown = false;
}

GameScene_Rope::~GameScene_Rope()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Rope::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Rope *layer = GameScene_Rope::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Accelerometer methods
// ===========================================================

void GameScene_Rope::onAcceleration(cocos2d::Acceleration *acc, cocos2d::Event *event)
{
    if (fabs(m_pPlayerBody->GetLinearVelocity().y) < 0.1f)
    {
        m_pPlayerBody->ApplyForceToCenter(b2Vec2(MOVE_SPPED * acc->x, 0), true);
    }
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_Rope::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_Rope::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_Rope::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_Rope::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Rope::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
    
    Device::setAccelerometerEnabled(true);
    auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameScene_Rope::onAcceleration, this));
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene_Rope::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        if (m_bTouchDown)
        {
            m_pRopeAnchor->ApplyForce(b2Vec2(0,500), m_pRopeAnchor->GetPosition(), true);
        }
        checkCollisions();
        updateWorld(dt);
    }
}

bool GameScene_Rope::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_initialTouchPos = pTouch->getLocation();
    m_fInitialTouchTime = getTime();
    
    m_bTouchDown = true;
    
    return true;
}

void GameScene_Rope::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
}

void GameScene_Rope::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    m_bTouchDown = false;
    
//    float dt = getTime() - m_fInitialTouchTime;
//    if (dt < 0.3f)
//    {
//        Vec2 loc = pTouch->getLocation();
//        float dx = loc.x - m_initialTouchPos.x;
//        float dy = loc.y - m_initialTouchPos.y;
//        if (fabs(dx) > fabs(dy)) // move more to horizontal than vertical
//        {
//            if (dx > 0) // swipe right
//            {
//                onSwipeRight();
//            }
//            else // swipe left
//            {
//                onSwipeLeft();
//            }
//        }
//        else // more vertical
//        {
//            if (dy > 0) // swipe right
//            {
//                onSwipeUp();
//            }
//            else // swipe left
//            {
//                onSwipeDown();
//            }
//        }
//    }
}

void GameScene_Rope::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

void GameScene_Rope::onSwipeLeft()
{
    
}

void GameScene_Rope::onSwipeRight()
{
}

void GameScene_Rope::onSwipeUp()
{
    if (fabs(m_pPlayerBody->GetLinearVelocity().y) < 0.1f)
    {
        m_pPlayerBody->ApplyLinearImpulse(b2Vec2(0, JUMP_SPEED), m_pPlayerBody->GetWorldCenter(), true);
    }
}

void GameScene_Rope::onSwipeDown()
{
    
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Rope::buildWorld()
{
    float w = getScreenWidth();
    float h = getScreenHeight();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -GRAVITY_Y);
    m_pWorld = new b2World(gravity);
    m_pWorld->SetContactListener(this);
    
    float y = getScreenHeight() * 0.8f;
    
    b2Body * pPrev = NULL;
    {
        Sprite * pPart = Sprite::create("rope_anchor.png");
        pPart->setPosition(getScreenWidth() / 2, y);
        addChild(pPart);
        
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.userData = pPart;
        bodyDef.position.Set(pPart->getPositionX()/PTM_RATIO, pPart->getPositionY()/PTM_RATIO);
        b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
        
        b2PolygonShape shape;
        shape.SetAsBox((pPart->boundingBox().size.width/2)/PTM_RATIO, (pPart->boundingBox().size.height/2)/PTM_RATIO);
        
        b2FixtureDef shapeDef;
        shapeDef.shape = &shape;
        shapeDef.density = 1.0f;
        shapeDef.friction = 1.0f;
        shapeDef.restitution = 0.0f;
        pBody->CreateFixture(&shapeDef);

        y -= pPart->boundingBox().size.height / 2;
        
        pPrev = pBody;
        
        m_pRopeAnchor = pBody;
    }
    
    b2RevoluteJointDef revolute_joint;
    for (int i = 0; i < 5; i++)
    {
        Sprite * pPart = Sprite::create("rope_part.png");
        pPart->setPosition(getScreenWidth() / 2, y - pPart->boundingBox().size.height / 2);
        addChild(pPart);
        
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.userData = pPart;
        bodyDef.position.Set(pPart->getPositionX()/PTM_RATIO, pPart->getPositionY()/PTM_RATIO);
        b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
        b2PolygonShape shape;
        shape.SetAsBox((pPart->boundingBox().size.width/2)/PTM_RATIO, (pPart->boundingBox().size.height/2)/PTM_RATIO);
    
        b2FixtureDef shapeDef;
        shapeDef.shape = &shape;
        shapeDef.density = 1.0f;
        shapeDef.friction = 1.0f;
        shapeDef.restitution = 0.0f;
        pBody->CreateFixture(&shapeDef);
        
        revolute_joint.Initialize(pPrev, pBody, b2Vec2(bodyDef.position.x, bodyDef.position.y+0.5f));
        m_pWorld->CreateJoint(&revolute_joint);
        
        y -= pPart->boundingBox().size.height;
        
        pPrev = pBody;
    }
    
    m_pRopeEnd = pPrev;
    
    throwObject();
}

void GameScene_Rope::updateWorld(float dt)
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
            sprite->setPosition(Vec2(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
        }
    }
}

void GameScene_Rope::buildPlayer(float groundY)
{
//    float w = getScreenWidth();
//    
//    Color3B c = Pref::getInstance()->getColor(COLOR_FILL);
//    m_pPlayer = LayerColor::create(ccc4(c.r, c.g, c.b, 255), w * 0.1f, w * 0.1f * 1.5f);
//    m_pPlayer->setTag(TAG_PLAYER);
//    m_pPlayer->setPosition(Vec2(w * 0.05f, groundY));
//    addChild(m_pPlayer);
//    
//    b2BodyDef bodyDef;
//    bodyDef.type = b2_dynamicBody;
//    bodyDef.userData = m_pPlayer;
//    bodyDef.position.Set(
//                         (m_pPlayer->getPositionX() + m_pPlayer->boundingBox().size.width / 2)/PTM_RATIO,
//                         (m_pPlayer->getPositionY() + m_pPlayer->boundingBox().size.height / 2)/PTM_RATIO
//                         );
//    m_pPlayerBody = m_pWorld->CreateBody(&bodyDef);
//    
//    b2PolygonShape shape;
//    shape.SetAsBox((m_pPlayer->boundingBox().size.width/2)/PTM_RATIO, (m_pPlayer->boundingBox().size.height/2)/PTM_RATIO);
//    
//    b2FixtureDef shapeDef;
//    shapeDef.shape = &shape;
//    shapeDef.density = 1.0f;
//    shapeDef.friction = 1.0f;
//    shapeDef.restitution = 0.0f;
//    shapeDef.filter.categoryBits = CAT_PLAYER;
//    shapeDef.filter.maskBits = CAT_ENEMY | CAT_GROUND | CAT_WALL;
//    m_pPlayerBody->CreateFixture(&shapeDef);
}

void GameScene_Rope::throwObject()
{
//    Sprite * pSprite = Sprite::create("circle.png");
//    pSprite->setScale(0.2f);
//    pSprite->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
//    pSprite->setTag(TAG_ENEMY);
//    pSprite->setPosition(Vec2(getScreenWidth() * 0.9f, getScreenHeight() * 0.4f));
//    addChild(pSprite);
//    
//    b2BodyDef bodyDef;
//    bodyDef.type = b2_dynamicBody;
//    bodyDef.userData = pSprite;
//    bodyDef.position.Set(pSprite->getPositionX()/PTM_RATIO, pSprite->getPositionY()/PTM_RATIO);
//    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
//    
//    b2CircleShape shape;
//    shape.m_radius = (pSprite->boundingBox().size.width/2)/PTM_RATIO;
//    
//    b2FixtureDef shapeDef;
//    shapeDef.shape = &shape;
//    shapeDef.density = 1.0f;
//    shapeDef.friction = 0.0f;
//    shapeDef.restitution = 1.0f;
//    shapeDef.filter.categoryBits = CAT_ENEMY;
//    shapeDef.filter.maskBits = CAT_PLAYER | CAT_GROUND;
//    pBody->CreateFixture(&shapeDef);
//    
//    pBody->ApplyLinearImpulse(b2Vec2(-50, 60), pBody->GetWorldCenter(), true);
}

void GameScene_Rope::checkCollisions()
{
    std::vector<b2Body *> toRemove;
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
                toRemove.push_back(bodyB);
            }
            else if (TAG_ENEMY == pSpriteA->getTag() && TAG_PLAYER == pSpriteB->getTag())
            {
                toRemove.push_back(bodyA);
            }
        }
    }
    
    for (int i = 0; i < toRemove.size(); i++)
    {
        killEnemy(toRemove[i]);
    }
}

void GameScene_Rope::killEnemy(b2Body * pBody)
{
    removeChild((Sprite *)pBody->GetUserData());
    m_pWorld->DestroyBody(pBody);
    throwObject();
}