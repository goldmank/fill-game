#include "GameScene_FlyGuy.h"
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

#define TAG_WALL   (0)
#define TAG_GROUND (1)
#define TAG_JUMPER (2)
#define TAG_ENEMY  (3)
#define TAG_PLAYER (4)

#define FLY_SPEED   (170)
#define GRAVITY_Y   (25)
#define ENEMY_SPEED (6.0f)
#define MAX_SPEED   (4)

#define CAT_GROUND  0x0001
#define CAT_WALL    0x0002
#define CAT_PLAYER  0x0004
#define CAT_ENEMY   0x0008
#define CAT_BULLET  0x0016

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_FlyGuy::GameScene_FlyGuy()
{
    srand(time(NULL));
    m_bDone = false;
    m_bFlying = false;
    m_fEnemyActiveTimer = 0;
    m_fBounsTimer = 0;
    m_pBouns = NULL;
    m_iBounsCount = 0;
}

GameScene_FlyGuy::~GameScene_FlyGuy()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_FlyGuy::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_FlyGuy *layer = GameScene_FlyGuy::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_FlyGuy::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_FlyGuy::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_FlyGuy::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_FlyGuy::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_FlyGuy::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
    
    Device::setAccelerometerEnabled(true);
    auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameScene_FlyGuy::onAcceleration, this));
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    m_pBounsText = CCLabelTTF::create("0", fontList[0], screenHeight * 0.04f);
    m_pBounsText->setPositionY(getScreenHeight() * 0.95f);
    addChild(m_pBounsText);
    
    updateBounsText();
}

void GameScene_FlyGuy::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        if (m_bFlying)
        {
            m_pPlayerBody->ApplyForceToCenter(b2Vec2(0, FLY_SPEED), true);
        }
        
        updateWorld(dt);
        checkCollisions();
        
        m_fEnemyActiveTimer += dt;
        int d = m_bEnemyActive ? 1 : 2;
        if (m_fEnemyActiveTimer > d)
        {
            m_fEnemyActiveTimer = 0;
            if (m_bEnemyActive)
            {
                m_bEnemyActive = false;
                m_pEnemy->setColor(ccc3(255, 255, 255));
            }
            else
            {
                m_bEnemyActive = true;
                m_pEnemy->setColor(ccc3(255, 0, 0));
            }
        }
        
        m_fBounsTimer += dt;
        d = NULL == m_pBouns ? 2 : 3;
        if (m_fBounsTimer > d)
        {
            m_fBounsTimer = 0;
            if (NULL == m_pBouns)
            {
                addBouns((float)(getScreenHeight() * 0.2f + (int)(rand() % (int)(getScreenHeight() * 0.6f))));
            }
            else
            {
                removeChild(m_pBouns);
                m_pBouns = NULL;
            }
        }
    }
}

bool GameScene_FlyGuy::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_bFlying = true;
    
    return true;
}

void GameScene_FlyGuy::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
}

void GameScene_FlyGuy::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    m_bFlying = false;
}

void GameScene_FlyGuy::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_FlyGuy::buildWorld()
{
    float h = getScreenHeight();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -GRAVITY_Y);
    m_pWorld = new b2World(gravity);
    m_pWorld->SetContactListener(this);
    
    buildEnemy();
    buildPlayer(h * 0.5f);
}

void GameScene_FlyGuy::updateWorld(float dt)
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
            
            if (TAG_WALL == sprite->getTag() || TAG_GROUND == sprite->getTag() || TAG_JUMPER == sprite->getTag())
            {
                sprite->setPositionX(sprite->getPositionX() - sprite->boundingBox().size.width/2);
                sprite->setPositionY(sprite->getPositionY() - sprite->boundingBox().size.height/2);
            }
        }
    }
}

void GameScene_FlyGuy::buildPlayer(float groundY)
{
    float w = getScreenWidth();
    
    m_pPlayer = Sprite::create("rect.png");
    m_pPlayer->setScale(0.32f, 0.4);
    m_pPlayer->setTag(TAG_PLAYER);
    m_pPlayer->setPosition(Vec2(w * 0.5f, groundY + m_pPlayer->boundingBox().size.height / 2));
    addChild(m_pPlayer);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = m_pPlayer;
    bodyDef.position.Set(m_pPlayer->getPositionX()/PTM_RATIO, m_pPlayer->getPositionY()/PTM_RATIO);
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
}

void GameScene_FlyGuy::buildEnemy()
{
    m_pEnemy = LayerColor::create(ccc4(255, 255, 255, 255), getScreenWidth(), getScreenHeight() * 0.03f);
    m_pEnemy->setPosition(Vec2(0, getScreenHeight() * 0.8f));
    addChild(m_pEnemy);
    
    MoveTo * pMoveTo1 = MoveTo::create(ENEMY_SPEED, Vec2(0, getScreenHeight() * 0.2f));
    MoveTo * pMoveTo2 = MoveTo::create(ENEMY_SPEED, Vec2(0, getScreenHeight() * 0.8f));
    CCSequence * pMove = CCSequence::createWithTwoActions(EaseSineInOut::create(pMoveTo1), EaseSineInOut::create(pMoveTo2));
    m_pEnemy->runAction(CCRepeatForever::create(pMove));
}

void GameScene_FlyGuy::addBouns(float y)
{
    m_pBouns = Sprite::create("rect.png");
    m_pBouns->setColor(ccc3(255, 248, 68));
    m_pBouns->setScale(0.32f);
    m_pBouns->setPosition(Vec2(getScreenWidth() * 0.5f, y));
    addChild(m_pBouns);
}

void GameScene_FlyGuy::checkCollisions()
{
    if (m_pPlayer->getPositionY() < -m_pPlayer->boundingBox().size.height/2)
    {
        onGameOver();
        return;
    }
    
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
        }
    }
    
    if (playerHit)
    {
        //onPlayerHit();
    }
    
    for (int i = 0; i < toRemove.size(); i++)
    {
        //killEnemy(toRemove[i]);
    }
    
    if (NULL != m_pBouns)
    {
        float dy = fabsf(m_pBouns->getPositionY() - m_pPlayer->getPositionY());
        if (dy < m_pBouns->boundingBox().size.height / 2)
        {
            m_fBounsTimer = 0;
            removeChild(m_pBouns);
            m_pBouns = NULL;
            m_iBounsCount++;
            updateBounsText();
        }
    }
    if (m_bEnemyActive)
    {
        float dy = fabsf(m_pEnemy->getPositionY() - m_pPlayer->getPositionY());
        if (dy < m_pEnemy->boundingBox().size.height / 2)
        {
            onGameOver();
        }
    }
}

void GameScene_FlyGuy::onGameOver()
{
    m_bDone = true;
    
    m_pEnemy->stopAllActions();
    
    DelayTime * pDelay = DelayTime::create(0.5f);
    CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_FlyGuy::restartGame));
    runAction(CCSequence::createWithTwoActions(pDelay, pFillDone));
}

void GameScene_FlyGuy::restartGame(cocos2d::Node * pSender)
{
    switchScene(GameScene_FlyGuy::scene());
}

void GameScene_FlyGuy::updateBounsText()
{
    char text[10];
    sprintf(text, "%d", m_iBounsCount);
    m_pBounsText->setString(text);
    m_pBounsText->setPositionX(getScreenWidth() * 0.04f + m_pBounsText->boundingBox().size.width * 0.5f);
}