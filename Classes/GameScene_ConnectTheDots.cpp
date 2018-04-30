#include "GameScene_ConnectTheDots.h"
#include "SimpleAudioEngine.h"
#include "Pref.h"
#include "SystemUtils.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define BORDER_SIZE (getScreenHeight() * 0.003f)
#define TAG_RIGHT_WALL (-2)
#define TAG_TOP_WALL (-3)
#define TAG_ENEMEY (1)
#define TAG_PLAYER (2)
#define TAG_STAR (3)

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

GameScene_ConnectTheDots::GameScene_ConnectTheDots()
{
    srand(time(NULL));
}

GameScene_ConnectTheDots::~GameScene_ConnectTheDots()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_ConnectTheDots::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_ConnectTheDots *layer = GameScene_ConnectTheDots::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_ConnectTheDots::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_ConnectTheDots::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_ConnectTheDots::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_ConnectTheDots::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_ConnectTheDots::handleInit(float screenWidth, float screenHeight)
{
    const char * batches[] = { "submarine", NULL };
    int batchIndex = 0;
    while (NULL != batches[batchIndex])
    {
        char pvr[30];
        sprintf(pvr, "%s.pvr", batches[batchIndex]);
        SpriteBatchNode * pGameBatch = SpriteBatchNode::create(pvr);
        if (NULL != pGameBatch)
        {
            addChild(pGameBatch);
            
            char plist[30];
            sprintf(plist, "%s.plist", batches[batchIndex]);
            SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plist);
        }
        
        batchIndex++;
    }
    
    buildWorld();
}

void GameScene_ConnectTheDots::handleUpdate(float dt)
{
    updateWorld(dt);
}

bool GameScene_ConnectTheDots::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pLeftSelected = NULL;
    for (int i = 0; NULL == m_pLeftSelected && i < m_leftCells.size(); i++)
    {
        if (isClicked(m_leftCells[i], pTouch))
        {
            m_pLeftSelected = m_leftCells[i];
        }
    }
    
    m_pRightSelected = NULL;
    for (int i = 0; NULL == m_pRightSelected && i < m_rightCells.size(); i++)
    {
        if (isClicked(m_rightCells[i], pTouch))
        {
            m_pRightSelected = m_rightCells[i];
        }
    }
    
    return true;
}

void GameScene_ConnectTheDots::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pLeftSelected && !isClicked(m_pLeftSelected, pTouch))
    {
        m_pLeftSelected = NULL;
        for (int i = 0; NULL == m_pLeftSelected && i < m_leftCells.size(); i++)
        {
            if (isClicked(m_leftCells[i], pTouch))
            {
                m_pLeftSelected = m_leftCells[i];
            }
        }
    }
}

void GameScene_ConnectTheDots::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
}

void GameScene_ConnectTheDots::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_ConnectTheDots::buildWorld()
{
    CCLayerGradient * pGradient = CCLayerGradient::create(ccc4(66, 207, 255, 255), ccc4(35, 178, 255, 255), Vec2(0, 1));
    pGradient->setContentSize(Size(getScreenWidth(), getScreenHeight()));
    addChild(pGradient);
    
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    m_pWorld = new b2World(gravity);
    m_pWorld->SetContactListener(this);
    
    // Define the ground body
    float size = BORDER_SIZE;
    Color3B border = Pref::getInstance()->getColor(COLOR_BORDER);
    float footerHeight = getScreenWidth() * 0.1f;
    float headerHeight = footerHeight;
    
    // left wall
    float x = 0;
    float y = getScreenHeight() * 0.075f;
    for (int i = 0; i < 5; i++)
    {
        Sprite * pCell = Sprite::createWithSpriteFrameName("cell.png");
        pCell->setPosition(x + pCell->boundingBox().size.width / 2, y + pCell->boundingBox().size.height / 2);
        addChild(pCell);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pCell;
        groundBodyDef.position.Set(pCell->getPositionX()/PTM_RATIO, pCell->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox((pCell->boundingBox().size.width/2)/PTM_RATIO, (pCell->boundingBox().size.height/2)/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
        
        y += pCell->boundingBox().size.height * 1.05f;
        
        m_leftCells.push_back(pCell);
    }
    
    // right wall
    x = getScreenWidth();
    y = getScreenHeight() * 0.075f;
    for (int i = 0; i < 5; i++)
    {
        Sprite * pCell = Sprite::createWithSpriteFrameName("cell.png");
        pCell->setPosition(x - pCell->boundingBox().size.width / 2, y + pCell->boundingBox().size.height / 2);
        addChild(pCell);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pCell;
        groundBodyDef.position.Set(pCell->getPositionX()/PTM_RATIO, pCell->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox((pCell->boundingBox().size.width/2)/PTM_RATIO, (pCell->boundingBox().size.height/2)/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
        
        y += pCell->boundingBox().size.height * 1.05f;
        
        m_rightCells.push_back(pCell);
    }
    { // bottom wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(255, border.g, border.b, 255), getScreenWidth(), size);
        pGroundTile->setPosition(ccp(0, footerHeight));
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);        
        pGround->CreateFixture(&groundBox, 0);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
    }
    { // bottom wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(255, border.g, border.b, 255), getScreenWidth(), size);
        pGroundTile->setPosition(ccp(0, getScreenHeight() - footerHeight));
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        pGround->CreateFixture(&groundBox, 0);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
    }
    
    addEnemy();
    addEnemy();
    addEnemy();
    addEnemy();
}

void GameScene_ConnectTheDots::addEnemy()
{
    float px = getScreenWidth() / 3 + rand() % (int)(getScreenWidth() * 0.5f);
    float py = getScreenHeight() / 3 + rand() % (int)(getScreenHeight() * 0.5f);
    float a = CC_DEGREES_TO_RADIANS(rand()%360);
    CCLOG("%.2f, %.2f, %.2f, %.2f", px, py, a);
    
    Sprite * pEnemy = Sprite::createWithSpriteFrameName("mine.png");
    pEnemy->setScale(0.5f);
    pEnemy->setTag(TAG_ENEMEY);
    pEnemy->setPosition(px, py);
    addChild(pEnemy);
    
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
    pBody->ApplyAngularImpulse(1, true);
    pBody->SetLinearVelocity(b2Vec2(vx, vy));
    
    pEnemy->setOpacity(0);
    pEnemy->runAction(FadeTo::create(0.5f, 255));
}

void GameScene_ConnectTheDots::updateWorld(float dt)
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
    
    checkCollisions();
}

void GameScene_ConnectTheDots::checkCollisions()
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
            
        }
    }
}

void GameScene_ConnectTheDots::removeSprite(cocos2d::Node * pNode)
{
    removeChild(pNode);
}