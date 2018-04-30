#include "GameScene_Map2PlayersFight.h"
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

GameScene_Map2PlayersFight::GameScene_Map2PlayersFight()
{
    srand(time(NULL));
    m_bReady = true;
    m_bOppTurn = false;
}

GameScene_Map2PlayersFight::~GameScene_Map2PlayersFight()
{
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Map2PlayersFight::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Map2PlayersFight *layer = GameScene_Map2PlayersFight::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Map2PlayersFight::handleInit(float screenWidth, float screenHeight)
{
    m_pPlayer = Sprite::create("grid_game_player.png");
    m_pOpponent = Sprite::create("grid_game_opp.png");
    
    buildMap();
    
    m_pPlayer->setPosition(m_pMap[0]->getPosition());
    m_pPlayer->setTag(0);
    m_pOpponent->setPosition(m_pMap[MAP_WIDTH * MAP_HEIGHT - 1]->getPosition());
    m_pOpponent->setTag(MAP_WIDTH * MAP_HEIGHT - 1);
    addChild(m_pPlayer);
    addChild(m_pOpponent);
    
//    int * pMap = new int[MAP_WIDTH * MAP_HEIGHT];
//    memset(pMap, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
//    pMap[0] = 1;
//    pMap[MAP_WIDTH * MAP_HEIGHT - 1] = 2;
//    std::vector<int> steps;
//    std::vector<int> oppSteps;
//    test(pMap, 0, MAP_WIDTH * MAP_HEIGHT - 1, steps, oppSteps);
//    
//    CCLOG("games: %d", (int)m_games.size());
}

void GameScene_Map2PlayersFight::handleUpdate(float dt)
{
    
}

bool GameScene_Map2PlayersFight::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pTouchedCell = NULL;
    
    if (m_bReady)
    {
        int playerIndex = m_pPlayer->getTag();
        if (m_bOppTurn)
        {
            playerIndex = m_pOpponent->getTag();
        }
        int playerRow = playerIndex / MAP_WIDTH;
        int playerCol = playerIndex % MAP_WIDTH;
        
        int index = -1;
        for (int i = 0; -1 == index && i < MAP_WIDTH * MAP_HEIGHT; i++)
        {
            if (isClicked(m_pMap[i], pTouch))
            {
                index = i;
            }
        }
        
        if (-1 != index)
        {
            int touchLine = index / MAP_WIDTH;
            int touchCol = index % MAP_WIDTH;
            if (touchLine == playerRow)
            {
                index = canGoToCellInCol(playerIndex, index);
                if (index != playerIndex)
                {
                    m_pTouchedCell = m_pMap[index];
                    m_pTouchedCell->setColor(Color3B(200, 200, 200));
                }
            }
            else if (touchCol == playerCol)
            {
                index = canGoToCellInRow(playerIndex, index);
                if (index != playerIndex)
                {
                    m_pTouchedCell = m_pMap[index];
                    m_pTouchedCell->setColor(Color3B(200, 200, 200));
                }
            }
        }
    }
    
    return true;
}

void GameScene_Map2PlayersFight::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedCell && !isClicked(m_pTouchedCell, pTouch))
    {
        m_pTouchedCell->setColor(Color3B(255, 255, 255));
    }
}

void GameScene_Map2PlayersFight::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedCell)
    {
        m_pTouchedCell->setColor(Color3B(255, 255, 255));
        
        if ('B' == m_pTouchedCell->getTag())
        {
            m_pTouchedCell->setColor(Color3B(160, 160, 160));
        }
        
        if (m_bOppTurn)
        {
            moveTo(m_pOpponent, m_pTouchedCell);
        }
        else
        {
            moveTo(m_pPlayer, m_pTouchedCell);
        }
        
        m_pTouchedCell = NULL;
    }
}

void GameScene_Map2PlayersFight::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Map2PlayersFight::buildMap()
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
            int tag = 'Z';
            
            Sprite * pTile = Sprite::create("tile_empty.png");
            
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
}

void GameScene_Map2PlayersFight::moveTo(cocos2d::Sprite * pPlayer, cocos2d::Sprite * pCell)
{
    if (!m_bReady)
    {
        return;
    }
    if ('B' == pCell->getTag())
    {
        return;
    }
    m_bReady = false;
    MoveTo * pMoveTo = MoveTo::create(0.2f, pCell->getPosition());
    CCFiniteTimeAction* pMoveDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_Map2PlayersFight::onMoveDone));
    pPlayer->stopAllActions();
    pPlayer->runAction(Sequence::createWithTwoActions(pMoveTo, pMoveDone));
    
    int playerIndex = pPlayer->getTag();
    switch (m_pMap[playerIndex]->getTag())
    {
        case 'Z' :
        {
            if (pPlayer == m_pPlayer)
            {
                m_pMap[playerIndex]->setColor(Color3B(255, 160, 160));
            }
            else
            {
                m_pMap[playerIndex]->setColor(Color3B(160, 160, 255));
            }
            m_pMap[playerIndex]->setTag('B');
        } break;
    }
}

void GameScene_Map2PlayersFight::onMoveDone(cocos2d::Node * pPlayer)
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
    
    pPlayer->setTag(index);
    int row = index / MAP_WIDTH;
    int col = index % MAP_WIDTH;
    
    m_bReady = true;
    m_bOppTurn = !m_bOppTurn;
    if (m_bOppTurn)
    {
        doOppStep();
    }
}

void GameScene_Map2PlayersFight::doOppStep()
{
    int index = m_pOpponent->getTag();
    Cell * pCell = (Cell *)m_pMap[index]->getUserData();
    
    int * pMap = new int[MAP_WIDTH * MAP_HEIGHT];
    memset(pMap, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
    pMap[index] = 1;
    pMap[m_pPlayer->getTag()] = 1;
    for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
    {
        if ('B' == m_pMap[i]->getTag())
        {
            pMap[i] = 1;
        }
    }
    printMap(pMap);
    std::vector<int> possMoves = getPossibleMoves(pMap, index);
    delete pMap;
    
    if (possMoves.empty())
    {
        switchScene(GameScene_Map2PlayersFight::scene());
    }
    else
    {
        int r = rand() % possMoves.size();
        int randMoveIndex = possMoves[r];
        Sprite * pTile = m_pMap[randMoveIndex];
        moveTo(m_pOpponent, pTile);
    }
    
//    int count = 0;
//    for (int i = 0; i < pCell->m_neighbours.size(); i++)
//    {
//        Cell * pNextCell = pCell->m_neighbours[i];
//        if ('B' == pNextCell->m_pTile->getTag())
//        {
//            count++;
//        }
//    }
//    if (count == pCell->m_neighbours.size())
//    {
//        switchScene(GameScene_Map2PlayersFight::scene());
//        return;
//    }
//    
//    int r = rand() % (int)pCell->m_neighbours.size();
//    Cell * pNextCell = pCell->m_neighbours[r];
//    while ('B' == pNextCell->m_pTile->getTag())
//    {
//        r = rand() % (int)pCell->m_neighbours.size();
//        pNextCell = pCell->m_neighbours[r];
//    }
//    
//    moveTo(m_pOpponent, pNextCell->m_pTile);
}

int GameScene_Map2PlayersFight::canGoToCellInRow(int from, int to)
{
    Sprite * pOtherPlayer = m_pOpponent;
    if (m_bOppTurn)
    {
        pOtherPlayer = m_pPlayer;
    }
    int otherIndex = pOtherPlayer->getTag();
    
    if (from < to)
    {
        int index = from + MAP_WIDTH;
        while (index <= to)
        {
            if ('B' == m_pMap[index]->getTag())
            {
                return index - MAP_WIDTH;
            }
            index += MAP_WIDTH;
        }
        return to;
    }
    else
    {
        int index = from - MAP_WIDTH;
        while (index >= to)
        {
            if ('B' == m_pMap[index]->getTag())
            {
                return index + MAP_WIDTH;
            }
            index -= MAP_WIDTH;
        }
        return to;
    }
}

int GameScene_Map2PlayersFight::canGoToCellInCol(int from, int to)
{
    Sprite * pOtherPlayer = m_pOpponent;
    if (m_bOppTurn)
    {
        pOtherPlayer = m_pPlayer;
    }
    int otherIndex = pOtherPlayer->getTag();
    
    if (from < to)
    {
        int index = from + 1;
        while (index <= to)
        {
            if ('B' == m_pMap[index]->getTag())
            {
                return index - 1;
            }
            index += 1;
        }
        return to;
    }
    else
    {
        int index = from - 1;
        while (index >= to)
        {
            if ('B' == m_pMap[index]->getTag())
            {
                return index + 1;
            }
            index -= 1;
        }
        return to;
    }
}

void GameScene_Map2PlayersFight::test(int * pMap, int playerIndex, int oppIndex, std::vector<int> stepsPlayer, std::vector<int> stepsOpp)
{
    std::vector<int> possMoves = getPossibleMoves(pMap, playerIndex);
    if (possMoves.empty())
    {
        std::vector<int> possOppMoves = getPossibleMoves(pMap, oppIndex);
        if (possOppMoves.empty())
        {
            Game g;
            g.m_moves = stepsPlayer;
            g.m_oppMoves = stepsOpp;
            m_games.push_back(g);
            delete pMap;
        }
        else
        {
            Game g;
            g.m_moves = stepsPlayer;
            g.m_oppMoves = stepsOpp;
            g.m_oppMoves.push_back(possOppMoves[0]);
            m_games.push_back(g);
            delete pMap;
        }
    }
    else
    {
        int * pMapCopy = copyMap(pMap);
        
        for (int i = 0; i < possMoves.size(); i++)
        {
            int destIndex = possMoves[i];
            
            int * pStepMap = copyMap(pMapCopy);
            pStepMap[destIndex] = 1;
            
            std::vector<int> newStepsPlayer;
            for (int i = 0; i < stepsPlayer.size(); i++)
            {
                newStepsPlayer.push_back(stepsPlayer[i]);
            }
            newStepsPlayer.push_back(destIndex);
            
            std::vector<int> possOppMoves = getPossibleMoves(pStepMap, oppIndex);
            if (possOppMoves.empty())
            {
                Game g;
                g.m_moves = newStepsPlayer;
                g.m_oppMoves = stepsOpp;
                m_games.push_back(g);
                delete pStepMap;
            }
            else
            {
                for (int j = 0; j < possOppMoves.size(); j++)
                {
                    int oppDestIndex = possOppMoves[j];
                    
                    int * pOppStepMap = copyMap(pStepMap);
                    pOppStepMap[oppDestIndex] = 2;
                    
                    std::vector<int> newOppStepsPlayer;
                    for (int i = 0; i < stepsOpp.size(); i++)
                    {
                        newOppStepsPlayer.push_back(stepsOpp[i]);
                    }
                    newOppStepsPlayer.push_back(oppDestIndex);
                    
                    test(pOppStepMap, destIndex, oppDestIndex, newStepsPlayer, newOppStepsPlayer);
                }
                delete pStepMap;
            }
        }
        
        delete pMapCopy;
    }
    //delete pMap;
}

int * GameScene_Map2PlayersFight::copyMap(int * pOther)
{
    int * pRetVal = new int[MAP_WIDTH * MAP_HEIGHT];
    memcpy(pRetVal, pOther, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
    return pRetVal;
}

void GameScene_Map2PlayersFight::printMap(int * pMap)
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        char buffer[MAP_WIDTH + 1];
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            buffer[j] = (char)(pMap[i * MAP_WIDTH + j] + 48);
        }
        buffer[MAP_WIDTH] = '\0';
        CCLOG("%s", buffer);
    }
}

std::vector<int> GameScene_Map2PlayersFight::getPossibleMoves(int * pMap, int index)
{
    std::vector<int> retVal;
    
    int row = index / MAP_WIDTH;
    int col = index % MAP_WIDTH;
    
    for (int i = col - 1; i >= 0; i--)
    {
        int index = row * MAP_WIDTH + i;
        if (0 == pMap[index])
        {
            retVal.push_back(index);
        }
        else
        {
            break;
        }
    }
    
    for (int i = col + 1; i < MAP_WIDTH; i++)
    {
        int index = row * MAP_WIDTH + i;
        if (0 == pMap[index])
        {
            retVal.push_back(index);
        }
        else
        {
            break;
        }
    }
    
    for (int i = row - 1; i >= 0; i--)
    {
        int index = i * MAP_WIDTH + col;
        if (0 == pMap[index])
        {
            retVal.push_back(index);
        }
        else
        {
            break;
        }
    }
    
    for (int i = row + 1; i < MAP_HEIGHT; i++)
    {
        int index = i * MAP_WIDTH + col;
        if (0 == pMap[index])
        {
            retVal.push_back(index);
        }
        else
        {
            break;
        }
    }
    
    return retVal;
}

void GameScene_Map2PlayersFight::play(std::vector<int> stepsPlayer, std::vector<int> stepsOpp)
{
    int * pMap = new int[MAP_WIDTH * MAP_HEIGHT];
    memset(pMap, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
    pMap[0] = 1;
    pMap[MAP_WIDTH * MAP_HEIGHT - 1] = 2;
    
    for (int i = 0; i < min(stepsPlayer.size(), stepsOpp.size()); i++)
    {
        int player = stepsPlayer[i];
        int opp = stepsOpp[i];
        pMap[player] = 1;
        pMap[opp] = 2;
        
        std::vector<int> poss = getPossibleMoves(pMap, opp);
        
        CCLOG("#%d",i);
        printMap(pMap);
    }
}