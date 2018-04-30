#include "GameScene_Cops.h"
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

GameScene_Cops::GameScene_Cops()
{
    srand(time(NULL));
    m_bDone = false;
}

GameScene_Cops::~GameScene_Cops()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Cops::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Cops *layer = GameScene_Cops::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Accelerometer methods
// ===========================================================

void GameScene_Cops::onAcceleration(cocos2d::Acceleration *acc, cocos2d::Event *event)
{
    if (fabs(m_pPlayerBody->GetLinearVelocity().y) < 0.1f)
    {
        m_pPlayerBody->ApplyForceToCenter(b2Vec2(MOVE_SPPED * acc->x, 0), true);
    }
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_Cops::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_Cops::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_Cops::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_Cops::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Cops::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
    
    Device::setAccelerometerEnabled(true);
    auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameScene_Cops::onAcceleration, this));
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene_Cops::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        checkCollisions();
        updateWorld(dt);
    }
}

bool GameScene_Cops::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_initialTouchPos = pTouch->getLocation();
    m_fInitialTouchTime = getTime();
    
    return true;
}

void GameScene_Cops::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
   
}

void GameScene_Cops::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    float dt = getTime() - m_fInitialTouchTime;
    if (dt < 0.3f)
    {
        Vec2 loc = pTouch->getLocation();
        float dx = loc.x - m_initialTouchPos.x;
        float dy = loc.y - m_initialTouchPos.y;
        if (fabs(dx) > fabs(dy)) // move more to horizontal than vertical
        {
            if (dx > 0) // swipe right
            {
                onSwipeRight();
            }
            else // swipe left
            {
                onSwipeLeft();
            }
        }
        else // more vertical
        {
            if (dy > 0) // swipe right
            {
                onSwipeUp();
            }
            else // swipe left
            {
                onSwipeDown();
            }
        }
    }
}

void GameScene_Cops::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

void GameScene_Cops::onSwipeLeft()
{
    
}

void GameScene_Cops::onSwipeRight()
{
}

void GameScene_Cops::onSwipeUp()
{
    if (fabs(m_pPlayerBody->GetLinearVelocity().y) < 0.1f)
    {
        m_pPlayerBody->ApplyLinearImpulse(b2Vec2(0, JUMP_SPEED), m_pPlayerBody->GetWorldCenter(), true);
    }
}

void GameScene_Cops::onSwipeDown()
{
    
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Cops::buildWorld()
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
    groundFixDef.filter.categoryBits = CAT_GROUND;
    groundFixDef.filter.maskBits = CAT_ENEMY | CAT_PLAYER;
    pGround->CreateFixture(&groundFixDef);
    
    // add walls
    {
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), 5, h);
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
        groundFixDef.filter.categoryBits = CAT_WALL;
        groundFixDef.filter.maskBits = CAT_PLAYER;
        pGround->CreateFixture(&groundFixDef);
    }
    {
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), 5, h*0.2f);
        pGroundTile->setPosition(Vec2(w * 0.85f, 0));
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
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_ENEMY | CAT_PLAYER;
        pGround->CreateFixture(&groundFixDef);
    }
    
    buildPlayer(pGroundTile->boundingBox().size.height);
    
    throwObject();
}

void GameScene_Cops::updateWorld(float dt)
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
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
            
            if (TAG_LAYER == sprite->getTag() || TAG_PLAYER == sprite->getTag())
            {
                sprite->setPositionX(sprite->getPositionX() - sprite->boundingBox().size.width/2);
                sprite->setPositionY(sprite->getPositionY() - sprite->boundingBox().size.height/2);
            }            
        }
    }
}

void GameScene_Cops::buildPlayer(float groundY)
{
    float w = getScreenWidth();
    
    Color3B c = Pref::getInstance()->getColor(COLOR_FILL);
    m_pPlayer = LayerColor::create(ccc4(c.r, c.g, c.b, 255), w * 0.1f, w * 0.1f * 1.5f);
    m_pPlayer->setTag(TAG_PLAYER);
    m_pPlayer->setPosition(Vec2(w * 0.05f, groundY));
    addChild(m_pPlayer);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = m_pPlayer;
    bodyDef.position.Set(
         (m_pPlayer->getPositionX() + m_pPlayer->boundingBox().size.width / 2)/PTM_RATIO,
         (m_pPlayer->getPositionY() + m_pPlayer->boundingBox().size.height / 2)/PTM_RATIO
    );
    m_pPlayerBody = m_pWorld->CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox((m_pPlayer->boundingBox().size.width/2)/PTM_RATIO, (m_pPlayer->boundingBox().size.height/2)/PTM_RATIO);
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 1.0f;
    shapeDef.restitution = 0.0f;
    shapeDef.filter.categoryBits = CAT_PLAYER;
    shapeDef.filter.maskBits = CAT_ENEMY | CAT_GROUND | CAT_WALL;
    m_pPlayerBody->CreateFixture(&shapeDef);
}

void GameScene_Cops::throwObject()
{
    Sprite * pSprite = Sprite::create("circle.png");
    pSprite->setScale(0.2f);
    pSprite->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pSprite->setTag(TAG_ENEMY);
    pSprite->setPosition(Vec2(getScreenWidth() * 0.9f, getScreenHeight() * 0.4f));
    addChild(pSprite);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = pSprite;
    bodyDef.position.Set(pSprite->getPositionX()/PTM_RATIO, pSprite->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pSprite->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    shapeDef.filter.categoryBits = CAT_ENEMY;
    shapeDef.filter.maskBits = CAT_PLAYER | CAT_GROUND;
    pBody->CreateFixture(&shapeDef);
    
    pBody->ApplyLinearImpulse(b2Vec2(-50, 60), pBody->GetWorldCenter(), true);
}

void GameScene_Cops::checkCollisions()
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

void GameScene_Cops::killEnemy(b2Body * pBody)
{
    removeChild((Sprite *)pBody->GetUserData());
    m_pWorld->DestroyBody(pBody);
    throwObject();
}