#include "GameScene.h"
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

// ===========================================================
// TestThread
// ===========================================================

void *TestThreadProc(void * pData)
{
    GameScene * pGameScene = (GameScene *)pData;
    while (!pGameScene->isDone())
    {
        std::vector<GameScene::WorkCircle> filled = pGameScene->getFilledCirles();
        if (!filled.empty())
        {
            pGameScene->test(filled);
        }
    }
}

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene::GameScene()
{
    srand(time(NULL));
            
    m_bDone = false;
    m_bShowGameOver = 0;
    m_iFillCount = 0;
    
    pthread_mutex_init(&m_checkMutex, NULL);
}

GameScene::~GameScene()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    pthread_mutex_destroy(&m_checkMutex);
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// Public methods
// ===========================================================

void GameScene::test(std::vector<WorkCircle> circles)
{
    CCLOG("test start");
    
    float w = getScreenWidth();
    float h = getScreenHeight();
    
    for (int i = 0; i < circles.size(); i++)
    {
        int r = circles[i].m_r;
        int cx = circles[i].m_x;
        int cy = circles[i].m_y;
        CCLOG("x: %d, y:%d, r:%d", cx, cy, r);
        int c = 0;
        for (int y = 0; y < r * 2; y++)
        {
            for (int x = 0; x < r * 2; x++)
            {
                float px = cx - r + x;
                float py = cy - r + y;
                if (px >= 0 && px < w && py >= 0 && py < h)
                {
                    float dx = px - cx;
                    float dy = py - cy;
                    if (sqrt(dx*dx + dy*dy) < r)
                    {
                        int index = (int)(py * w + px);
                        m_pFillBuffer[index] = true;
                        c++;
                    }
                }
            }
        }
    }
    
    bool done = true;
    int count = (int)(w * h);
    for (int i = 0; done && i < count; i++)
    {
        if (!m_pFillBuffer[i])
        {
            done = false;
        }
    }
    
    if (done)
    {
        onGameOver();
    }
    
    CCLOG("test end %d", done);
}

std::vector<GameScene::WorkCircle> GameScene::getFilledCirles()
{
    std::vector<GameScene::WorkCircle> retVal;
    
    pthread_mutex_lock(&m_checkMutex);
    
    for (int i = 0; i < m_filledCircles.size(); i++)
    {
        retVal.push_back(m_filledCircles[i]);
    }
    m_filledCircles.clear();
    
    pthread_mutex_unlock(&m_checkMutex);
    
    return retVal;
}

void GameScene::addFilledCircle(GameScene::WorkCircle wc)
{
    pthread_mutex_lock(&m_checkMutex);
    
    m_filledCircles.push_back(wc);
    
    pthread_mutex_unlock(&m_checkMutex);
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene::handleInit(float screenWidth, float screenHeight)
{
    buildWorld(screenWidth, screenHeight);
    
    int fillBufferSize = (int)(screenWidth * screenHeight);
    m_pFillBuffer = new bool[fillBufferSize];
    memset(m_pFillBuffer, 1, sizeof(bool) * fillBufferSize);
    
    // add borders to fill
    int size = (int)BORDER_SIZE;
    for (int y = size; y < screenHeight - size; y++)
    {
        for (int x = size; x < screenWidth - size; x++)
        {
            m_pFillBuffer[(int)(y * screenWidth + x)] = 0;
        }
    }
    
    m_pCanvas = CCRenderTexture::create(screenWidth, screenHeight, kCCTexture2DPixelFormat_RGBA8888);
    m_pCanvas->setPosition(ccp(screenWidth / 2, screenHeight / 2));
    addChild(m_pCanvas);
    
    m_pEnemies = CCLayer::create();
    m_pEnemies->setZOrder(1);
    addChild(m_pEnemies);

    addCenter();
    
    prepareEnemies(m_pEnemies);
    
    pthread_create(&m_checkThread, NULL, TestThreadProc, (void *)this);
}

void GameScene::handleUpdate(float dt)
{
    if (m_bDone)
    {
        if (m_bShowGameOver > 0)
        {
            m_bShowGameOver += dt;
            if (m_bShowGameOver > 1)
            {
                m_bShowGameOver = 0;
                switchScene(GameScene::scene());
            }
        }
        return;
    }
    else
    {
        float progress = (getTime() / GAME_MAX_TIME);
        if (progress > 1)
        {
            progress = 1;
        }
        m_pTimer->setPercentage(100 * progress);
        
        if (1 == progress)
        {
            onGameOver();
            return;
        }
        
        std::vector<WorkCircle>::iterator iter = m_workCircles.begin();
        while (iter != m_workCircles.end())
        {
            // update progress scale to fit circle scale
            float scale = (*iter).m_pCircle->getScale();
            (*iter).m_pCircleProgress->setScale(scale * 1.0f);
            
            // animate progress while filling circle
            bool removed = false;
            if (0 != (*iter).m_fReleaseTime)
            {
                float progress = (getTime() - (*iter).m_fReleaseTime) * FILL_SPEED;
                if (progress > 1)
                {
                    progress = 1;
                }
                                
                if (1 == progress)
                {
                    finishCircleFill((*iter));
                    removed = true;
                }
            }
            
            if (removed)
            {
                iter = m_workCircles.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        
        updateEnemies(m_pEnemies, dt);
        
        runWorld(dt);
    }
    
    checkCollision();
}

bool GameScene::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    CCLOG("touch start");
    if (m_bDone)
    {
        return true;
    }
    startCircle(pTouch->getLocation());
    return true;
}

void GameScene::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    CCLOG("touch move");
    if (m_bDone)
    {
        return;
    }
}

void GameScene::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    CCLOG("touch end");
    if (m_bDone)
    {
        return;
    }
    if (!m_workCircles.empty())
    {
        WorkCircle & lastCircle = m_workCircles[m_workCircles.size() - 1];
        if (0 == lastCircle.m_fReleaseTime)
        {
            lastCircle.m_fReleaseTime = getTime();
            lastCircle.m_pCircle->stopAllActions();
            if (lastCircle.m_pCircle->getScale() < 0.09f)
            {
                lastCircle.m_pCircle->setScale(0.09f);
            }
            
            float d = 1.0f / FILL_SPEED;
            CCNode * pFiller = (CCNode *)(lastCircle.m_pCircleProgress->getChildren().at(0));
            CCScaleTo * pScaleIn = CCScaleTo::create(d / 2, 0.85f);
            CCScaleTo * pScaleOut = CCScaleTo::create(d / 2, 1.0f);
            CCSequence * pSeq = CCSequence::createWithTwoActions(pScaleIn, pScaleOut);
            pFiller->runAction(CCEaseSineInOut::create(pSeq));
        }
    }
}

void GameScene::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene::buildWorld(float screenWidth, float screenHeight)
{
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    m_pWorld = new b2World(gravity);
    
    // Define the ground body
    float size = BORDER_SIZE;
    Color3B border = Pref::getInstance()->getColor(COLOR_FILL);
    
    { // bottom wall
        CCLayerColor * pGroundTile = CCLayerColor::create(ccc4(border.r, border.g, border.b, 255), getScreenWidth(), size);
        pGroundTile->setPosition(ccp(0, 0));
        pGroundTile->setZOrder(1);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        m_pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        m_pGroundFixture = m_pGround->CreateFixture(&groundBox, 0);
    }
    { // top wall
        CCLayerColor * pGroundTile = CCLayerColor::create(ccc4(border.r, border.g, border.b, 255), getScreenWidth(), size);
        pGroundTile->setZOrder(1);
        pGroundTile->setPosition(ccp(0, screenHeight));
        pGroundTile->setTag(TAG_TOP_WALL);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        m_pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        m_pGroundFixture = m_pGround->CreateFixture(&groundBox, 0);
    }
    { // left wall
        CCLayerColor * pGroundTile = CCLayerColor::create(ccc4(border.r, border.g, border.b, 255), size, getScreenHeight());
        pGroundTile->setZOrder(1);
        pGroundTile->setPosition(ccp(0, 0));
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        m_pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        m_pGroundFixture = m_pGround->CreateFixture(&groundBox, 0);
    }
    { // right wall
        CCLayerColor * pGroundTile = CCLayerColor::create(ccc4(border.r, border.g, border.b, 255), size, getScreenHeight());
        pGroundTile->setZOrder(1);
        pGroundTile->setPosition(ccp(screenWidth, 0));
        pGroundTile->setTag(TAG_RIGHT_WALL);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        m_pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        m_pGroundFixture = m_pGround->CreateFixture(&groundBox, 0);
    }
}

void GameScene::startCircle(cocos2d::Vec2 pos)
{
    WorkCircle wc;
    memset(&wc, 0, sizeof(wc));
    
    wc.m_pCircle = Sprite::create("circle.png");
    wc.m_pCircle->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    wc.m_pCircle->setPosition(pos);
    addChild(wc.m_pCircle);
    
    wc.m_pCircleProgress = ProgressTimer::create(CCSprite::create("circle.png"));
    wc.m_pCircleProgress->setType(kCCProgressTimerTypeRadial);
    wc.m_pCircleProgress->setPercentage(100);
    wc.m_pCircleProgress->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    wc.m_pCircleProgress->setPosition(pos);
    addChild(wc.m_pCircleProgress);
    
    Sprite * pProgressOverlay = Sprite::create("circle.png");
    pProgressOverlay->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pProgressOverlay->setScale(0.9f);
    pProgressOverlay->setPosition(ccp(wc.m_pCircle->boundingBox().size.width / 2, wc.m_pCircle->boundingBox().size.height / 2));
    wc.m_pCircleProgress->addChild(pProgressOverlay);
    
    wc.m_pCircle->setScale(0);
    wc.m_pCircleProgress->setScale(0);
    
    wc.m_pCircle->runAction(ScaleTo::create(4.0f, 5.0f));
    m_workCircles.push_back(wc);
}

void GameScene::addCenter()
{
    float width = getScreenWidth();
    float height = getScreenHeight();
    
    CCSprite * pCenterOuter = CCSprite::create("circle.png");
    pCenterOuter->setScale(0.51f);
    pCenterOuter->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pCenterOuter->setPosition(ccp(width / 2, height / 2));
    addChild(pCenterOuter, 1);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.userData = pCenterOuter;
    bodyDef.position.Set(pCenterOuter->getPositionX()/PTM_RATIO, pCenterOuter->getPositionY()/PTM_RATIO);
    m_pCenterBody = m_pWorld->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (pCenterOuter->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    m_pCenterBody->CreateFixture(&shapeDef);
    
    // add center fill
    CCSprite * pCenter = CCSprite::create("circle.png");
    pCenter->setScale(pCenterOuter->getScale() * 0.9f);
    pCenter->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pCenter->setPosition(pCenterOuter->getPosition());
    addChild(pCenter, 1);
    
    // add timer
    CCSprite * pTimerSprite = CCSprite::create("circle.png");
    pTimerSprite->setColor(Pref::getInstance()->getColor(COLOR_BG));
    m_pTimer = CCProgressTimer::create(pTimerSprite);
    m_pTimer->setScale(pCenterOuter->getScale() * 0.86f);
    m_pTimer->setType(kCCProgressTimerTypeRadial);
    m_pTimer->setPosition(pCenterOuter->getPosition());
    addChild(m_pTimer, 1);
    
    // add timer cover
    CCSprite * pTimerCover = CCSprite::create("circle.png");
    pTimerCover->setScale(pCenterOuter->getScale() * 0.82f);
    pTimerCover->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pTimerCover->setPosition(pCenterOuter->getPosition());
    addChild(pTimerCover, 1);
    
    // add fill counter text
    m_fCounterSize = pCenterOuter->boundingBox().size.width * 0.5f;
    m_pCounter = CCLabelTTF::create("0", fontList[0], m_fCounterSize);
    m_pCounter->setPosition(ccp(pCenterOuter->getPositionX() + pCenterOuter->boundingBox().size.width * 0.02f, pCenterOuter->getPositionY() - pCenterOuter->boundingBox().size.width * 0.02f));
    m_pCounter->setColor(Pref::getInstance()->getColor(COLOR_BG));
    addChild(m_pCounter, 1);
}

void GameScene::onGameOver()
{
    m_bShowGameOver = 0.01f;
    m_bDone = true;
    for (int i = 0; i < m_workCircles.size(); i++)
    {
        m_workCircles[i].m_pCircle->stopAllActions();
        CCNode * pFiller = (CCNode *)(m_workCircles[i].m_pCircleProgress->getChildren().at(0));
        pFiller->stopAllActions();
    }
}

void GameScene::finishCircleFill(WorkCircle & wc)
{
    // draw circle on canvas
//    m_pCanvas->begin();
//    wc.m_pCircle->visit();
//    m_pCanvas->end();
//    Director::getInstance()->getRenderer()->render();
//    
//    wc.m_x = wc.m_pCircle->getPositionX();
//    wc.m_y = wc.m_pCircle->getPositionY();
//    wc.m_r = wc.m_pCircle->boundingBox().size.width / 2;
//    
//    removeChild(wc.m_pCircle);
//    wc.m_pCircle = NULL;
//    removeChild(wc.m_pCircleProgress);
//    wc.m_pCircleProgress = NULL;
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.userData = wc.m_pCircle;
    bodyDef.position.Set(wc.m_pCircle->getPositionX()/PTM_RATIO, wc.m_pCircle->getPositionY()/PTM_RATIO);
    b2Body * pBody = getWorld()->CreateBody(&bodyDef);
    
    b2CircleShape shape;
    shape.m_radius = (wc.m_pCircle->boundingBox().size.width/2)/PTM_RATIO;
    
    b2FixtureDef shapeDef;
    shapeDef.shape = &shape;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    pBody->CreateFixture(&shapeDef);
    
    removeChild(wc.m_pCircleProgress);
    wc.m_pCircleProgress = NULL;
    
    
    // inceremnt counter
    m_iFillCount++;
    
    CCFiniteTimeAction* pCounterFadeDone = CCCallFuncN::create(this, callfuncN_selector(GameScene::onCounterFadeDone));
    m_pCounter->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.3f), pCounterFadeDone));
    Vec2 pos = m_pCounter->getPosition();
    Color3B color = m_pCounter->getColor();
    
    char newValue[10];
    sprintf(newValue, "%d", m_iFillCount);
    m_pCounter = CCLabelTTF::create(newValue, fontList[0], m_fCounterSize);
    m_pCounter->setPosition(pos);
    m_pCounter->setColor(color);
    addChild(m_pCounter, 1);
    
    m_pCounter->setOpacity(0.5);
    m_pCounter->setScale(0.5f);
    m_pCounter->runAction(CCSpawn::createWithTwoActions(CCFadeTo::create(0.3f, 255), CCScaleTo::create(0.3f, 1.0f)));
    
    
    
    
    //addFilledCircle(wc);
}

void GameScene::runWorld(float dt)
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
}

void GameScene::checkCollision()
{
    for (int i = 0; i < m_workCircles.size(); i++)
    {
        Node * pCircle = m_workCircles[i].m_pCircle;
        float r = pCircle->boundingBox().size.width / 2;
        for (int i = 0; !m_bDone && i < m_pEnemies->getChildrenCount(); i++)
        {
            Node * pEnemy = (Node *)m_pEnemies->getChildren().at(i);
            float dx = pEnemy->getPositionX() - pCircle->getPositionX();
            float dy = pEnemy->getPositionY() - pCircle->getPositionY();
            float d = sqrt(dx*dx+dy*dy);
            float er = pEnemy->boundingBox().size.width / 2;
            if (d < r + er)
            {
                onGameOver();
            }
        }
    }
}

void GameScene::onCounterFadeDone(cocos2d::CCNode * pSender)
{
    removeChild(pSender);
}