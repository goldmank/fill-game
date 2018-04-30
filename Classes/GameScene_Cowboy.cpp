#include "GameScene_Cowboy.h"
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

#define MOVE_SPPED (300)
#define JUMP_SPEED (30)
#define GRAVITY_Y  (20)
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

GameScene_Cowboy::GameScene_Cowboy()
{
    srand(time(NULL));
    m_bDone = false;
    m_fAddEnemyTimer = 0;
    m_bShooting = false;
    m_fAddBulletTimer = 0;
    m_iScore = 0;
}

GameScene_Cowboy::~GameScene_Cowboy()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Cowboy::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Cowboy *layer = GameScene_Cowboy::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_Cowboy::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_Cowboy::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_Cowboy::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_Cowboy::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Cowboy::handleInit(float screenWidth, float screenHeight)
{
    buildWorld();
    
    Device::setAccelerometerEnabled(true);
    auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(GameScene_Cowboy::onAcceleration, this));
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    m_pScoreText = CCLabelTTF::create("0", fontList[0], screenHeight * 0.04f);
    m_pScoreText->setPositionY(getScreenHeight() * 0.95f);
    addChild(m_pScoreText);
    
    updateScoreText();
}

void GameScene_Cowboy::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        m_fAddEnemyTimer += dt;
        if (m_fAddEnemyTimer > NEW_ENEMY_TIME)
        {
            m_fAddEnemyTimer = 0;
            addEnemy(getScreenWidth() * 0.1f + rand() % (int)(getScreenWidth() * 0.8f),
                     getScreenHeight() * 0.2f + rand() % (int)(getScreenHeight() * 0.7f));
        }
        
        updateWorld(dt);
        checkCollisions();
        
        if (m_bShooting && fabsf(m_pPlayerBody->GetLinearVelocity().x) < 0.01f)
        {
            m_fAddBulletTimer += dt;
            if (m_fAddBulletTimer > 0.1f)
            {
                m_fAddBulletTimer = 0;
                
                Sprite * pBullet = Sprite::create("circle.png");
                pBullet->setScale(0.05f);
                pBullet->setColor(ccc3(250, 250, 60));
                pBullet->setPosition(m_shootPoint);
                addChild(pBullet);
                
                FadeTo * pFade = FadeTo::create(0.2f, 0);
                CCFiniteTimeAction* pAnimDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_Cowboy::onShootBulletDone));
                pBullet->runAction(Sequence::createWithTwoActions(pFade, pAnimDone));
                
                for (int i = 0; i < getChildrenCount(); i++)
                {
                    Node * pChild = (Node *)getChildren().at(i);
                    if (TAG_ENEMY == pChild->getTag())
                    {
                        float dx = fabsf(pChild->getPositionX() - m_shootPoint.x);
                        float dy = fabsf(pChild->getPositionY() - m_shootPoint.y);
                        if (dx < pBullet->boundingBox().size.width / 2 && dy < pBullet->boundingBox().size.height / 2)
                        {
                            hitEnemy(pChild);
                            m_iScore++;
                            updateScoreText();
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool GameScene_Cowboy::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    float dx = fabsf(m_pPlayer->getPositionX() - pTouch->getLocation().x);
    float dy = fabsf(m_pPlayer->getPositionY() - pTouch->getLocation().y);
    if (dx < m_pPlayer->boundingBox().size.width / 2 && dy < m_pPlayer->boundingBox().size.height / 2)
    {
        m_bShooting = false;
        
        m_initialTouchPos = pTouch->getLocation();
        m_fInitialTouchTime = getTime();
    }
    else
    {
        m_bShooting = true;
        m_fAddBulletTimer = 1;
        m_shootPoint = pTouch->getLocation();
    }
    
    return true;
}

void GameScene_Cowboy::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    if (m_bShooting)
    {
        m_shootPoint = pTouch->getLocation();
    }
}

void GameScene_Cowboy::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    if (!m_bShooting)
    {
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
        }
    }
    m_bShooting = false;
}

void GameScene_Cowboy::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

void GameScene_Cowboy::onSwipeLeft()
{
    m_pPlayerBody->ApplyLinearImpulse(b2Vec2(-JUMP_SPEED, 0), m_pPlayerBody->GetWorldCenter(), true);
}

void GameScene_Cowboy::onSwipeRight()
{
    m_pPlayerBody->ApplyLinearImpulse(b2Vec2(JUMP_SPEED, 0), m_pPlayerBody->GetWorldCenter(), true);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Cowboy::buildWorld()
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

void GameScene_Cowboy::updateWorld(float dt)
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
    
    for (int i = 0; i < outEnemies.size(); i++)
    {
        killEnemy(outEnemies[i]);
    }
}

void GameScene_Cowboy::buildPlayer(float groundY)
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

void GameScene_Cowboy::checkCollisions()
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

void GameScene_Cowboy::killEnemy(b2Body * pBody)
{
    Sprite * pSprite = (Sprite *)pBody->GetUserData();
    if (NULL != pSprite->getUserData())
    {
//        if (TAG_ENEMY == pSprite->getTag())
//        {
//            delete (EnemyDetails *)pSprite->getUserData();
//        }
        pSprite->setUserData(NULL);
    }
    removeChild(pSprite);
    m_pWorld->DestroyBody(pBody);
}

void GameScene_Cowboy::shoot()
{
    
}

void GameScene_Cowboy::addEnemy(float x, float y)
{
    Sprite * pEnemy = Sprite::create("rect.png");
    pEnemy->setScale(0.2f);
    pEnemy->setColor(ccc3(255, 130, 0));
    pEnemy->setTag(TAG_ENEMY);
    pEnemy->setPosition(Vec2(x, y));
    addChild(pEnemy);
}

void GameScene_Cowboy::hitEnemy(cocos2d::Node * pEnemy)
{
    removeChild(pEnemy);
}

void GameScene_Cowboy::onPlayerHit()
{
    
}

void GameScene_Cowboy::onShootBulletDone(cocos2d::Node * pSender)
{
    removeChild(pSender);
}

void GameScene_Cowboy::updateScoreText()
{
    char text[10];
    sprintf(text, "%d", m_iScore);
    m_pScoreText->setString(text);
    m_pScoreText->setPositionX(getScreenWidth() * 0.04f + m_pScoreText->boundingBox().size.width * 0.5f);
}