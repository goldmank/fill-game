#include "GameScene_TilePatternMatch.h"
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
// Initialization & cleanup
// ===========================================================

GameScene_TilePatternMatch::GameScene_TilePatternMatch()
{
    srand(time(NULL));
    m_bReady = true;
    m_pFirstTile = NULL;
    m_pSecondTile = NULL;
}

GameScene_TilePatternMatch::~GameScene_TilePatternMatch()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_TilePatternMatch::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_TilePatternMatch *layer = GameScene_TilePatternMatch::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_TilePatternMatch::handleInit(float screenWidth, float screenHeight)
{
    buildMap();
}

void GameScene_TilePatternMatch::handleUpdate(float dt)
{
    
}

bool GameScene_TilePatternMatch::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (m_bReady)
    {
        m_pTouchedCell = NULL;
        for (int i = 0; NULL == m_pTouchedCell && i < MAP_WIDTH * MAP_HEIGHT; i++)
        {
            if (isClicked(m_pMap[i], pTouch))
            {
                m_pTouchedCell = m_pMap[i];
                m_pTouchedCell->setOpacity(200);
            }
        }
    }
    
    return true;
}

void GameScene_TilePatternMatch::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedCell && !isClicked(m_pTouchedCell, pTouch))
    {
        m_pTouchedCell->setOpacity(255);
    }
}

void GameScene_TilePatternMatch::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedCell)
    {
        if (NULL == m_pFirstTile)
        {
            m_pFirstTile = m_pTouchedCell;
        }
        else
        {
            m_pSecondTile = m_pTouchedCell;
            
            m_pFirstTile->setOpacity(255);
            m_pSecondTile->setOpacity(255);
            
            switchTiles(m_pFirstTile, m_pSecondTile);
        }
        
        m_pTouchedCell = NULL;
    }
}

void GameScene_TilePatternMatch::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_TilePatternMatch::buildMap()
{
    const int map[MAP_WIDTH * MAP_HEIGHT] =
    {
        0, 0, 0, 0,
        2, 1, 2, 0,
        0, 1, 0, 0,
        0, 0, 0, 0
    };
    
    Sprite * pGridTile = Sprite::create("tile_empty.png");
    float cx = getScreenWidth() / 2;
    float cy = getScreenHeight() / 2;
    float w = pGridTile->boundingBox().size.width * MAP_WIDTH;
    float h = pGridTile->boundingBox().size.height * MAP_HEIGHT;
    float y = cy - h / 2;
    for (int i = MAP_HEIGHT - 1; i >= 0; i--)
    {
        float x = cx - w / 2;
        
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int tag = map[i * MAP_WIDTH + j];
            
            Sprite * pTile = Sprite::create("tile_empty.png");
            switch (tag)
            {
                case 0 : break;
                case 1 : pTile->setColor(Color3B(255, 160, 0)); break;
                case 2 : pTile->setColor(Color3B(255, 0, 255)); break;
            }
            pTile->setTag(tag);
            pTile->setPosition(x + pTile->boundingBox().size.width / 2, y + pTile->boundingBox().size.height / 2);
            addChild(pTile);
            
            Cell * pCell = new Cell();
            pCell->index = i;
            pCell->m_pTile = pTile;
            pTile->setUserData(pCell);
            
            m_pMap[i * MAP_WIDTH + j] = pTile;
            
            x += pTile->boundingBox().size.width;
            
            if (j == MAP_WIDTH - 1)
            {
                y += pTile->boundingBox().size.height;
            }
        }
    }
    
    for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
    {
        Sprite * pTile = m_pMap[i];
        Cell * pCell = (Cell *)pTile->getUserData();
        
        int row = i / MAP_WIDTH;
        int col = i % MAP_WIDTH;
        
        if (col - 1 >= 0)
        {
            int index = row * MAP_WIDTH + (col - 1);
            Sprite * pLeftTile = m_pMap[index];
            Cell * pLeftCell = (Cell *)pLeftTile->getUserData();
            pCell->m_neighbours.push_back(pLeftCell);
        }
        if (col + 1 < MAP_WIDTH)
        {
            int index = row * MAP_WIDTH + (col + 1);
            Sprite * pRightTile = m_pMap[index];
            Cell * pRightCell = (Cell *)pRightTile->getUserData();
            pCell->m_neighbours.push_back(pRightCell);
        }
        if (row - 1 >= 0)
        {
            int index = (row - 1) * MAP_WIDTH + col;
            Sprite * pTopTile = m_pMap[index];
            Cell * pTopCell = (Cell *)pTopTile->getUserData();
            pCell->m_neighbours.push_back(pTopCell);
        }
        if (row + 1 < MAP_HEIGHT)
        {
            int index = (row + 1) * MAP_WIDTH + col;
            Sprite * pBottomTile = m_pMap[index];
            Cell * pBottomCell = (Cell *)pBottomTile->getUserData();
            pCell->m_neighbours.push_back(pBottomCell);
        }
    }
    
    Sprite * pExample = Sprite::create("ex.png");
    pExample->setPosition(getScreenWidth() * 0.5f, getScreenHeight() * 0.15f);
    addChild(pExample);
}

void GameScene_TilePatternMatch::switchTiles(cocos2d::Sprite * pTile1, cocos2d::Sprite * pTile2)
{
    m_bReady = false;
    
    float d = 0.5f;
    pTile1->runAction(MoveTo::create(d, pTile2->getPosition()));
    
    CCFiniteTimeAction* pMoveDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_TilePatternMatch::onMoveDone));
    pTile2->runAction(Sequence::createWithTwoActions(MoveTo::create(d, pTile1->getPosition()), pMoveDone));
}

void GameScene_TilePatternMatch::onMoveDone(cocos2d::Node * pPlayer)
{
    m_bReady = true;
    m_pFirstTile = NULL;
    m_pSecondTile = NULL;
}