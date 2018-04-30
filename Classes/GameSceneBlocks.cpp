#include "GameSceneBlocks.h"
#include "SimpleAudioEngine.h"
#include "Pref.h"
#include "SystemUtils.h"
#include "MainScene.h"
#include "GameScene_2Balls.h"

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
// Initialization & cleanup
// ===========================================================

GameSceneBlocks::GameSceneBlocks()
{
    srand(time(NULL));
    
    m_bDone = false;
    m_iScore = 0;
    m_iDestScore = 0;
    m_iTime = GAME_TIME;
    m_fScoreAnimCounter = 0;
    m_fTimer = 0;
    m_bPaused = false;
    m_fTimeBounsTimer = 0;
    m_fSpeedBounsTimer = 0;
    m_fFillBounsTimer = 0;
    m_bFilledToBlocker = true;
}

GameSceneBlocks::~GameSceneBlocks()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameSceneBlocks::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameSceneBlocks *layer = GameSceneBlocks::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameSceneBlocks::handleInit(float screenWidth, float screenHeight)
{
    float headerHeight = addHeader();
    float footerHeight = addFooter();
    float boardHeight = screenHeight - (headerHeight + footerHeight);
    
    m_fFooterHeight = footerHeight;
    
    const int colsCount = 8;
    const float size = (screenWidth-BORDER_SIZE*2) / colsCount;
    const float frame = size * 0.01f;
    const float innerSize = size - frame * 2;
    
    int rowsCount = boardHeight / size;
    
    m_pCells = CCLayer::create();
    addChild(m_pCells);
    
    float y = getScreenHeight() / 2 - (rowsCount * size) / 2;
    for (int t = 0; t < rowsCount; t++)
    {
        float x = BORDER_SIZE;
        for (int i = 0; i < colsCount; i++)
        {
            Color3B c = Pref::getInstance()->getColor(COLOR_BG);
            LayerColor * pCell = LayerColor::create(ccc4(c.r, c.g, c.b, 255), innerSize, innerSize);
            pCell->setTag(TAG_EMPTY);
            pCell->setPositionX(x + frame);
            pCell->setPositionY(y + frame);
            m_pCells->addChild(pCell);
            
        //TODO: add option for cells that are uncolorable and other that hides coins
            
            x += size;
        }
        y += size;
    }
    
    m_iRows = rowsCount;
    m_iCols = colsCount;
    
    buildWorld(screenWidth, screenHeight, headerHeight, footerHeight);
    
    m_pEnemies = CCLayer::create();
    addChild(m_pEnemies, 1);
    
    prepareEnemies(m_pEnemies);
}

void GameSceneBlocks::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }
    else
    {
        if (!m_bPaused)
        {
            updateEnemies(m_pEnemies, dt);
            runWorld(dt);
            checkCollisions();
            
            m_fScoreAnimCounter += dt;
            if (m_fScoreAnimCounter > 0.02f)
            {
                m_fScoreAnimCounter = 0;
                if (m_iScore < m_iDestScore)
                {
                    m_iScore++;
                    if (m_iScore + 3 < m_iDestScore)
                    {
                        m_iScore+=3;
                    }
                    updateScoreText();
                }
            }
            
            if (0 == m_fTimeBounsTimer)
            {
                m_fTimer += dt;
            }
            float t = GAME_TIME - m_fTimer;
            if (m_iTime != (int)t)
            {
                m_iTime = t;
                updateTimeText();
            }
            
            updateBounsTimers(dt);
            
            if (0 == m_iTime)
            {
                onGameOver();
            }
        }
    }
}

bool GameSceneBlocks::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pClickedBtn = NULL;
    
    if (m_bDone)
    {
        if (isClicked(m_pShop, pTouch))
        {
            m_pShop->setScale(0.95f);
            m_pClickedBtn = m_pShop;
        }
        else if (isClicked(m_pHome, pTouch))
        {
            m_pHome->setScale(0.95f);
            m_pClickedBtn = m_pHome;
        }
        else if (isClicked(m_pRestart, pTouch))
        {
            m_pRestart->setScale(0.95f);
            m_pClickedBtn = m_pRestart;
        }
        else if (isClicked(m_pShare, pTouch))
        {
            m_pClickedBtn = m_pShare;
        }
        return true;
    }
    else if (!m_bPaused)
    {
        handleCellTouchStart(pTouch);
    }
    
    m_pClickedBtn = getBounsBtnClicked(pTouch->getLocation());
    
    return true;
}

void GameSceneBlocks::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        if (NULL != m_pClickedBtn && !isClicked(m_pClickedBtn, pTouch))
        {
            m_pClickedBtn->setScale(1.0f);
            m_pClickedBtn = NULL;
        }
        return;
    }
    else if (!m_bPaused)
    {
        handleCellTouchMove(pTouch);
    }
    
    CCNode * pSender = getBounsBtnClicked(pTouch->getLocation());
    if (pSender != m_pClickedBtn)
    {
        m_pClickedBtn = NULL;
    }
}

void GameSceneBlocks::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone)
    {
        if (NULL != m_pClickedBtn && isClicked(m_pClickedBtn, pTouch))
        {
            m_pClickedBtn->setScale(1.0f);
            
            CCFiniteTimeAction * pAction = NULL;
            if (m_pShop == m_pClickedBtn)
            {
                pAction = CCCallFuncN::create(this, callfuncN_selector(GameSceneBlocks::onShop));
            }
            else if (m_pHome == m_pClickedBtn)
            {
                pAction = CCCallFuncN::create(this, callfuncN_selector(GameSceneBlocks::onHome));
            }
            else if (m_pRestart == m_pClickedBtn)
            {
                pAction = CCCallFuncN::create(this, callfuncN_selector(GameSceneBlocks::onRestart));
            }
            else if (m_pShare == m_pClickedBtn)
            {
                pAction = CCCallFuncN::create(this, callfuncN_selector(GameSceneBlocks::onShare));
            }
            CCDelayTime * pDelay = CCDelayTime::create(0.2f);
            runAction(CCSequence::createWithTwoActions(pDelay, pAction));
        }
        return;
    }
    
    if (m_bPaused)
    {
        resumeGame();
    }
    else
    {
        handleCellTouchEnd(pTouch);
        
        CCNode * pSender = getBounsBtnClicked(pTouch->getLocation());
        if (pSender == m_pClickedBtn)
        {
            if (m_pTimeBounsCount == pSender)
            {
                onTimeBounsClicked();
            }
            if (m_pFillBounsCount == pSender)
            {
                onFillBounsClicked();
            }
            if (m_pSpeedBounsCount == pSender)
            {
                onSpeedBounsClicked();
            }
        }
    }
}

void GameSceneBlocks::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

void GameSceneBlocks::handleCellTouchStart(cocos2d::Touch* pTouch)
{
    m_filledCells.clear();
    checkCellTouch(pTouch);
}

void GameSceneBlocks::handleCellTouchMove(cocos2d::Touch* pTouch)
{
    checkCellTouch(pTouch);
}

void GameSceneBlocks::handleCellTouchEnd(cocos2d::Touch* pTouch)
{
    checkCellTouch(pTouch);
}

// ===========================================================
// Protected methods
// ===========================================================

void GameSceneBlocks::setDoubleFilledCel(int index)
{
    CCNode * pCell = (CCNode *)m_pCells->getChildren().at(index);
    pCell->setTag(TAG_D_FILLING);
}

// ===========================================================
// Private methods
// ===========================================================

float GameSceneBlocks::addHeader()
{
    Color3B c = Pref::getInstance()->getColor(COLOR_BG);
    LayerColor * pHeader = LayerColor::create(ccc4(c.r, c.g, c.b, 255), getScreenWidth(), getScreenWidth() * 0.18f);
    pHeader->setPositionY(getScreenHeight() - pHeader->boundingBox().size.height);
    pHeader->setZOrder(3);
    addChild(pHeader);
    
    float textSize = pHeader->boundingBox().size.height * 0.4f;
    float pad = pHeader->boundingBox().size.width * 0.03f;
    
    m_pScoreLabel = CCLabelTTF::create("score:", fontList[0], textSize*0.5f);
    m_pScoreLabel->setPositionX(pad + m_pScoreLabel->boundingBox().size.width / 2);
    m_pScoreLabel->setPositionY(pHeader->boundingBox().size.height * 0.75f);
    m_pScoreLabel->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pHeader->addChild(m_pScoreLabel);
    
    m_pScore = CCLabelTTF::create("0", fontList[0], textSize);
    m_pScore->setPositionX(m_pScoreLabel->getPositionX() - m_pScoreLabel->boundingBox().size.width / 2 + m_pScore->boundingBox().size.width * 0.65f);
    m_pScore->setPositionY(m_pScoreLabel->getPositionY() - m_pScoreLabel->boundingBox().size.height / 2 - m_pScore->boundingBox().size.height *0.55f);
    m_pScore->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pHeader->addChild(m_pScore);
    
    m_pTimeLabel = CCLabelTTF::create("time:", fontList[0], textSize*0.5f);
    m_pTimeLabel->setPositionX(getScreenWidth() - pad - m_pTimeLabel->boundingBox().size.width / 2);
    m_pTimeLabel->setPositionY(m_pScoreLabel->getPositionY());
    m_pTimeLabel->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pHeader->addChild(m_pTimeLabel);
    
    m_pTime = CCLabelTTF::create("60", fontList[0], textSize);
    m_pTime->setPositionX(m_pTimeLabel->getPositionX() + m_pTimeLabel->boundingBox().size.width / 2 - m_pTime->boundingBox().size.width / 2);
    m_pTime->setPositionY(m_pScore->getPositionY());
    m_pTime->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pHeader->addChild(m_pTime);

    
//    CCLabelTTF * pPercent = CCLabelTTF::create("38%", fontList[0], textSize*0.25f);
//    pPercent->setPositionX(pad + pPercent->boundingBox().size.width / 2);
//    pPercent->setPositionY(pHeader->boundingBox().size.height * 0.25f);
//    pPercent->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
//    pHeader->addChild(pPercent);
    
//    Color3B progressColor = Pref::getInstance()->getColor(COLOR_FILL);
//    LayerColor * pProgress = LayerColor::create(ccc4(progressColor.r, progressColor.g, progressColor.b, 255), getScreenWidth(), getScreenWidth()*0.01f);
//    pProgress->setPositionX(0);
//    pProgress->setPositionY(footerHeight - pProgress->boundingBox().size.height);
//    addChild(pProgress, 3);
    
    
    Color3B cb = Pref::getInstance()->getColor(COLOR_BORDER);
    LayerColor * pLevelBg = LayerColor::create(ccc4(cb.r, cb.g, cb.b, 255), getScreenWidth() * 0.25f, pHeader->boundingBox().size.height);
    pLevelBg->setPositionX(getScreenWidth() / 2 - pLevelBg->boundingBox().size.width / 2);
    pHeader->addChild(pLevelBg);
    
    char levelTextStr[10];
    sprintf(levelTextStr, "%d", getLevelNumber());
    CCLabelTTF * pLevel = CCLabelTTF::create(levelTextStr, fontList[0], textSize*1.4f);
    pLevel->setPositionX(getScreenWidth() / 2);
    pLevel->setPositionY(pHeader->boundingBox().size.height * 0.6f);
    pLevel->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pHeader->addChild(pLevel);
    
    CCLabelTTF * pLevelText = CCLabelTTF::create("level", fontList[0], textSize*0.5f);
    pLevelText->setPositionX(getScreenWidth() / 2);
    pLevelText->setPositionY(pLevel->boundingBox().size.height * 0.35f);
    pLevelText->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pHeader->addChild(pLevelText);

    
    return pHeader->boundingBox().size.height;
}

cocos2d::Node * GameSceneBlocks::buildTimer()
{
    Node * pTimer = Sprite::create("circle.png");
    pTimer->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pTimer->setScale(0.26f);
    
    CCProgressTimer * pTimeProgress = CCProgressTimer::create(Sprite::create("circle.png"));
    pTimeProgress->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pTimeProgress->setType(kCCProgressTimerTypeRadial);
    pTimeProgress->setPercentage(0);
    pTimeProgress->setPosition(ccp(pTimeProgress->boundingBox().size.width / 2, pTimeProgress->boundingBox().size.height / 2));
    pTimer->addChild(pTimeProgress);
    
    Sprite * pTimeProgressOverlay = Sprite::create("circle.png");
    pTimeProgressOverlay->setColor(Pref::getInstance()->getColor(COLOR_BG));
    pTimeProgressOverlay->setPosition(ccp(pTimeProgressOverlay->boundingBox().size.width / 2, pTimeProgressOverlay->boundingBox().size.height / 2));
    pTimeProgressOverlay->setScale(0.94f);
    pTimer->addChild(pTimeProgressOverlay);
    
    pTimer->setVisible(false);
    pTimer->setScale(0);
    
    return pTimer;
}

float GameSceneBlocks::addFooter()
{
    Color3B c = Pref::getInstance()->getColor(COLOR_BG);
    LayerColor * pFooter = LayerColor::create(ccc4(c.r, c.g, c.b, 255), getScreenWidth(), getScreenWidth() * 0.18f);
    pFooter->setZOrder(3);
    addChild(pFooter);
    
    m_pTimeBounsProgress = buildTimer();
    pFooter->addChild(m_pTimeBounsProgress);
    
    Sprite * pTimeIcon = Sprite::create("ic_action_clock.png");
    pTimeIcon->setPositionX(pFooter->boundingBox().size.width * 0.2f);
    pTimeIcon->setPositionY(pFooter->boundingBox().size.height / 2);
    pTimeIcon->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pFooter->addChild(pTimeIcon);
    
    float textSize = pTimeIcon->boundingBox().size.height * 0.5f;
    m_pTimeBounsCount = CCLabelTTF::create("0", fontList[0], textSize);
    m_pTimeBounsCount->setPositionX(pTimeIcon->getPositionX() + pTimeIcon->boundingBox().size.width / 2 + m_pTimeBounsCount->boundingBox().size.width / 2);
    m_pTimeBounsCount->setPositionY(pTimeIcon->getPositionY());
    m_pTimeBounsCount->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pFooter->addChild(m_pTimeBounsCount);
    m_pTimeBounsCount->setUserData(pTimeIcon);
    
    m_pTimeBounsProgress->setPositionX((pTimeIcon->getPositionX() + m_pTimeBounsCount->getPositionX()) * 0.48f);
    m_pTimeBounsProgress->setPositionY(pTimeIcon->getPositionY());
    
    
    m_pFillBounsProgress = buildTimer();
    pFooter->addChild(m_pFillBounsProgress);
    
    Sprite * pFillIcon = Sprite::create("ic_action_blob.png");
    pFillIcon->setPositionX(pFooter->boundingBox().size.width * 0.5f);
    pFillIcon->setPositionY(pFooter->boundingBox().size.height / 2);
    pFillIcon->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pFooter->addChild(pFillIcon);
    
    m_pFillBounsCount = CCLabelTTF::create("0", fontList[0], textSize);
    m_pFillBounsCount->setPositionX(pFillIcon->getPositionX() + pFillIcon->boundingBox().size.width / 2 + m_pFillBounsCount->boundingBox().size.width / 2);
    m_pFillBounsCount->setPositionY(pFillIcon->getPositionY());
    m_pFillBounsCount->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pFooter->addChild(m_pFillBounsCount);
    m_pFillBounsCount->setUserData(pFillIcon);
    
    m_pFillBounsProgress->setPositionX((pFillIcon->getPositionX() + m_pFillBounsCount->getPositionX()) * 0.5f);
    m_pFillBounsProgress->setPositionY(pFillIcon->getPositionY());
    
    m_pSpeedBounsProgress = buildTimer();
    pFooter->addChild(m_pSpeedBounsProgress);
    
    Sprite * pSpeedIcon = Sprite::create("ic_action_flash.png");
    pSpeedIcon->setPositionX(pFooter->boundingBox().size.width * 0.8f);
    pSpeedIcon->setPositionY(pFooter->boundingBox().size.height / 2);
    pSpeedIcon->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pFooter->addChild(pSpeedIcon);
    
    m_pSpeedBounsCount = CCLabelTTF::create("0", fontList[0], textSize);
    m_pSpeedBounsCount->setPositionX(pSpeedIcon->getPositionX() + pSpeedIcon->boundingBox().size.width / 2 + m_pSpeedBounsCount->boundingBox().size.width / 2);
    m_pSpeedBounsCount->setPositionY(pSpeedIcon->getPositionY());
    m_pSpeedBounsCount->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pFooter->addChild(m_pSpeedBounsCount);
    m_pSpeedBounsCount->setUserData(pSpeedIcon);
    
    updateBounsText(m_pTimeBounsCount, Pref::getInstance()->getTimeBounsCount());
    updateBounsText(m_pFillBounsCount, Pref::getInstance()->getFillBounsCount());
    updateBounsText(m_pSpeedBounsCount, Pref::getInstance()->getSpeedBounsCount());
    
    m_pSpeedBounsProgress->setPositionX((pSpeedIcon->getPositionX() + m_pSpeedBounsCount->getPositionX()) * 0.5f);
    m_pSpeedBounsProgress->setPositionY(pSpeedIcon->getPositionY());
    
    return pFooter->boundingBox().size.height;
}

void GameSceneBlocks::buildWorld(float screenWidth, float screenHeight, float headerHeight, float footerHeight)
{
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    m_pWorld = new b2World(gravity);
    
    // Define the ground body
    float size = BORDER_SIZE;
    Color3B border = Pref::getInstance()->getColor(COLOR_BORDER);
    
    { // bottom wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), getScreenWidth(), size);
        pGroundTile->setPosition(ccp(0, footerHeight + size));
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        pGround->CreateFixture(&groundBox, 0);
    }
    { // top wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), getScreenWidth(), size);
        pGroundTile->setPosition(ccp(0, screenHeight - headerHeight - size));
        pGroundTile->setTag(TAG_TOP_WALL);
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        pGround->CreateFixture(&groundBox, 0);
    }
    { // left wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), size, getScreenHeight());
        pGroundTile->setPosition(ccp(0, 0));
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        pGround->CreateFixture(&groundBox, 0);
    }
    { // right wall
        LayerColor * pGroundTile = LayerColor::create(ccc4(border.r, border.g, border.b, 255), size, getScreenHeight());
        pGroundTile->setPosition(ccp(screenWidth, 0));
        pGroundTile->setTag(TAG_RIGHT_WALL);
        pGroundTile->setZOrder(2);
        addChild(pGroundTile);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.userData = pGroundTile;
        groundBodyDef.position.Set(pGroundTile->getPositionX()/PTM_RATIO, pGroundTile->getPositionY()/PTM_RATIO);
        b2Body * pGround = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pGroundTile->boundingBox().size.width/PTM_RATIO, pGroundTile->boundingBox().size.height/PTM_RATIO);
        pGround->CreateFixture(&groundBox, 0);
    }
}

void GameSceneBlocks::runWorld(float dt)
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
            else if (sprite->getTag() == TAG_FILLED)
            {
                sprite->setPositionX(sprite->getPositionX() - sprite->boundingBox().size.width/2);
                sprite->setPositionY(sprite->getPositionY() - sprite->boundingBox().size.height/2);
            }
        }
    }
}

void GameSceneBlocks::checkCellTouch(cocos2d::Touch* pTouch)
{
    for (int i = 0; i < m_pCells->getChildrenCount(); i++)
    {
        CCNode * pCell = (CCNode *)m_pCells->getChildren().at(i);
        if (isClicked(pCell, pTouch))
        {
            if (TAG_EMPTY == pCell->getTag())
            {
                pCell->setTag(TAG_FILLING);
                
                float size = pCell->boundingBox().size.width;
                Color3B c = Pref::getInstance()->getColor(COLOR_FILL);
                LayerColor * pCellFill = LayerColor::create(ccc4(c.r, c.g, c.b, 255), size, size);
                pCell->addChild(pCellFill);
                
                float fillSpeed = FILL_SPEED;
                if (0 != m_fFillBounsTimer)
                {
                    fillSpeed /= 2;
                }
                pCellFill->setScale(0);
                CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameSceneBlocks::onFillCellDone));
                pCellFill->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(fillSpeed, 1), pFillDone));
                
                m_filledCells.push_back(pCell);
                m_iDestScore += m_filledCells.size();
            }
        }
    }
}

void GameSceneBlocks::onFillCellDone(cocos2d::CCNode * pSender)
{
    Node * pCell = (Node *)pSender->getParent();
    pCell->setTag(TAG_FILLED);
    pCell->removeAllChildren();
    pCell->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    
    if (m_bFilledToBlocker)
    {
        b2BodyDef groundBodyDef;
        groundBodyDef.type = b2_kinematicBody;
        groundBodyDef.userData = pCell;
        groundBodyDef.position.Set((pCell->getPositionX() + pCell->boundingBox().size.width/2)/PTM_RATIO, (pCell->getPositionY() + pCell->boundingBox().size.height / 2)/PTM_RATIO);
        b2Body * pBody = m_pWorld->CreateBody(&groundBodyDef);
        
        b2PolygonShape groundBox;
        groundBox.SetAsBox(pCell->boundingBox().size.width/PTM_RATIO/2, pCell->boundingBox().size.height/PTM_RATIO/2);
        pBody->CreateFixture(&groundBox, 0);
    }
    
    int clearedCount = 0;
    for (int i = 0; i < m_pCells->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pCells->getChildren().at(i);
        pNode->stopAllActions();
        if (pNode->getTag() == TAG_FILLED)
        {
            clearedCount++;
        }
    }
    
    if (!m_bDone && clearedCount >= m_pCells->getChildrenCount() * 0.7f)
    {
        onGameOver();
    }
}

void GameSceneBlocks::checkCollisions()
{
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pEnemy = (Node *)m_pEnemies->getChildren().at(i);
        float cx = pEnemy->getPositionX() + pEnemy->boundingBox().size.width / 2;
        float cy = pEnemy->getPositionY() + pEnemy->boundingBox().size.height / 2;
        
        for (int j = 0; j < m_pCells->getChildrenCount(); j++)
        {
            Node * pCell = (Node *)m_pCells->getChildren().at(j);
            if (TAG_FILLING == pCell->getTag())
            {
                float dx = fabs(cx - (pCell->getPositionX() + pCell->boundingBox().size.width / 2));
                float dy = fabs(cy - (pCell->getPositionY() + pCell->boundingBox().size.height / 2));
                float m = max(pCell->boundingBox().size.width / 2, pEnemy->boundingBox().size.width / 2);
                if (dx < m && dy < m)
                {
                    onGameOver();
                    return;
                }
            }
        }
    }
}

void GameSceneBlocks::onGameOver()
{
    m_bDone = true;
        
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pEnemies->getChildren().at(i);
        pNode->stopAllActions();
    }
    int clearedCount = 0;
    for (int i = 0; i < m_pCells->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pCells->getChildren().at(i);
        pNode->stopAllActions();
        if (pNode->getTag() == TAG_FILLED)
        {
            clearedCount++;
        }
    }
    
    if (clearedCount >= m_pCells->getChildrenCount() * 0.7f)
    {
        showGameOverScreen(true, "cleared!");
    }
    else
    {
        showGameOverScreen(false, "failed!");
    }
}

void GameSceneBlocks::updateScoreText()
{
    char text[30];
    sprintf(text, "%d", m_iScore);
    m_pScore->setString(text);
    m_pScore->setPositionX(m_pScoreLabel->getPositionX() - m_pScoreLabel->boundingBox().size.width / 2 + m_pScore->boundingBox().size.width * 0.5f);
}

void GameSceneBlocks::updateTimeText()
{
    char text[30];
    sprintf(text, "%d", m_iTime);
    m_pTime->setString(text);
    m_pTime->setPositionX(m_pTimeLabel->getPositionX() + m_pTimeLabel->boundingBox().size.width / 2 - m_pTime->boundingBox().size.width * 0.5f);
}

void GameSceneBlocks::updateBounsText(cocos2d::CCLabelTTF * pText, int value)
{
    char text[30];
    sprintf(text, "%d", value);
    
    Sprite * pIcon = (Sprite *)pText->getUserData();
    pText->setString(text);
    pText->setPositionX(pIcon->getPositionX() + pIcon->boundingBox().size.width / 2 + pText->boundingBox().size.width / 2);
}

cocos2d::CCNode * GameSceneBlocks::getBounsBtnClicked(cocos2d::Vec2 loc)
{
    if (isBounsBtnClicked(m_pTimeBounsCount, loc))
    {
        return m_pTimeBounsCount;
    }
    if (isBounsBtnClicked(m_pFillBounsCount, loc))
    {
        return m_pFillBounsCount;
    }
    if (isBounsBtnClicked(m_pSpeedBounsCount, loc))
    {
        return m_pSpeedBounsCount;
    }
    return NULL;
}

bool GameSceneBlocks::isBounsBtnClicked(cocos2d::CCLabelTTF * pBtn, cocos2d::Vec2 loc)
{
    if (loc.y > m_fFooterHeight)
    {
        return false;
    }
    
    float dx = fabs(pBtn->getPositionX() - loc.x);
    float dy = fabs(pBtn->getPositionY() - loc.y);
    if (dx < getScreenWidth() * 0.2f && dy < getScreenHeight() * 0.1f)
    {
        return pBtn;
    }
    return NULL;
}

void GameSceneBlocks::showGameOverScreen(bool cleared, const char * titleText)
{
    float d = 0.75f;
    
    LayerColor * pOverlay = LayerColor::create(ccc4(0, 0, 0, 0), getScreenWidth(), getScreenHeight());
    addChild(pOverlay, 10);
    
    pOverlay->runAction(CCFadeTo::create(d, 190));
    
    Color3B bg = Pref::getInstance()->getColor(COLOR_BG);
    LayerColor * pPanel = LayerColor::create(ccc4(bg.r, bg.g, bg.b, 255), getScreenWidth() * 0.6f, getScreenHeight() * 0.5f);
    pPanel->setPositionX(getScreenWidth() / 2 - pPanel->boundingBox().size.width / 2);
    pPanel->setPositionY(-pPanel->boundingBox().size.height);
    pOverlay->addChild(pPanel);
    
    float panelY = getScreenHeight() / 2 - pPanel->boundingBox().size.height / 2;
    CCMoveTo * pMoveTo = CCMoveTo::create(d, ccp(pPanel->getPositionX(), panelY));
    pPanel->runAction(CCEaseExponentialOut::create(pMoveTo));
    
    Color3B fill = Pref::getInstance()->getColor(COLOR_ENEMY);
    LayerColor * pPanelTop = LayerColor::create(ccc4(fill.r, fill.g, fill.b, 255), pPanel->boundingBox().size.width, getScreenWidth() * 0.12f);
    pPanelTop->setPositionX(0);
    pPanelTop->setPositionY(pPanel->boundingBox().size.height - pPanelTop->boundingBox().size.height);
    pPanel->addChild(pPanelTop);
    
    CCLabelTTF * pTitle = CCLabelTTF::create(titleText, fontList[0], pPanelTop->boundingBox().size.height * 0.65f);
    pTitle->setPositionX(pPanelTop->boundingBox().size.width * 0.51f);
    pTitle->setPositionY(pPanelTop->boundingBox().size.height * 0.49f);
    pTitle->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pPanelTop->addChild(pTitle);
    
    CCLabelTTF * pScoreTitle = CCLabelTTF::create("your score:", fontList[0], pPanel->boundingBox().size.height * 0.06f);
    pScoreTitle->setPositionX(pPanelTop->boundingBox().size.width / 2);
    pScoreTitle->setPositionY(pPanelTop->getPositionY() - pPanelTop->boundingBox().size.height - pScoreTitle->boundingBox().size.height / 2);
    pScoreTitle->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pPanel->addChild(pScoreTitle);
    
    char text[10];
    sprintf(text, "%d", m_iScore);
    CCLabelTTF * pScore = CCLabelTTF::create(text, fontList[0], pPanel->boundingBox().size.height * 0.27f);
    pScore->setPositionX(pScoreTitle->getPositionX());
    pScore->setPositionY(pScoreTitle->getPositionY() - pScoreTitle->boundingBox().size.height * 0.7f - pScore->boundingBox().size.height / 2);
    pScore->setColor(Pref::getInstance()->getColor(COLOR_BORDER));
    pPanel->addChild(pScore);
    
    Color3B btnC = Pref::getInstance()->getColor(COLOR_BORDER);
    float btnSize = pPanel->boundingBox().size.width * 0.25f;
    float pad = btnSize * 1.25f;
    m_pRestart = LayerColor::create(ccc4(btnC.r, btnC.g, btnC.b, 255), btnSize, btnSize);
    m_pRestart->setPositionX(pPanel->boundingBox().size.width / 2 - pad - btnSize / 2);
    m_pRestart->setPositionY(btnSize * 0.35f);
    pPanel->addChild(m_pRestart);
    
    m_pHome = LayerColor::create(ccc4(btnC.r, btnC.g, btnC.b, 255), btnSize, btnSize);
    m_pHome->setPositionX(pPanel->boundingBox().size.width / 2 - btnSize / 2);
    m_pHome->setPositionY(m_pRestart->getPositionY());
    pPanel->addChild(m_pHome);
    
    m_pShop = LayerColor::create(ccc4(btnC.r, btnC.g, btnC.b, 255), btnSize, btnSize);
    m_pShop->setPositionX(pPanel->boundingBox().size.width / 2 + pad - btnSize / 2);
    m_pShop->setPositionY(m_pRestart->getPositionY());
    pPanel->addChild(m_pShop);
    
    m_pShare = LayerColor::create(ccc4(btnC.r, btnC.g, btnC.b, 255), pPanel->boundingBox().size.width, getScreenWidth() * 0.12f);
    m_pShare->setPositionX(0);
    m_pShare->setPositionY(0 - m_pShare->boundingBox().size.height);
    pPanel->addChild(m_pShare);
    
    CCLabelTTF * pShareText = CCLabelTTF::create("SHARE", fontList[0], m_pShare->boundingBox().size.height * 0.7f);
    pShareText->setPositionX(m_pShare->boundingBox().size.width * 0.51f);
    pShareText->setPositionY(m_pShare->boundingBox().size.height * 0.49f);
    pShareText->setColor(bg);
    m_pShare->addChild(pShareText);
    
    CCSprite * pRestartIcon = CCSprite::create("ic_action_reload.png");
    pRestartIcon->setColor(bg);
    pRestartIcon->setPositionX(m_pRestart->boundingBox().size.width / 2);
    pRestartIcon->setPositionY(m_pRestart->boundingBox().size.height / 2);
    m_pRestart->addChild(pRestartIcon);
    
    CCSprite * pHomeIcon = CCSprite::create("ic_action_home.png");
    pHomeIcon->setColor(bg);
    pHomeIcon->setPositionX(m_pHome->boundingBox().size.width / 2);
    pHomeIcon->setPositionY(m_pHome->boundingBox().size.height / 2);
    m_pHome->addChild(pHomeIcon);
    
    CCSprite * pShopIcon = CCSprite::create("ic_action_arrow_right.png");
    pShopIcon->setColor(bg);
    pShopIcon->setPositionX(m_pShop->boundingBox().size.width / 2);
    pShopIcon->setPositionY(m_pShop->boundingBox().size.height / 2);
    m_pShop->addChild(pShopIcon);
    
    LayerColor * pFooter = LayerColor::create(ccc4(fill.r, fill.g, fill.b, 255), pPanel->boundingBox().size.width, getScreenWidth() * 0.01f);
    pFooter->setPositionX(0);
    pFooter->setPositionY(m_pShare->getPositionY() - pFooter->boundingBox().size.height);
    pPanel->addChild(pFooter);
}

void GameSceneBlocks::onHome(cocos2d::CCNode * pSender)
{
    switchScene(MainScene::scene());
}

void GameSceneBlocks::onShop(cocos2d::CCNode * pSender)
{
    switchScene(GameScene_2Balls::scene(getLevelNumber() + 1));
}

void GameSceneBlocks::onShare(cocos2d::CCNode * pSender)
{
    SystemUtils::getInstance()->shareScore(m_iScore);
}

void GameSceneBlocks::onTimeBounsClicked()
{
    int count = Pref::getInstance()->getTimeBounsCount();
    if (count > 0)
    {
        if (0 == m_fTimeBounsTimer)
        {
            count--;
            Pref::getInstance()->setTimeBounsCount(count);
            Pref::getInstance()->save();
            
            updateBounsText(m_pTimeBounsCount, count);
            
            m_fTimeBounsTimer = 0.001f;
            m_pTimeBounsProgress->setVisible(true);
            m_pTimeBounsProgress->stopAllActions();
            m_pTimeBounsProgress->setScale(0);
            m_pTimeBounsProgress->runAction(CCScaleTo::create(0.35f, 0.22f));
        }
    }
    else
    {
        showBuyPopup();
    }
}

void GameSceneBlocks::onFillBounsClicked()
{
    int count = Pref::getInstance()->getFillBounsCount();
    if (count > 0)
    {
        if (0 == m_fFillBounsTimer)
        {
            count--;
            Pref::getInstance()->setFillBounsCount(count);
            Pref::getInstance()->save();
            
            updateBounsText(m_pFillBounsCount, count);
            
            m_fFillBounsTimer = 0.001f;
            m_pFillBounsProgress->setVisible(true);
            m_pFillBounsProgress->stopAllActions();
            m_pFillBounsProgress->setScale(0);
            m_pFillBounsProgress->runAction(CCScaleTo::create(0.35f, 0.22f));
        }
    }
    else
    {
        showBuyPopup();
    }

}

void GameSceneBlocks::onSpeedBounsClicked()
{
    int count = Pref::getInstance()->getSpeedBounsCount();
    if (count > 0)
    {
        if (0 == m_fSpeedBounsTimer)
        {
            count--;
            Pref::getInstance()->setSpeedBounsCount(count);
            Pref::getInstance()->save();
            
            updateBounsText(m_pSpeedBounsCount, count);
            
            m_fSpeedBounsTimer = 0.001f;
            m_pSpeedBounsProgress->setVisible(true);
            m_pSpeedBounsProgress->stopAllActions();
            m_pSpeedBounsProgress->setScale(0);
            m_pSpeedBounsProgress->runAction(CCScaleTo::create(0.35f, 0.22f));
            
            enemySlowdown(m_pEnemies, true);
        }
    }
    else
    {
        showBuyPopup();
    }

}

void GameSceneBlocks::showBuyPopup()
{
    pauseGame();
}

void GameSceneBlocks::pauseGame()
{
    m_bPaused = true;
    
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pEnemies->getChildren().at(i);
        pNode->pause();
    }
    for (int i = 0; i < m_pCells->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pCells->getChildren().at(i);
        pNode->pause();
    }
}

void GameSceneBlocks::resumeGame()
{
    for (int i = 0; i < m_pEnemies->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pEnemies->getChildren().at(i);
        pNode->resume();
    }
    for (int i = 0; i < m_pCells->getChildrenCount(); i++)
    {
        Node * pNode = (Node *)m_pCells->getChildren().at(i);
        pNode->resume();
    }
    
    m_bPaused = false;
}

void GameSceneBlocks::updateBounsTimers(float dt)
{
    if (m_fTimeBounsTimer > 0)
    {
        m_fTimeBounsTimer += dt;
        if (m_fTimeBounsTimer > BOUNS_TIME_DURATION)
        {
            m_fTimeBounsTimer = BOUNS_TIME_DURATION;
        }
        
        float progress = 100 * (m_fTimeBounsTimer / BOUNS_TIME_DURATION);
        ((CCProgressTimer *)m_pTimeBounsProgress->getChildren().at(0))->setPercentage(100 - progress);
        
        if (BOUNS_TIME_DURATION == m_fTimeBounsTimer)
        {
            m_fTimeBounsTimer = 0;
            m_pTimeBounsProgress->runAction(FadeTo::create(0.3f, 0));
        }
    }
    
    if (m_fFillBounsTimer > 0)
    {
        m_fFillBounsTimer += dt;
        if (m_fFillBounsTimer > BOUNS_FILL_DURATION)
        {
            m_fFillBounsTimer = BOUNS_FILL_DURATION;
        }
        
        float progress = 100 * (m_fFillBounsTimer / BOUNS_FILL_DURATION);
        ((CCProgressTimer *)m_pFillBounsProgress->getChildren().at(0))->setPercentage(100 - progress);
        
        if (BOUNS_FILL_DURATION == m_fFillBounsTimer)
        {
            m_fFillBounsTimer = 0;
            m_pFillBounsProgress->runAction(FadeTo::create(0.3f, 0));
        }
    }
    
    if (m_fSpeedBounsTimer > 0)
    {
        m_fSpeedBounsTimer += dt;
        if (m_fSpeedBounsTimer > BOUNS_SPEED_DURATION)
        {
            m_fSpeedBounsTimer = BOUNS_SPEED_DURATION;
        }
        
        float progress = 100 * (m_fSpeedBounsTimer / BOUNS_SPEED_DURATION);
        ((CCProgressTimer *)m_pSpeedBounsProgress->getChildren().at(0))->setPercentage(100 - progress);
        
        if (BOUNS_SPEED_DURATION == m_fSpeedBounsTimer)
        {
            m_fSpeedBounsTimer = 0;
            m_pSpeedBounsProgress->runAction(FadeTo::create(0.3f, 0));
            enemySlowdown(m_pEnemies, false);
        }
    }
}