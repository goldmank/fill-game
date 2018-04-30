#include "GameScene_MrRun.h"
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

#define JUMP_DURATION (0.1f)
#define JUMP_MAX_DURATION (JUMP_DURATION*3)
#define JUMP_SCALE (0.5f)
#define JUMP_SLIDE_SIZE (6)

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_MrRun::GameScene_MrRun()
{
    srand(time(NULL));
    m_bDone = false;
    m_fSpeed = 320;
    m_bJumping = false;
    m_fJumpStartTimer = 0;
    m_fFallStartTime = 0;
    m_bTouchDown = false;
    m_pFallToTile = NULL;
    m_bDisableTouch = false;
}

GameScene_MrRun::~GameScene_MrRun()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_MrRun::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_MrRun *layer = GameScene_MrRun::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_MrRun::handleInit(float screenWidth, float screenHeight)
{
    m_pGameLayer = Layer::create();
    addChild(m_pGameLayer);
    
    buildLevel();
    
    Sprite * pFirstTile = (Sprite *)m_pGameLayer->getChildren().at(0);
    m_pPlayer = Sprite::create("grid_game_player.png");
    float destHeight = pFirstTile->boundingBox().size.height / 2;
    m_pPlayer->setScale(destHeight / m_pPlayer->boundingBox().size.height);
    m_pPlayer->setPosition(pFirstTile->getPosition());
    addChild(m_pPlayer);
    m_pPlayer->setUserData(m_pGameLayer->getChildren().at(0));
    
    m_iDistance = 0;
    m_pDistanceText = CCLabelTTF::create("0", fontList[0], screenWidth * 0.1f);
    m_pDistanceText->setPositionX(screenWidth * 0.05f + m_pDistanceText->boundingBox().size.width / 2);
    m_pDistanceText->setPositionY(screenHeight * 0.95f);
    m_pDistanceText->setColor(Color3B(250, 0, 250));
    addChild(m_pDistanceText);
}

void GameScene_MrRun::handleUpdate(float dt)
{
    if (m_bDone)
    {
        return;
    }

    // move the map
    float newY = m_pGameLayer->getPositionY() - dt * m_fSpeed;
    m_pGameLayer->setPositionY(newY);
    
    // remove unused tiles
    bool updateDistance = false;
    Sprite * pBottomTile = (Sprite *)m_pGameLayer->getChildren().at(0);
    while (newY + pBottomTile->getPositionY() + pBottomTile->boundingBox().size.height / 2 < 0)
    {
        m_pGameLayer->removeChild(pBottomTile);
        pBottomTile = (Sprite *)m_pGameLayer->getChildren().at(0);
        m_iDistance++;
        updateDistance = true;
    }
    
    float tx = m_pDistanceText->getPositionX() - m_pDistanceText->boundingBox().size.width / 2;
    char distText[10];
    sprintf(distText, "%d", m_iDistance);
    m_pDistanceText->setString(distText);
    m_pDistanceText->setPositionX(tx + m_pDistanceText->boundingBox().size.width / 2);
    
    // player position on map
    float playerY = -m_pGameLayer->getPositionY() + m_pPlayer->getPositionY();
    
    // update player tile
    Sprite * pPlayerTile = NULL;
    for (int i = 0; NULL == pPlayerTile && i < m_pGameLayer->getChildrenCount(); i++)
    {
        Sprite * pTile = (Sprite *)m_pGameLayer->getChildren().at(i);
        pTile->setColor(Color3B(255,255,255));
        float dx = fabsf(pTile->getPositionX() - m_pPlayer->getPositionX());
        float dy = fabsf(pTile->getPositionY() - playerY);
        if (dx < pBottomTile->boundingBox().size.width / 2 && dy < pBottomTile->boundingBox().size.height / 2)
        {
            pPlayerTile = pTile;
            pTile->setColor(Color3B(150,150,150));
        }
    }
    
    if (!m_bJumping)
    {
        if (NULL == pPlayerTile || 0 == pPlayerTile->getTag())
        {
            onGameOver();
        }
        else if (NULL != pPlayerTile && 2 == pPlayerTile->getTag())
        {
            startJumpUp();
            m_bDisableTouch = true;
        }
    }
    else // jumping
    {
        float newPlayerX = m_pPlayer->getPositionX();
        
        Sprite * pNextTile = m_pFallToTile;
        if (NULL == pNextTile)
        {
            pNextTile = (Sprite *)pBottomTile->getUserData();
        }
        if (newPlayerX < pNextTile->getPositionX())
        {
            newPlayerX += dt * pNextTile->boundingBox().size.width * JUMP_SLIDE_SIZE;
            if (newPlayerX > pNextTile->getPositionX())
            {
                newPlayerX = pNextTile->getPositionX();
            }
        }
        else if (newPlayerX > pNextTile->getPositionX())
        {
            newPlayerX -= dt * pNextTile->boundingBox().size.width * JUMP_SLIDE_SIZE;
            if (newPlayerX < pNextTile->getPositionX())
            {
                newPlayerX = pNextTile->getPositionX();
            }
        }
        
        m_pPlayer->setPositionX(newPlayerX);
        
        m_fJumpStartTimer += dt;
        if (m_fJumpStartTimer < JUMP_DURATION)
        {
            float progress = m_fJumpStartTimer / JUMP_DURATION;
            m_pPlayer->setScale(1.0f + progress*JUMP_SCALE);
        }
        else if (m_bTouchDown) // continue in air
        {
            if (m_fJumpStartTimer > JUMP_MAX_DURATION)
            {
                m_bTouchDown = false;
            }
        }
        else // tap released -> start fall
        {
            if (0 == m_fFallStartTime)
            {
                m_fFallStartTime = getTime();
                m_pFallToTile = pNextTile;
            }
            else
            {
                float dt = getTime() - m_fFallStartTime;
                if (dt > JUMP_DURATION)
                {
                    dt = JUMP_DURATION;
                    m_bJumping = false;
                    m_fJumpStartTimer = 0;
                    m_fFallStartTime = 0;
                    m_bTouchDown = false;
                    m_pFallToTile = NULL;
                    m_bDisableTouch = false;
                }
                
                float progress = 1.0f - (dt / JUMP_DURATION);
                m_pPlayer->setScale(1.0f + progress*JUMP_SCALE);
            }
        }
    }
}

bool GameScene_MrRun::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDone || m_bDisableTouch)
    {
        return true;
    }
    
    startJumpUp();
    
    return true;
}

void GameScene_MrRun::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
}

void GameScene_MrRun::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bDisableTouch)
    {
        return;
    }
    m_bTouchDown = false;
}

void GameScene_MrRun::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_MrRun::buildLevel()
{
    int mapWidth = 5;
    int currXIndex = 2;
    
    std::vector<int> tiles;
    for (int i = 0; i < 250; i++)
    {
        if (i > 5 && 0 == rand() % 6 && tiles[i - 1] != 2)
        {
            tiles.push_back(2);
        }
        else if (i > 5 && 0 == rand() % 5 && tiles[i - 1] != 2)
        {
            tiles.push_back(0);
            tiles.push_back(0);
            tiles.push_back(2);
            tiles.push_back(3);
            tiles.push_back(2);
        }
        else
        {
            if (i > 5 && rand() % 2 == 0 && tiles[i - 1] != 2)
            {
                int dir = rand() % 2 == 0 ? 1 : -1;
                while (currXIndex + dir < 0 || currXIndex + dir >= mapWidth)
                {
                    dir = rand() % 2 == 0 ? 1 : -1;
                }
                tiles.push_back(dir);
                
                currXIndex += dir;
            }
            else
            {
                tiles.push_back(0);
            }
        }
    }
    
    
    float y = 0;
    float x = getScreenWidth() / 2;
    Sprite * pPrev = NULL;
    for (int i = 0; i < tiles.size(); i++)
    {
        Sprite * pTile = NULL;
        switch (tiles[i])
        {
            case 0 :
            case -1 :
            case 1 :
            {
                pTile = Sprite::create("tile_empty.png");
                pTile->setTag(1);
            } break;
            case 2 :
            {
                pTile = Sprite::create("tile_trans.png");
                pTile->setTag(0);
            } break;
            case 3 :
            {
                pTile = Sprite::create("tile_jump.png");
                pTile->setTag(2);
            } break;
        }
        
        float destWidth = getScreenWidth() / mapWidth;
        pTile->setScale(destWidth / pTile->boundingBox().size.width);
        
        if (tiles[i] == -1)
        {
            x -= pTile->boundingBox().size.width;
        }
        else if (tiles[i] == 1)
        {
            x += pTile->boundingBox().size.width;
        }
        
        pTile->setPosition(x, y + pTile->boundingBox().size.height / 2);
        m_pGameLayer->addChild(pTile);
        
        if (NULL != pPrev)
        {
            pPrev->setUserData(pTile);
        }
        y += pTile->boundingBox().size.height;
        
        pPrev = pTile;
    }
}

void GameScene_MrRun::onGameOver()
{
    m_bDone = true;
    switchScene(GameScene_MrRun::scene());
}

void GameScene_MrRun::startJumpUp()
{
    if (m_bJumping)
    {
        return;
    }
    
    m_bJumping = true;
    m_fJumpStartTimer = 0;
    m_bTouchDown = true;
    m_pFallToTile = NULL;
    m_fJumpStartTimer = 0;
    m_fFallStartTime = 0;
}