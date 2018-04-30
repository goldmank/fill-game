#include "GameScene_FruitNinja.h"
#include "SimpleAudioEngine.h"
#include "Pref.h"
#include "SystemUtils.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace DollarRecognizer;

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

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_FruitNinja::GameScene_FruitNinja()
{
    srand(time(NULL));
    m_fLastEnemyAddTime = 0;
}

GameScene_FruitNinja::~GameScene_FruitNinja()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_FruitNinja::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_FruitNinja *layer = GameScene_FruitNinja::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_FruitNinja::handleInit(float screenWidth, float screenHeight)
{
    m_gestureDetector.loadTemplates();
    
    const char * batches[] = { "fightline", NULL };
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
 
    b2Vec2 gravity;
    gravity.Set(0.0f, -15.0f);
    m_pWorld = new b2World(gravity);
}

void GameScene_FruitNinja::handleUpdate(float dt)
{
    updateWorld(dt);
    
    m_fLastEnemyAddTime += dt;
    if (m_fLastEnemyAddTime > 2)
    {
        m_fLastEnemyAddTime = 0;
        
        switch (rand()%3)
        {
            case 0 :
            {
                addObject(getScreenWidth() * 0.2f, -getScreenHeight() / 2, 36, M_PI_2 * 0.97f);
            } break;
            case 1 :
            {
                addObject(getScreenWidth() * 0.5f, -getScreenHeight() / 2, 36, M_PI_2);
            } break;
            case 2 :
            {
                addObject(getScreenWidth() * 0.8f, -getScreenHeight() / 2, 36, M_PI_2 * 1.03f);
            } break;
        }
    }
}

bool GameScene_FruitNinja::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_gesture.clear();
    m_gesture.push_back(Point2D(pTouch->getLocation().x, getScreenHeight() - pTouch->getLocation().y));
    return true;
}

void GameScene_FruitNinja::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_gesture.push_back(Point2D(pTouch->getLocation().x, getScreenHeight() - pTouch->getLocation().y));
}

void GameScene_FruitNinja::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_gesture.push_back(Point2D(pTouch->getLocation().x, getScreenHeight() - pTouch->getLocation().y));
    RecognitionResult rc = m_gestureDetector.recognize(m_gesture);
    if (rc.name == "V")
    {
        std::vector<b2Body *> toDelete;
        for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
        {
            if (NULL != b->GetUserData())
            {
                Node* sprite = (Node*)b->GetUserData();
                if (sprite->getPositionY() > 0)
                {
                    ObjectData * pData = (ObjectData *)sprite->getUserData();
                    if (pData->m_iType == 1)
                    {
                        toDelete.push_back(b);
                    }
                }
            }
        }
        
        for (int i = 0; i < toDelete.size(); i++)
        {
            b2Body * b = toDelete[i];
            Node* sprite = (Node*)b->GetUserData();
            ObjectData * pData = (ObjectData *)sprite->getUserData();
            delete pData;
            sprite->setUserData(NULL);
            removeChild(sprite);
            m_pWorld->DestroyBody(b);
        }
    }
}

void GameScene_FruitNinja::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_FruitNinja::updateWorld(float dt)
{
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    m_pWorld->Step(dt, velocityIterations, positionIterations);
    
    // Iterate over the bodies in the physics world
    std::vector<b2Body *> toDelete;
    for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
    {
        if (NULL != b->GetUserData())
        {
            Node* sprite = (Node*)b->GetUserData();
            sprite->setPosition(ccp(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
            
            if (b->GetLinearVelocity().y < 0 && sprite->getPositionY() < -sprite->boundingBox().size.height)
            {
                toDelete.push_back(b);
            }
            else
            {
                ObjectData * pData = (ObjectData *)sprite->getUserData();
                float dt = (getTime() - pData->m_fStartTime) / 6;
                if (dt > 1)
                {
                    dt = 1;
                }
                sprite->setScale(pData->m_fInitialScale - dt * pData->m_fInitialScale);
            }
        }
    }
    
    for (int i = 0; i < toDelete.size(); i++)
    {
        b2Body * b = toDelete[i];
        Node* sprite = (Node*)b->GetUserData();
        ObjectData * pData = (ObjectData *)sprite->getUserData();
        delete pData;
        sprite->setUserData(NULL);
        removeChild(sprite);
        m_pWorld->DestroyBody(b);
    }
}

void GameScene_FruitNinja::addObject(float x, float y, float v, float a)
{
    int type = rand() % 2;
    
    Sprite * pEnemy = NULL;
    switch (type)
    {
        case 0 : pEnemy = Sprite::create("circle.png"); break;
        case 1 : pEnemy = Sprite::create("circle_v.png"); break;
    }
    float destSize = getScreenWidth() * 0.1f;
    float scale = destSize / pEnemy->boundingBox().size.width;
    pEnemy->setScale(scale);
    pEnemy->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pEnemy->setPosition(ccp(x, y));
    
    addChild(pEnemy, 1);
    
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
    shapeDef.isSensor = true;
    pBody->CreateFixture(&shapeDef);
    
    pBody->SetLinearVelocity(b2Vec2(v * cos(a), v * sin(a)));
    
    ObjectData * pData = new ObjectData();
    pData->m_iType = type;
    pData->m_fStartTime = getTime();
    pData->m_fInitialScale = scale;
    pEnemy->setUserData(pData);
}