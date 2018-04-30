#include "GameScene_DontShootYourself.h"
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

GameScene_DontShootYourself::GameScene_DontShootYourself()
{
    srand(time(NULL));
    m_fLastAddedEnemy = 0;
    m_fForceTimer = 0;
    m_bDrawGuide = false;
    m_fDestAngle = 0;
    m_bGotHit = false;
    m_fLastAddedStar = 0;
}

GameScene_DontShootYourself::~GameScene_DontShootYourself()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_DontShootYourself::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_DontShootYourself *layer = GameScene_DontShootYourself::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Box2d collision methods
// ===========================================================

void GameScene_DontShootYourself::BeginContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    m_contacts.push_back(myContact);
}

void GameScene_DontShootYourself::EndContact(b2Contact* contact)
{
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(m_contacts.begin(), m_contacts.end(), myContact);
    if (pos != m_contacts.end())
    {
        m_contacts.erase(pos);
    }
}

void GameScene_DontShootYourself::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void GameScene_DontShootYourself::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_DontShootYourself::handleInit(float screenWidth, float screenHeight)
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
    buildPlayer();
    
    m_forceTo = m_pPlayer->getPosition();
    m_pPlayerBody->SetLinearVelocity(b2Vec2(0,4));
}

void GameScene_DontShootYourself::handleUpdate(float dt)
{
    //m_pPlayerBody->SetTransform(b2Vec2(m_pPlayer->getPositionX()/PTM_RATIO, m_pPlayer->getPositionY()/PTM_RATIO), 0);
    
    updateWorld(dt);
    
    if (m_bGotHit)
    {
        return;
    }
    
    if (getTime() - m_fLastAddedEnemy > 2)
    {
        m_fLastAddedEnemy = getTime();
        addEnemy();
    }
    
    if (getTime() - m_fLastAddedStar > 2)
    {
        m_fLastAddedStar = getTime();
        addStar();
    }
    
    if (!m_bDrawGuide && m_guidePointIndex < m_guidePoints.size())
    {
//        Vec2 nextPos = m_guidePoints[m_guidePointIndex];
//        
//        float dx = nextPos.x - m_pPlayer->getPositionX();
//        float dy = nextPos.y - m_pPlayer->getPositionY();
//        float d = sqrtf(dx*dx + dy*dy);
//        if (d < m_pPlayer->boundingBox().size.width / 4) // reach new point
//        {
//            m_guidePointIndex++;
//        }
//        else
//        {
//            float angle = atan2f(dy, dx);
//            float newX = m_pPlayer->getPositionX() + 10 * angle;
//            float newY = m_pPlayer->getPositionY() + 10 * angle;
//            //m_pPlayerBody->SetTransform(b2Vec2(newX/PTM_RATIO, newY/PTM_RATIO), angle);
//            //m_pPlayer->setPosition(newX, newY);
//        }
    }
    
//    if (m_fForceTimer > 0)
//    {
//        m_fForceTimer = 0;//dt;
//        
//        float dx = m_forceTo.x - m_pPlayer->getPositionX();
//        float dy = m_forceTo.y - m_pPlayer->getPositionY();
//        float d = sqrt(dx*dx + dy*dy);
//        float a = atan2f(dy, dx);
//        float f = 65 * (1.0f - (d / getScreenHeight()));
//        float fx = cos(a) * f;
//        float fy = sin(a) * f;
//        m_pPlayerBody->ApplyForceToCenter(b2Vec2(fx, fy), true);
//    }
    
    if (m_fForceTimer > 0)
    {
        float dx = m_forceTo.x - m_pPlayer->getPositionX();
        float dy = m_forceTo.y - m_pPlayer->getPositionY();
        float d = sqrt(dx*dx + dy*dy);
        if (d < m_pPlayer->boundingBox().size.width * 0.6f)
        {
            m_fForceTimer = 0;
        }
        else
        {
            float a = atan2f(dy, dx);
            float f = 5;
            float fx = cos(a) * f;
            float fy = sin(a) * f;
            m_pPlayerBody->SetLinearVelocity(b2Vec2(fx, fy));
            
            m_fDestAngle = a;
        }
        
        m_fForceTimer = 0;
    }
    
//    float currAngle = m_pPlayerBody->GetAngle();
//    
//    if (currAngle > m_fDestAngle)
//    {
//        currAngle -= dt * 5;
//        if (currAngle < m_fDestAngle)
//        {
//            currAngle = m_fDestAngle;
//        }
//    }
//    else if (currAngle < m_fDestAngle)
//    {
//        currAngle += dt * 5;
//        if (currAngle > m_fDestAngle)
//        {
//            currAngle = m_fDestAngle;
//        }
//    }
    
    if (NULL != m_pPlayerBody)
    {
        float a = atan2f(m_pPlayerBody->GetLinearVelocity().y, m_pPlayerBody->GetLinearVelocity().x);
        m_pPlayerBody->SetTransform(m_pPlayerBody->GetPosition(), a);
    }
    
    
    
    m_oldPlayerPos = m_pPlayer->getPosition();
}

bool GameScene_DontShootYourself::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bGotHit)
    {
        return true;
    }
    m_forceTo = pTouch->getLocation();
    m_fForceTimer = 1;
    
//    m_bDrawGuide = false;
//    
//    float dx = m_pPlayer->getPositionX() - pTouch->getLocation().x;
//    float dy = m_pPlayer->getPositionY() - pTouch->getLocation().y;
//    float d = sqrtf(dx*dx + dy*dy);
//    if (d < m_pPlayer->boundingBox().size.width)
//    {
//        m_bDrawGuide = true;
//        clearGuideDots();
//        m_guidePointIndex = 0;
//        m_guidePoints.clear();
//        m_guidePoints.push_back(pTouch->getLocation());
//        addGuideDot(pTouch->getLocation());
//    }
    return true;
}

void GameScene_DontShootYourself::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bGotHit)
    {
        return;
    }
    m_forceTo = pTouch->getLocation();
    m_fForceTimer = 1;
    
//    if (m_bDrawGuide)
//    {
//        m_guidePoints.push_back(pTouch->getLocation());
//        
//        Vec2 lastDotPos = m_guide[m_guide.size() - 1]->getPosition();
//        float dx = lastDotPos.x - pTouch->getLocation().x;
//        float dy = lastDotPos.y - pTouch->getLocation().y;
//        float d = sqrtf(dx*dx + dy*dy);
//        if (d > getScreenWidth() * 0.05f)
//        {
//            addGuideDot(pTouch->getLocation());
//        }
//    }
}

void GameScene_DontShootYourself::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bGotHit)
    {
        return;
    }
    
    CCLOG("touch end");
    
    m_fForceTimer = 0;
//    if (m_bDrawGuide)
//    {
//        m_bDrawGuide = false;
//        
//        m_guidePoints.push_back(pTouch->getLocation());
//        
//        Vector<FiniteTimeAction*> moves;
//        Vec2 lastPos = m_pPlayer->getPosition();
//        for (int i = 1; i < m_guidePoints.size(); i++)
//        {
//            //if (i % 3 == 0)
//            {
//                float dx = m_guidePoints[i].x - lastPos.x;
//                float dy = m_guidePoints[i].y - lastPos.y;
//                float p = (float)i / m_guidePoints.size();
//                float d = sqrt(dx*dx + dy*dy) / (220 + p * 220);
//                
//                MoveTo * pMove = MoveTo::create(d, m_guidePoints[i]);
//                moves.pushBack(pMove);
//                
//                lastPos = m_guidePoints[i];
//            }
//        }
//        m_pPlayer->runAction(Sequence::create(moves));
//        
//        clearGuideDots();
//    }
}

void GameScene_DontShootYourself::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_DontShootYourself::buildWorld()
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
    float headerHeight = 0;
    
    { // bottom wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), getScreenWidth(), size);
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
    { // top wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 0), getScreenWidth(), size);
        pGroundTile->setPosition(ccp(0, getScreenHeight() - headerHeight));
        pGroundTile->setTag(TAG_TOP_WALL);
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
    }
    { // left wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 0), size, getScreenHeight());
        pGroundTile->setPosition(ccp(0, 0));
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
    }
    { // right wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 0 ), size, getScreenHeight());
        pGroundTile->setPosition(ccp(getScreenWidth(), 0));
        pGroundTile->setTag(TAG_RIGHT_WALL);
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundShape;
        groundShape.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        
        b2FixtureDef groundFixDef;
        groundFixDef.shape = &groundShape;
        groundFixDef.filter.categoryBits = CAT_GROUND;
        groundFixDef.filter.maskBits = CAT_PLAYER | CAT_ENEMY;
        pGround->CreateFixture(&groundFixDef);
    }
    
    LayerColor * pUIPanel = LayerColor::create(ccc4(255, 255, 255, 255), getScreenWidth(), footerHeight);
    addChild(pUIPanel, 10);
    
    Sprite * pBomb = Sprite::createWithSpriteFrameName("mine.png");
    pBomb->setScale(0.4f);
    pBomb->setPosition(getScreenWidth() * 0.1f + pBomb->boundingBox().size.width / 2, pUIPanel->boundingBox().size.height / 2);
    pUIPanel->addChild(pBomb);
    
    m_pCounter = CCLabelTTF::create("0", fontList[0], footerHeight * 0.6f);
    m_pCounter->setTag(0);
    m_pCounter->setPosition(pBomb->getPositionX() + pBomb->boundingBox().size.width + m_pCounter->boundingBox().size.width / 2, pBomb->getPositionY());
    m_pCounter->setColor(ccc3(0, 0, 0));
    pUIPanel->addChild(m_pCounter);
    
    Sprite * pCoin = Sprite::createWithSpriteFrameName("coin.png");
    pCoin->setScale(0.4f);
    pCoin->setPosition(getScreenWidth() * 0.7f + pCoin->boundingBox().size.width / 2, pUIPanel->boundingBox().size.height / 2);
    pUIPanel->addChild(pCoin);
    
    m_pStars = CCLabelTTF::create("0", fontList[0], footerHeight * 0.6f);
    m_pStars->setTag(0);
    m_pStars->setPosition(pCoin->getPositionX() + pCoin->boundingBox().size.width + m_pStars->boundingBox().size.width / 2, pCoin->getPositionY());
    m_pStars->setColor(ccc3(0, 0, 0));
    pUIPanel->addChild(m_pStars);
}

void GameScene_DontShootYourself::buildPlayer()
{
    m_pPlayer = Sprite::createWithSpriteFrameName("normal-01.png");
    m_pPlayer->setTag(TAG_PLAYER);
    m_pPlayer->setPosition(getScreenWidth() / 2, getScreenHeight() / 2);
    addChild(m_pPlayer);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = m_pPlayer;
    bodyDef.position.Set(m_pPlayer->getPositionX()/PTM_RATIO, m_pPlayer->getPositionY()/PTM_RATIO);
    bodyDef.angle = M_PI_2;
    b2Body * pBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (m_pPlayer->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    shapeDef.filter.categoryBits = CAT_PLAYER;
    shapeDef.filter.maskBits = CAT_GROUND | CAT_ENEMY;
    pBody->CreateFixture(&shapeDef);
    
    m_fDestAngle = bodyDef.angle;
    
    m_pPlayerBody = pBody;
    
    char name[50];
    Vector<SpriteFrame*> array(6);
    for (int i = 1; i <= 6; i++)
    {
        sprintf(name, "normal-%02d.png", i);
        SpriteFrame * pFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        array.pushBack(pFrame);
    }
    CCAnimation * pAnimFrames = CCAnimation::createWithSpriteFrames(array, 0.06f);
    pAnimFrames->setLoops(-1);
    Animate * pAnim = Animate::create(pAnimFrames);
    m_pPlayer->runAction(pAnim);
}

void GameScene_DontShootYourself::addEnemy()
{
    float px = m_pPlayer->getPositionX();
    float py = m_pPlayer->getPositionY();
    float a = M_PI + m_pPlayerBody->GetAngle();
    CCLOG("%.2f, %.2f, %.2f, %.2f", px, py, a);
    
    Sprite * pEnemy = Sprite::createWithSpriteFrameName("mine.png");
    pEnemy->setScale(0.5f);
    pEnemy->setTag(TAG_ENEMEY);
    pEnemy->setPosition(px + cos(a) * m_pPlayer->boundingBox().size.width * 0.5f, py + sin(a) * m_pPlayer->boundingBox().size.height * 0.5f);
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
    
    int count = m_pCounter->getTag() + 1;
    m_pCounter->setTag(count);
    char countStr[10];
    sprintf(countStr, "%d", count);
    float x = m_pCounter->getPositionX() - m_pCounter->boundingBox().size.width / 2;
    m_pCounter->setString(countStr);
    m_pCounter->setPositionX(x + m_pCounter->boundingBox().size.width / 2);
}

void GameScene_DontShootYourself::addStar()
{
    Sprite * pStar = Sprite::createWithSpriteFrameName("coin.png");
    pStar->setScale(0.5f);
    pStar->setTag(TAG_STAR);
    pStar->setPosition(rand() % getScreenWidth(), rand() % getScreenHeight());
    addChild(pStar);
    
    CCFiniteTimeAction* pAnimDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_DontShootYourself::removeSprite));
    pStar->runAction(Sequence::create(DelayTime::create(2), FadeTo::create(1, 0), pAnimDone, NULL));
}

void GameScene_DontShootYourself::updateWorld(float dt)
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
            
            if (sprite->getTag() == TAG_RIGHT_WALL)
            {
                sprite->setPositionX(sprite->getPositionX() - sprite->boundingBox().size.width);
            }
            else if (sprite->getTag() == TAG_TOP_WALL)
            {
                sprite->setPositionY(sprite->getPositionY() - sprite->boundingBox().size.height);
            }            
        }
    }
    
    checkCollisions();
}

void GameScene_DontShootYourself::checkCollisions()
{
    if (m_bGotHit)
    {
        return;
    }
    
    std::vector<b2Body *> toRemove;
    for (std::vector<MyContact>::iterator pos = m_contacts.begin(); pos != m_contacts.end(); ++pos)
    {
        b2Body *bodyA = (*pos).fixtureA->GetBody();
        b2Body *bodyB = (*pos).fixtureB->GetBody();
        if (NULL != bodyA->GetUserData() && NULL != bodyB->GetUserData())
        {
            Sprite * pSpriteA = (Sprite *)bodyA->GetUserData();
            Sprite * pSpriteB = (Sprite *)bodyB->GetUserData();
            if (TAG_ENEMEY == pSpriteB->getTag() && TAG_PLAYER == pSpriteA->getTag())
            {
                if (pSpriteB->getOpacity() == 255)
                {
                    m_bGotHit = true;
                }
            }
            else if (TAG_ENEMEY == pSpriteA->getTag() && TAG_PLAYER == pSpriteB->getTag())
            {
                if (pSpriteA->getOpacity() == 255)
                {
                    m_bGotHit = true;
                }
            }
        }
    }
    
    if (m_bGotHit)
    {
        char name[50];
        Vector<SpriteFrame*> array(10);
        for (int i = 1; i <= 10; i++)
        {
            sprintf(name, "explosion-%02d.png", i);
            SpriteFrame * pFrame = SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
            array.pushBack(pFrame);
        }
        CCAnimation * pAnimFrames = CCAnimation::createWithSpriteFrames(array, 0.06f);
        Animate * pAnim = Animate::create(pAnimFrames);
        m_pPlayer->stopAllActions();
        m_pPlayer->runAction(pAnim);
        
        m_pWorld->DestroyBody(m_pPlayerBody);
        m_pPlayerBody = NULL;
    }
    else
    {
        for (int i = 0; i < getChildrenCount(); i++)
        {
            Node * pNode = getChildren().at(i);
            if (TAG_STAR == pNode->getTag())
            {
                float dx = m_pPlayer->getPositionX() - pNode->getPositionX();
                float dy = m_pPlayer->getPositionY() - pNode->getPositionY();
                float d = sqrtf(dx*dx + dy*dy);
                if (d < m_pPlayer->boundingBox().size.width / 2)
                {
                    collectStar(pNode);
                    break;
                }
            }
        }
    }
}

void GameScene_DontShootYourself::addGuideDot(cocos2d::Vec2 pos)
{
    Sprite * pDot = Sprite::create("guide.png");
    pDot->setPosition(pos);
    addChild(pDot);
    
    pDot->setOpacity(0);
    pDot->runAction(FadeTo::create(0.25f, 255));
    
    m_guide.push_back(pDot);
}

void GameScene_DontShootYourself::clearGuideDots()
{
    for (int i = 0; i < m_guide.size(); i++)
    {
        CCFiniteTimeAction* pAnimDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_DontShootYourself::removeSprite));
        
        m_guide[i]->stopAllActions();
        m_guide[i]->runAction(Sequence::createWithTwoActions(FadeTo::create(0.25f, 0), pAnimDone));
    }
    
    m_guide.clear();
}

void GameScene_DontShootYourself::removeSprite(cocos2d::Node * pNode)
{
    removeChild(pNode);
}

void GameScene_DontShootYourself::collectStar(cocos2d::Node * pStar)
{
    removeChild(pStar);
    
    int count = m_pStars->getTag() + 1;
    m_pStars->setTag(count);
    char countStr[10];
    sprintf(countStr, "%d", count);
    float x = m_pStars->getPositionX() - m_pStars->boundingBox().size.width / 2;
    m_pStars->setString(countStr);
    m_pStars->setPositionX(x + m_pStars->boundingBox().size.width / 2);
}