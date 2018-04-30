#include "GameScene_RunUp.h"
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
#define TAG_STAR   (5)

#define MOVE_SPPED (100)
#define JUMP_SPEED (25)
#define GRAVITY_Y  (20)
#define MAX_SPEED  (4)
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

GameScene_RunUp::GameScene_RunUp()
{
    srand(time(NULL));
    m_bDone = false;
    m_fMoveXSpeed = 0;
    m_bJumping = false;
    m_fLastDy = 0;
    m_iCurrGround = 0;
    m_fLastJumpTime = 0;
    m_iStars = 0;
    m_fAddEnemyCounter = 0;
    m_fHitSafeCounter = 0;
}

GameScene_RunUp::~GameScene_RunUp()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_RunUp::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_RunUp *layer = GameScene_RunUp::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_RunUp::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_RunUp::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_RunUp::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_RunUp::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_RunUp::handleInit(float screenWidth, float screenHeight)
{
    m_pGameLayer = Layer::create();
    addChild(m_pGameLayer);
    
    LayerColor * pPanel = LayerColor::create(ccc4(180, 180, 180, 255), getScreenWidth(), getScreenHeight() * 0.1f);
    addChild(pPanel);
    
    m_pDistanceText = CCLabelTTF::create("0", fontList[0], screenHeight * 0.04f);
    m_pDistanceText->setColor(ccc3(255, 255, 255));
    m_pDistanceText->setPosition(ccp(getScreenWidth() * 0.25f, pPanel->boundingBox().size.height / 2));
    addChild(m_pDistanceText);
    
    m_pStarsText = CCLabelTTF::create("0", fontList[0], screenHeight * 0.04f);
    m_pStarsText->setColor(ccc3(255, 255, 255));
    m_pStarsText->setPosition(ccp(getScreenWidth() * 0.75f, pPanel->boundingBox().size.height / 2));
    addChild(m_pStarsText);
    
    buildWorld(pPanel->boundingBox().size.height);
}

void GameScene_RunUp::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        if (m_fMoveXSpeed != 0)
        {
            m_pPlayerBody->ApplyForceToCenter(b2Vec2(m_fMoveXSpeed, 0), true);
        }
        CCLOG("m_fMoveXSpeed: %.2f", m_fMoveXSpeed);
        
        updateWorld(dt);
        checkCollisions();
        
        m_fAddEnemyCounter += dt;
        if (m_fAddEnemyCounter > 2)
        {
            m_fAddEnemyCounter = 0;
            int t = m_iCurrGround + 8;
            if (t >= m_grounds.size())
            {
                t = (int)m_grounds.size() - 1;
            }
            float y = m_grounds[t].m_bodies[0]->GetPosition().y * PTM_RATIO - getScreenHeight() * 0.1f;
            addEnemy(y);
        }
        
        if (m_fHitSafeCounter > 0)
        {
            m_fHitSafeCounter -= dt;
            if (m_fHitSafeCounter <= 0)
            {
                onPlayerRestartAfterHit();
            }
        }
    }
}

bool GameScene_RunUp::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return true;
    }
    
    m_fMoveXSpeed = 0;
    m_fLastDy = 0;
    m_bJumping = false;
    
    return true;
}

void GameScene_RunUp::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
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
    
    float dy = start.y - loc.y;
    if (start.y - loc.y > getScreenHeight() * 0.1f && dy - m_fLastDy >= 0)
    {
        m_bJumping = true;
    }
    else
    {
        m_bJumping = false;
    }
    m_fLastDy = dy;
}

void GameScene_RunUp::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        return;
    }
    
    m_fMoveXSpeed = 0;
    m_bJumping = false;
}

void GameScene_RunUp::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

b2Body * GameScene_RunUp::createGroundBody(float x, float y, float w, float h, Color3B color, int tag, float angle)
{
    // Define the ground body
    LayerColor * pGroundTile = LayerColor::create(ccc4(color.r, color.g, color.b, 255), w, h);
    pGroundTile->setPosition(Vec2(x, y));
    pGroundTile->setTag(tag);
    m_pGameLayer->addChild(pGroundTile);
    
    b2BodyDef groundBodyDef;
    groundBodyDef.userData = pGroundTile;
    groundBodyDef.position.Set(
                               (pGroundTile->getPositionX() + pGroundTile->boundingBox().size.width / 2)/PTM_RATIO,
                               (pGroundTile->getPositionY() + pGroundTile->boundingBox().size.height / 2)/PTM_RATIO
                               );
    groundBodyDef.angle = angle;
    b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
    
    b2PolygonShape groundShape;
    groundShape.SetAsBox((pGroundTile->boundingBox().size.width/2)/PTM_RATIO, (pGroundTile->boundingBox().size.height/2)/PTM_RATIO);
    
    b2FixtureDef groundFixDef;
    groundFixDef.shape = &groundShape;
    groundFixDef.filter.categoryBits = CAT_GROUND;
    groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
    pGround->CreateFixture(&groundFixDef);
    
    return pGround;
}

GameScene_RunUp::GroundDetails GameScene_RunUp::addGround(float y, int * values, float angle)
{
    float w = getScreenWidth();
    float h = getScreenHeight();
    
    // Define the ground body
    Color3B gc = Pref::getInstance()->getColor(COLOR_BG);
    Color3B jc = Pref::getInstance()->getColor(COLOR_ENEMY);
    float step = w / 10;
    float gh = h * 0.015f;
    
    GroundDetails g;
    
    float x1 = 0;
    float x2 = 0;
    for (int i = 0; i < 10; i++)
    {
        if (0 != values[i]) // hole or jumper
        {
            if (i > 0 && values[i - 1] == 0) // complete ground
            {
                g.m_bodies.push_back(createGroundBody(x1, y, x2 - x1, gh, gc, TAG_GROUND, angle));
            }
            x1 = x2;
            x2 = x1 + step;
            if (1 == values[i]) // jumper
            {
                g.m_bodies.push_back(createGroundBody(x1, y, x2 - x1, gh, jc, TAG_JUMPER, angle));
            }
            else
            {
                g.m_holes.push_back(x1 + step / 2);
            }
            x1 = x2;
        }
        else
        {
            x2 += step;
        }
    }
    if (0 == values[9])
    {
        g.m_bodies.push_back(createGroundBody(x1, y, x2 - x1, gh, gc, TAG_GROUND, angle));
    }
   
    return g;
}

void GameScene_RunUp::buildWorld(float groundY)
{
    float h = getScreenHeight();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -GRAVITY_Y);
    m_pWorld = new b2World(gravity);
    m_pWorld->SetContactListener(this);
    
    float y = groundY;
    int values[10];
    for (int i = 0; i < 35; i++)
    {
        memset(values, 0, sizeof(values));
        if (0 == i)
        {
            values[0] = 2;
            values[2] = 1;
        }
        else
        {
            int r = 2 + rand() % 7;
            values[r] = 1;
            int t = rand() % 10;
            while (t == r) { t = rand() % 10; }
            values[t] = 2;
        }
        float angle = 0;
        if (i > 0)
        {
            angle = i%2==0 ? 0.03f : -0.03f;
        }
        GroundDetails gd = addGround(y, values, 0);
        m_grounds.push_back(gd);
        
        if (rand()%2 == 0)
        {
            Sprite * pStar = Sprite::create("circle.png");
            pStar->setColor(ccc3(200, 100, 150));
            pStar->setScale(0.06f);
            pStar->setTag(TAG_STAR);
            pStar->setPosition(getScreenWidth() * 0.1f + (rand() % (int)(getScreenWidth() * 0.7f)), y + pStar->boundingBox().size.height * 2);
            m_pGameLayer->addChild(pStar);
        }
        
        y += getScreenHeight() * 0.15f;
    }
    
    // add walls
    float wallsX[2];
    wallsX[0] = 0;
    wallsX[1] = getScreenWidth() - 5;
    for (int i = 0; i < 2; i++)
    {
        Color3B border = Pref::getInstance()->getColor(COLOR_BG);
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), 5, h);
        pGroundTile->setPosition(Vec2(wallsX[i], 0));
        pGroundTile->setTag(TAG_WALL);
        m_pGameLayer->addChild(pGroundTile);
        
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
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
        
        m_pWalls.push_back(pGround);
    }
    
    buildPlayer(groundY + h * 0.016f);
}

void GameScene_RunUp::updateWorld(float dt)
{
    float py = m_pPlayer->getPositionY();
    if (py - m_pGameLayer->getPositionY() > getScreenHeight() / 2)
    {
        m_pGameLayer->setPositionY(-(py - getScreenHeight() / 2));
    }
    
    // move walls with layer
    for (int i = 0; i < m_pWalls.size(); i++)
    {
        b2Body * pBody = m_pWalls[i];
        b2Vec2 pos = pBody->GetPosition();
        pos.y = (-m_pGameLayer->getPositionY() + getScreenHeight() / 2) / PTM_RATIO;
        pBody->SetTransform(pos, 0);
    }
    
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
            else if (TAG_ENEMY == sprite->getTag())
            {
                EnemyDetails * pData = (EnemyDetails *)sprite->getUserData();
                
                b2Vec2 v = b->GetLinearVelocity();
                if (fabsf(v.y) < 0.01f) // stading on ground
                {
                    if (0 == pData->m_iDirection)
                    {
                        for (int i = 0; i < m_grounds.size(); i++)
                        {
                            for (int j = 0; j < m_grounds[i].m_bodies.size(); j++)
                            {
                                if (m_grounds[i].m_holes.empty())
                                {
                                    continue;
                                }
                                float groundY = m_grounds[i].m_bodies[j]->GetPosition().y * PTM_RATIO;
                                if (fabsf(groundY - sprite->getPositionY()) < sprite->boundingBox().size.height)
                                {
                                    float dx = m_grounds[i].m_holes[0] - sprite->getPositionX();
                                    if (fabsf(dx) < sprite->boundingBox().size.width / 2)
                                    {
                                        pData->m_iDirection = 0;
                                    }
                                    else
                                    {
                                        if (dx > 0)
                                        {
                                            pData->m_iDirection = 1;
                                        }
                                        else if (dx < 0)
                                        {
                                            pData->m_iDirection = -1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    v.x = pData->m_iDirection * 5;
                }
                else // falling
                {
                    v.x = 0;
                    pData->m_iDirection = 0;
                }
                b->SetLinearVelocity(v);
                
                if (sprite->getPositionY() + sprite->boundingBox().size.height / 2 < m_grounds[0].m_bodies[0]->GetPosition().y * PTM_RATIO)
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
    
    if (m_pPlayerBody->GetLinearVelocity().y < 0)
    {
        for (int i = 0; i < m_grounds.size(); i++)
        {
            b2Body * pBody = m_grounds[i].m_bodies[0];
            float groundY = pBody->GetPosition().y * PTM_RATIO;
            bool active = i == 0 || groundY < m_pPlayer->getPositionY() - m_pPlayer->boundingBox().size.height * 0.5f;
            for (int j = 0; j < m_grounds[i].m_bodies.size(); j++)
            {
                b2Body * pBody = m_grounds[i].m_bodies[j];
                b2Fixture * pFixture = pBody->GetFixtureList();
                
                b2Filter filter;
                memset(&filter, 0, sizeof(filter));
                filter.categoryBits = CAT_GROUND;
                
                if (active)
                {
                    m_iCurrGround = i;
                    filter.maskBits = CAT_PLAYER | CAT_ENEMY;
                }
                else
                {
                    filter.maskBits = CAT_ENEMY;
                }
                pFixture->SetFilterData(filter);
            }
        }
    }
}

void GameScene_RunUp::buildPlayer(float groundY)
{
    float w = getScreenWidth();
    
    m_pPlayer = Sprite::create("rect.png");
    m_pPlayer->setScale(0.32f, 0.4);
    m_pPlayer->setTag(TAG_PLAYER);
    m_pPlayer->setPosition(Vec2(w * 0.5f, groundY + m_pPlayer->boundingBox().size.height / 2));
    m_pGameLayer->addChild(m_pPlayer);
    m_pPlayer->setColor(ccc3(255, 155, 255));
    
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

void GameScene_RunUp::checkCollisions()
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
            if (TAG_PLAYER == pSpriteA->getTag() && TAG_GROUND == pSpriteB->getTag())
            {
                if (m_bJumping)
                {
                    doJump();
                }
            }
            else if (TAG_PLAYER == pSpriteB->getTag() && TAG_GROUND == pSpriteA->getTag())
            {
                if (m_bJumping)
                {
                    doJump();
                }
            }
            else if (TAG_PLAYER == pSpriteA->getTag() && TAG_JUMPER == pSpriteB->getTag())
            {
                if (m_pPlayerBody->GetLinearVelocity().y <= 0)
                {
                    float dx = fabsf(pSpriteA->getPositionX() - (pSpriteB->getPositionX() + pSpriteB->boundingBox().size.width / 2));
                    if (dx < pSpriteB->boundingBox().size.width / 2)
                    {
                        m_pPlayerBody->ApplyLinearImpulse(b2Vec2(0, JUMP_SPEED*2.0f), m_pPlayerBody->GetWorldCenter(), true);
                    }
                }
            }
            else if (TAG_PLAYER == pSpriteB->getTag() && TAG_JUMPER == pSpriteA->getTag())
            {
                if (m_pPlayerBody->GetLinearVelocity().y <= 0)
                {
                    float dx = fabsf(pSpriteB->getPositionX() - (pSpriteA->getPositionX() + pSpriteA->boundingBox().size.width / 2));
                    if (dx < pSpriteA->boundingBox().size.width / 2)
                    {
                        m_pPlayerBody->ApplyLinearImpulse(b2Vec2(0, JUMP_SPEED*2.0f), m_pPlayerBody->GetWorldCenter(), true);
                    }
                }
            }
        }
    }
    
    for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
    {
        if (NULL != b->GetUserData())
        {
            Node* sprite = (Node*)b->GetUserData();
            if (TAG_ENEMY == sprite->getTag())
            {
                float dx = fabsf(m_pPlayer->getPositionX() - sprite->getPositionX());
                float dy = fabsf(m_pPlayer->getPositionY() - sprite->getPositionY());
                if (dx < m_pPlayer->boundingBox().size.width / 2 && dy < m_pPlayer->boundingBox().size.height / 2)
                {
                    if (m_pPlayerBody->GetLinearVelocity().y > 0.01f)
                    {
                        
                    }
                    else if (m_pPlayerBody->GetLinearVelocity().y < -0.01f)
                    {
                        killEnemy(b);
                    }
                    else if (0 == m_fHitSafeCounter)
                    {
                        onPlayerHit();
                    }
                    break;
                }
            }
        }
    }
    
    float px = m_pPlayer->getPositionX();
    float py = m_pPlayer->getPositionY();
    for (int i = 0; i < m_pGameLayer->getChildrenCount(); i++)
    {
        Node * pStar = (Node *)m_pGameLayer->getChildren().at(i);
        if (TAG_STAR == pStar->getTag())
        {
            float dx = fabsf(px - pStar->getPositionX());
            float dy = fabsf(py - pStar->getPositionY());
            if (dx < m_pPlayer->boundingBox().size.width / 2 && dy < pStar->boundingBox().size.height / 2)
            {
                onStartCollected(pStar);                
                break;
            }
        }
        //killEnemy(toRemove[i]);
    }
}

void GameScene_RunUp::doJump()
{
    if (getTime() - m_fLastJumpTime > 0.5f)
    {
        m_fLastJumpTime = getTime();
        m_pPlayerBody->ApplyLinearImpulse(b2Vec2(0, JUMP_SPEED), m_pPlayerBody->GetWorldCenter(), true);
    }
}

void GameScene_RunUp::addEnemy(float y)
{
    float w = getScreenWidth();
    
    Sprite * pEnemy = Sprite::create("rect.png");
    pEnemy->setScale(0.22f, 0.3);
    pEnemy->setTag(TAG_ENEMY);
    pEnemy->setPosition(Vec2(w * 0.5f, y));
    m_pGameLayer->addChild(pEnemy);
    
    EnemyDetails * pData = new EnemyDetails();
    pData->m_iDirection = 0;
    pEnemy->setUserData(pData);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = pEnemy;
    bodyDef.position.Set(pEnemy->getPositionX()/PTM_RATIO, pEnemy->getPositionY()/PTM_RATIO);
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    pBody->SetFixedRotation(true);
    
    b2PolygonShape shape;
    shape.SetAsBox((pEnemy->boundingBox().size.width/2)/PTM_RATIO, (pEnemy->boundingBox().size.height/2)/PTM_RATIO);
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 1.0f;
    shapeDef.restitution = 0.0f;
    shapeDef.filter.categoryBits = CAT_ENEMY;
    shapeDef.filter.maskBits = CAT_WALL | CAT_GROUND;
    pBody->CreateFixture(&shapeDef);
}

void GameScene_RunUp::killEnemy(b2Body * pBody)
{
    Sprite * pSprite = (Sprite *)pBody->GetUserData();
    EnemyDetails * pData = (EnemyDetails *)pSprite->getUserData();
    delete pData;
    m_pGameLayer->removeChild(pSprite);
    m_pWorld->DestroyBody(pBody);
}

void GameScene_RunUp::onPlayerHit()
{
    m_fHitSafeCounter = 2;
    m_pPlayer->setColor(ccc3(255, 0, 0));
}

void GameScene_RunUp::onPlayerRestartAfterHit()
{
    m_fHitSafeCounter = 0;
    m_pPlayer->setColor(ccc3(255, 155, 255));
}

void GameScene_RunUp::onStartCollected(cocos2d::Node * pStar)
{
    m_pGameLayer->removeChild(pStar);
    m_iStars++;
    
    char text[10];
    sprintf(text, "%d", m_iStars);
    m_pStarsText->setString(text);
}