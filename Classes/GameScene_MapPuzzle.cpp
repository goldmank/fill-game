#include "GameScene_MapPuzzle.h"
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

// 0 - free walk
// S - start
// X - end
// V - move the player one cell down
// ^ - move the player one cell up
// > - move the player one cell right
// < - move the player one cell left
// B - Blocking wall
// Z - free walk that turns to Blocking wall
// v - move the player one cell down and then turn to free walk

//const char map[MAP_WIDTH * MAP_HEIGHT] =
//{
//    '0', 'V', '0', 'X', '0',
//    '>', '0', 'B', '0', '<',
//    '0', '^', 'V', 'A', 'B',
//    'B', '0', '0', '0', '0',
//    'V', '0', '>', '^', 'Z',
//    '0', '0', '^', '0', 'Z',
//    '0', '0', 'S', '0', '0'
//};

const char map[MAP_WIDTH * MAP_HEIGHT] =
{
    '0', '0', 'B', '0', '0',
    'V', '0', 'v', '>', 'X',
    '0', '0', '0', '<', 'B',
    '^', '0', '0', '0', '0',
    '0', '<', '0', 'B', '^',
    'S', '0', '^', '0', '0',
    '>', 'Z', '0', '0', '0'
};

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_MapPuzzle::GameScene_MapPuzzle()
{
    srand(time(NULL));
    m_bReady = true;
}

GameScene_MapPuzzle::~GameScene_MapPuzzle()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_MapPuzzle::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_MapPuzzle *layer = GameScene_MapPuzzle::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_MapPuzzle::handleInit(float screenWidth, float screenHeight)
{
    m_pPlayer = Sprite::create("grid_game_player.png");
    
    buildMap();
    
    addChild(m_pPlayer);
}

void GameScene_MapPuzzle::handleUpdate(float dt)
{
    
}

bool GameScene_MapPuzzle::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pTouchedCell = NULL;
    
    if (m_bReady)
    {
        int playerIndex = m_pPlayer->getTag();
        int playerRow = playerIndex / MAP_WIDTH;
        int playerCol = playerIndex % MAP_WIDTH;
        
        int startRow = playerRow - 1;
        if (startRow < 0)
        {
            startRow = 0;
        }
        int startCol = playerCol - 1;
        if (startCol < 0)
        {
            startCol = 0;
        }
        int endRow = playerRow + 1;
        if (endRow >= MAP_HEIGHT)
        {
            endRow = MAP_HEIGHT - 1;
        }
        int endCol = playerCol + 1;
        if (endCol >= MAP_WIDTH)
        {
            endCol = MAP_WIDTH - 1;
        }
        
        for (int i = startRow; NULL == m_pTouchedCell && i <= endRow; i++)
        {
            for (int j = startCol; NULL == m_pTouchedCell && j <= endCol; j++)
            {
                int index = i * MAP_WIDTH + j;
                if (index != playerIndex)
                {
                    Sprite * pCell = m_pMap[index];
                    if (isClicked(pCell, pTouch))
                    {
                        m_pTouchedCell = pCell;
                        m_pTouchedCell->setColor(Color3B(200, 200, 200));
                    }
                }
            }
        }
    }
    
    return true;
}

void GameScene_MapPuzzle::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedCell && !isClicked(m_pTouchedCell, pTouch))
    {
        m_pTouchedCell->setColor(Color3B(255, 255, 255));
    }
}

void GameScene_MapPuzzle::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedCell)
    {
        m_pTouchedCell->setColor(Color3B(255, 255, 255));
        
        if ('B' == m_pTouchedCell->getTag())
        {
            m_pTouchedCell->setColor(Color3B(160, 160, 160));
        }
        
        if (isClicked(m_pTouchedCell, pTouch))
        {
            moveTo(m_pTouchedCell);
        }
        m_pTouchedCell = NULL;
    }
}

void GameScene_MapPuzzle::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_MapPuzzle::buildMap()
{
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
            
            Sprite * pTile = NULL;
            switch (tag)
            {
                case '^' : pTile = Sprite::create("grid_tile_up.png"); break;
                case 'v' :
                case 'V' : pTile = Sprite::create("grid_tile_up.png"); pTile->setRotation(180); break;
                case '>' : pTile = Sprite::create("grid_tile_up.png"); pTile->setRotation(90); break;
                case 'A' :
                case '<' : pTile = Sprite::create("grid_tile_up.png"); pTile->setRotation(270); break;
                case 'B' : pTile = Sprite::create("tile_empty.png"); pTile->setColor(Color3B(160, 160, 160)); break;
                case 'X' : pTile = Sprite::create("grid_target.png"); break;
                case 'S' :
                {
                    pTile = Sprite::create("tile_empty.png");
                    m_pPlayer->setPosition(x + pTile->boundingBox().size.width / 2, y + pTile->boundingBox().size.height / 2);
                    m_pPlayer->setTag(i * MAP_WIDTH + j);
                    tag = '0';
                } break;
                default  : pTile = Sprite::create("tile_empty.png"); break;
            }
            
            pTile->setTag(tag);
            pTile->setPosition(x + pTile->boundingBox().size.width / 2, y + pTile->boundingBox().size.height / 2);
            addChild(pTile);
            
            m_pMap[i * MAP_WIDTH + j] = pTile;
            
            x += pTile->boundingBox().size.width;
            
            if (j == MAP_WIDTH - 1)
            {
                y += pTile->boundingBox().size.height;
            }
        }
    }
}

void GameScene_MapPuzzle::moveTo(cocos2d::Sprite * pCell)
{
    if ('B' == pCell->getTag())
    {
        return;
    }
    m_bReady = false;
    MoveTo * pMoveTo = MoveTo::create(0.2f, pCell->getPosition());
    CCFiniteTimeAction* pMoveDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_MapPuzzle::onMoveDone));
    m_pPlayer->stopAllActions();
    m_pPlayer->runAction(Sequence::createWithTwoActions(pMoveTo, pMoveDone));
    
    int playerIndex = m_pPlayer->getTag();
    switch (m_pMap[playerIndex]->getTag())
    {
        case 'Z' :
        {
            m_pMap[playerIndex]->setColor(Color3B(160, 160, 160));
            m_pMap[playerIndex]->setTag('B');
        } break;
    }
}

void GameScene_MapPuzzle::onMoveDone(cocos2d::Node * pPlayer)
{
    int index = -1;
    int count = MAP_WIDTH * MAP_HEIGHT;
    for (int i = 0; -1 == index && i < count; i++)
    {
        Sprite * pTile = m_pMap[i];
        float dx = pPlayer->getPositionX() - pTile->getPositionX();
        float dy = pPlayer->getPositionY() - pTile->getPositionY();
        if (fabs(dx) < 5 && fabs(dy) < 5)
        {
            index = i;
        }
    }
    
    m_pPlayer->setTag(index);
    int row = index / MAP_WIDTH;
    int col = index % MAP_WIDTH;
    
    switch (m_pMap[index]->getTag())
    {
        case '^' :
        {
            row--;
            index = row * MAP_WIDTH + col;
            moveTo(m_pMap[index]);
        } break;
        case 'v' :
        {
            Sprite * pTile = Sprite::create("tile_empty.png");
            pTile->setTag('0');
            pTile->setPosition(m_pMap[index]->getPosition());
            pTile->setScale(m_pMap[index]->getScale());
            removeChild(m_pMap[index]);
            m_pMap[index] = pTile;
            addChild(pTile, 0);
            
            row++;
            index = row * MAP_WIDTH + col;
            moveTo(m_pMap[index]);
        } break;
        case 'V' :
        {
            row++;
            index = row * MAP_WIDTH + col;
            moveTo(m_pMap[index]);
        } break;
        case '>' :
        {
            col++;
            index = row * MAP_WIDTH + col;
            moveTo(m_pMap[index]);
        } break;
        case 'A' :
        {
            m_pMap[index]->runAction(RotateTo::create(0.3f, 0));
            m_pMap[index]->setTag('^');
            
            col--;
            index = row * MAP_WIDTH + col;
            moveTo(m_pMap[index]);
        } break;
        case '<' :
        {
            col--;
            index = row * MAP_WIDTH + col;
            moveTo(m_pMap[index]);
        } break;
        case 'X' :
        {
            switchScene(GameScene_MapPuzzle::scene());
        } break;
        default : m_bReady = true; break;
    }
}