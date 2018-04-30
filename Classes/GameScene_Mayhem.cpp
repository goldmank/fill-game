#include "GameScene_Mayhem.h"
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

GameScene_Mayhem::GameScene_Mayhem()
{
    srand(time(NULL));
    m_bDone = false;
    m_fMoveXSpeed = 0;
    m_fGunAngle = 0;
    m_iMoveTouchId = -1;
    m_iShootTouchId = -1;
    m_pGunHolder = NULL;
    m_fAddEnemyTimer = 0;
}

GameScene_Mayhem::~GameScene_Mayhem()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Mayhem::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Mayhem *layer = GameScene_Mayhem::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_Mayhem::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_Mayhem::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_Mayhem::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_Mayhem::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Mayhem::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
    
    Device::setAccelerometerEnabled(true);
    auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameScene_Mayhem::onAcceleration, this));
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene_Mayhem::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        float r = m_pPlayer->boundingBox().size.height;
        float x = cos(m_fGunAngle) * r;
        float y = sin(m_fGunAngle) * r;
        m_pGun->setPositionX(m_pPlayer->boundingBox().size.width / 2 + x);
        m_pGun->setPositionY(m_pPlayer->boundingBox().size.height / 2 + y);
        
        if (m_fMoveXSpeed != 0)
        {
            m_pPlayerBody->ApplyForceToCenter(b2Vec2(m_fMoveXSpeed, 0), true);
        }
        
        if (-1 != m_iShootTouchId)
        {
            m_fBulleteCreateTimer += dt;
            if (m_fBulleteCreateTimer > NEW_BULLETE_TIME)
            {
                m_fBulleteCreateTimer = 0;
                shoot();
            }
        }
        
        m_fAddEnemyTimer += dt;
        if (m_fAddEnemyTimer > NEW_ENEMY_TIME)
        {
            m_fAddEnemyTimer = 0;
            addEnemy();
        }
        
        updateWorld(dt);
        checkCollisions();
    }
}

bool GameScene_Mayhem::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    if (pTouch->getLocation().x < getScreenWidth() / 2)
    {
        m_iMoveTouchId = pTouch->getID();
        m_fMoveXSpeed = 0;
    }
    else
    {
        m_iShootTouchId = pTouch->getID();
        m_fBulleteCreateTimer = 0;
        
        m_pGunHolder = Sprite::create("circle.png");
        m_pGunHolder->setScale(0.2f);
        m_pGunHolder->setOpacity(180);
        m_pGunHolder->setPosition(pTouch->getLocation());
        addChild(m_pGunHolder);
    }
    
    return true;
}

void GameScene_Mayhem::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    if (m_iMoveTouchId == pTouch->getID())
    {
        Vec2 start = pTouch->getLocation();
        Vec2 loc = pTouch->getStartLocation();
        float dx = loc.x - start.x;
        if (dx > 0)
        {
            m_fMoveXSpeed = -MOVE_SPPED;
        }
        else if (dx < 0)
        {
            m_fMoveXSpeed = MOVE_SPPED;
        }
    }
    else if (m_iShootTouchId == pTouch->getID())
    {
        Vec2 start = pTouch->getLocation();
        Vec2 loc = pTouch->getStartLocation();
        float dx = loc.x - start.x;
        float dy = loc.y - start.y;
        m_fGunAngle = M_PI + atan2f(dy, dx);
    }
}

void GameScene_Mayhem::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    if (m_iMoveTouchId == pTouch->getID())
    {
        m_fMoveXSpeed = 0;
        m_iMoveTouchId = -1;
    }
    else if (m_iShootTouchId == pTouch->getID())
    {
        m_iShootTouchId = -1;
        if (NULL != m_pGunHolder)
        {
            removeChild(m_pGunHolder);
            m_pGunHolder = NULL;
        }
    }
}

void GameScene_Mayhem::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Mayhem::buildWorld()
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
    groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
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
        groundFixDef.filter.categoryBits = CAT_WALL;
        groundFixDef.filter.maskBits = CAT_PLAYER;
        pGround->CreateFixture(&groundFixDef);
    }
    
    buildPlayer(pGroundTile->boundingBox().size.height);
}

void GameScene_Mayhem::updateWorld(float dt)
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
            
            if (TAG_LAYER == sprite->getTag() || TAG_PLAYER == sprite->getTag())
            {
                sprite->setPositionX(sprite->getPositionX() - sprite->boundingBox().size.width/2);
                sprite->setPositionY(sprite->getPositionY() - sprite->boundingBox().size.height/2);
            }
            
            if (TAG_ENEMY == sprite->getTag())
            {
                b2Vec2 v = b->GetLinearVelocity();
                if (fabs(v.y) < 0.5f)
                {
                    EnemyDetails * pEnemyDetails = (EnemyDetails *)sprite->getUserData();
                    if (!pEnemyDetails->m_bRunToPlayer)
                    {
                        pEnemyDetails->m_bRunToPlayer = true;
                        
                        float dx = m_pPlayer->getPositionX() - sprite->getPositionX();
                        if (dx < 0)
                        {
                            b->ApplyLinearImpulse(b2Vec2(-10, 0), b->GetWorldCenter(), true);
                        }
                        else
                        {
                            b->ApplyLinearImpulse(b2Vec2(10, 0), b->GetWorldCenter(), true);
                        }
                    }
                }
                
                if (sprite->getPositionX() + sprite->boundingBox().size.width / 2 < 0 || sprite->getPositionX() - sprite->boundingBox().size.width / 2 > getScreenWidth())
                {
                    outEnemies.push_back(b);
                }
            }
        }
    }
    
    b2Vec2 v = m_pPlayerBody->GetLinearVelocity();
    if (v.x > MAX_SPEED)
    {
        v.x = MAX_SPEED;
        m_pPlayerBody->SetLinearVelocity(v);
    }
    else if (v.x < -MAX_SPEED)
    {
        v.x = -MAX_SPEED;
        m_pPlayerBody->SetLinearVelocity(v);
    }
    
    for (int i = 0; i < outEnemies.size(); i++)
    {
        killEnemy(outEnemies[i]);
    }
}

void GameScene_Mayhem::buildPlayer(float groundY)
{
    float w = getScreenWidth();
    
    Color3B c = Pref::getInstance()->getColor(COLOR_FILL);
    m_pPlayer = LayerColor::create(ccc4(c.r, c.g, c.b, 255), w * 0.06f, w * 0.06f * 1.5f);
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
    m_pPlayerBody->SetFixedRotation(true);
    
    b2PolygonShape shape;
    shape.SetAsBox((m_pPlayer->boundingBox().size.width/2)/PTM_RATIO, (m_pPlayer->boundingBox().size.height/2)/PTM_RATIO);
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 1.0f;
    shapeDef.restitution = 0.0f;
    shapeDef.filter.categoryBits = CAT_PLAYER;
    shapeDef.filter.maskBits = CAT_WALL | CAT_GROUND;
    m_pPlayerBody->CreateFixture(&shapeDef);
    
    m_pGun = Sprite::create("circle.png");
    m_pGun->setScale(0.04f);
    m_pGun->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    m_pPlayer->addChild(m_pGun);
}

void GameScene_Mayhem::checkCollisions()
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
    
    if (playerHit)
    {
        onPlayerHit();
    }
    
    for (int i = 0; i < toRemove.size(); i++)
    {
        killEnemy(toRemove[i]);
    }
}

void GameScene_Mayhem::killEnemy(b2Body * pBody)
{
    Sprite * pSprite = (Sprite *)pBody->GetUserData();
    if (NULL != pSprite->getUserData())
    {
        if (TAG_ENEMY == pSprite->getTag())
        {
            delete (EnemyDetails *)pSprite->getUserData();
        }
        pSprite->setUserData(NULL);
    }
    removeChild(pSprite);
    m_pWorld->DestroyBody(pBody);
}

void GameScene_Mayhem::shoot()
{
    float r = m_pPlayer->boundingBox().size.height;
    float x = cos(m_fGunAngle) * r;
    float y = sin(m_fGunAngle) * r;
    
    Sprite * pSprite = Sprite::create("circle.png");
    pSprite->setScale(0.03f);
    pSprite->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pSprite->setPositionX(m_pPlayer->getPositionX() + m_pPlayer->boundingBox().size.width / 2 + x);
    pSprite->setPositionY(m_pPlayer->getPositionY() + m_pPlayer->boundingBox().size.height / 2 + y);
    pSprite->setTag(TAG_BULLET);
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
    shapeDef.filter.categoryBits = CAT_BULLET;
    shapeDef.filter.maskBits = CAT_ENEMY;
    pBody->CreateFixture(&shapeDef);
    
    pBody->ApplyLinearImpulse(b2Vec2(x, y), pBody->GetWorldCenter(), true);
}

void GameScene_Mayhem::addEnemy()
{
    float x = getScreenWidth() * 0.15f + rand() % getScreenWidth() * 0.75f;
    
    Sprite * pSprite = Sprite::create("circle.png");
    pSprite->setScale(0.1f);
    pSprite->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pSprite->setPositionX(x);
    pSprite->setPositionY(getScreenHeight() + pSprite->boundingBox().size.height / 2);
    pSprite->setTag(TAG_ENEMY);
    addChild(pSprite);
    
    EnemyDetails * pEnemyDetails = new EnemyDetails();
    pEnemyDetails->m_bRunToPlayer = false;
    pSprite->setUserData(pEnemyDetails);
    
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
    shapeDef.restitution = 0.0f;
    shapeDef.filter.categoryBits = CAT_ENEMY;
    shapeDef.filter.maskBits = CAT_BULLET | CAT_GROUND;
    pBody->CreateFixture(&shapeDef);
}

void GameScene_Mayhem::onPlayerHit()
{
    
}