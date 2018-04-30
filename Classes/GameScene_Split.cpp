#include "GameScene_Split.h"
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

GameScene_Split::GameScene_Split()
{
    srand(time(NULL));
    m_pTouchedTile = NULL;
}

GameScene_Split::~GameScene_Split()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Split::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Split *layer = GameScene_Split::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_Split::handleInit(float screenWidth, float screenHeight)
{
    const char * batches[] = { "hextiles", NULL };
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
}

void GameScene_Split::handleUpdate(float dt)
{
}

bool GameScene_Split::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    m_pTouchedTile = NULL;
    for (int i = m_pTiles->getChildrenCount() - 1; NULL == m_pTouchedTile && i >= 0; i--)
    {
        Node * pTile = m_pTiles->getChildren().at(i);
        if (isClicked(pTile, pTouch))
        {
            m_pTouchedTile = pTile;
            m_pTouchedTile->setColor(ccc3(255, 0, 0));
        }
    }
    return true;
}

void GameScene_Split::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedTile && !isClicked(m_pTouchedTile, pTouch))
    {
        m_pTouchedTile->setColor(ccc3(255, 255, 255));
        m_pTouchedTile = NULL;
    }
}

void GameScene_Split::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    if (NULL != m_pTouchedTile)
    {
        m_pTouchedTile->setColor(ccc3(255, 255, 255));
        
        if (isClicked(m_pTouchedTile, pTouch))
        {
            int tileIndex = -1;
            for (int i = 0; -1 == tileIndex && i < m_pTiles->getChildrenCount(); i++)
            {
                if (m_pTiles->getChildren().at(i) == m_pTouchedTile)
                {
                    tileIndex = i;
                }
            }
            
            if (-1 != tileIndex)
            {
                int playerTile = m_pPlayer->getTag();
                
                Node * pPlayerTile = m_pTiles->getChildren().at(playerTile);
                float dx = pPlayerTile->getPositionX() - m_pTouchedTile->getPositionX();
                float dy = pPlayerTile->getPositionY() - m_pTouchedTile->getPositionY();
                float d = sqrtf(dx*dx + dy*dy);
                if (d <= m_pTouchedTile->boundingBox().size.width)
                {
                    int tileType = m_pTouchedTile->getTag();
                    if ('f' == tileType)
                    {
                        Sprite * pFollower = (Sprite *)m_pTouchedTile->getUserData();
                        m_pTouchedTile->setUserData(NULL);
                        m_pTouchedTile->setTag('0');
                        
                        pFollower->setDisplayFrame(SpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("alienBlue.png"));
                        pFollower->setPositionY(m_pTouchedTile->getPositionY() + pFollower->boundingBox().size.height / 2);
                        m_followers.push_back(pFollower);
                        
                        moveEnemies();
                    }
                    else if (!isFollowerOnTile(tileIndex))
                    {
                        if (0 == d)
                        {
                            if (m_pTouchedTile->getTag() == '3' && !m_followers.empty())
                            {
                                moveFollowers();
                                moveEnemies();
                            }
                        }
                        else
                        {
                            moveFollowers();
                            moveToTile(m_pPlayer, m_pTouchedTile, tileIndex);
                            moveEnemies();
                        }
                    }
                    
                    CCFiniteTimeAction* pAnimDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_Split::onMoveDone));
                    runAction(Sequence::createWithTwoActions(DelayTime::create(0.25f), pAnimDone));
                }
            }
        }
        
        m_pTouchedTile = NULL;
    }
}

void GameScene_Split::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Split::buildWorld()
{
    m_pTiles = Layer::create();
    addChild(m_pTiles);
    
    char data[] = {
        '2', '2', 'f', '0', '0',
        '2', '0', '0', '0', '0',
        '0', 'e', '1', '1', '1',
        '0', '0', '0', '0', '1',
        '0', 'e', '0', '0', '3'
    };
    
    Sprite * pTemp = Sprite::createWithSpriteFrameName("tileDirt.png");
    float tileWidth = pTemp->boundingBox().size.width;
    float tileHeight = pTemp->boundingBox().size.height;
    float y = getScreenHeight() * 0.5f + tileHeight * 1.5f;
    for (int i = 0; i < 5; i++)
    {
        float x = getScreenWidth() * 0.5f - tileWidth * 1.75f;
        if (i % 2 == 0)
        {
            x -= tileWidth / 2;
        }
        for (int j = 0; j < 5; j++)
        {
            Sprite * pTile = NULL;
            
            int tileIndex = i * 5 + j;
            char type = data[tileIndex];
            switch (type)
            {
                case 'z' :
                case 'f' :
                case 'e' :
                case '0' : pTile = Sprite::createWithSpriteFrameName("tileDirt.png"); break;
                case '1' : pTile = Sprite::createWithSpriteFrameName("tileGrass.png"); break;
                case '2' : pTile = Sprite::createWithSpriteFrameName("tileSand.png"); break;
                case '3' : pTile = Sprite::createWithSpriteFrameName("tileWater.png"); break;
            }
            pTile->setPosition(x, y);
            pTile->setTag(type);
            m_pTiles->addChild(pTile);
            
            if ('z' == type)
            {
                Sprite * pTree = Sprite::createWithSpriteFrameName("treeCactus_3.png");
                pTree->setPosition(pTile->getPositionX(), pTile->getPositionY() + pTree->boundingBox().size.height / 2);
                addChild(pTree, i);
            }
            else if ('f' == type)
            {
                Sprite * pFollower = Sprite::createWithSpriteFrameName("alienYellow.png");
                pFollower->setTag(tileIndex);
                pFollower->setPosition(pTile->getPositionX(), pTile->getPositionY() + pTile->boundingBox().size.height * 0.0f + pFollower->boundingBox().size.height / 2);
                addChild(pFollower, i);
                
                pTile->setUserData(pFollower);
            }
            else if ('e' == type)
            {
                Sprite * pEnemy = Sprite::createWithSpriteFrameName("alienBeige.png");
                pEnemy->setTag(tileIndex);
                pEnemy->setPosition(pTile->getPositionX(), pTile->getPositionY() + pTile->boundingBox().size.height * 0.0f + pEnemy->boundingBox().size.height / 2);
                pEnemy->setUserData(buildEnemyPattern(rand()%2));
                addChild(pEnemy);
                
                pEnemy->setZOrder(tileIndex / 5);
                
                m_enemies.push_back(pEnemy);
            }
            
            x += tileWidth;
        }
        y -= tileHeight * 0.56f;
    }
}

void GameScene_Split::buildPlayer()
{
    int tileIndex = -1;
    for (int i = 0; -1 == tileIndex && i < m_pTiles->getChildrenCount(); i++)
    {
        if (m_pTiles->getChildren().at(i)->getTag() == '3')
        {
            tileIndex = i;
        }
    }
    Node * pTile = m_pTiles->getChildren().at(tileIndex);
    
    m_pPlayer = Sprite::createWithSpriteFrameName("alienBlue.png");
    m_pPlayer->setTag(tileIndex);
    m_pPlayer->setPosition(pTile->getPositionX(), pTile->getPositionY() + pTile->boundingBox().size.height * 0.0f + m_pPlayer->boundingBox().size.height / 2);
    addChild(m_pPlayer);
    
    m_pPlayer->setZOrder(tileIndex / 5);
}

void GameScene_Split::moveEnemies()
{
    for (int i = 0; i < m_enemies.size(); i++)
    {
        Sprite * pEnemy = m_enemies[i];
        int newTile = getEnemyMoveTile(pEnemy);
        
        Node * pTile = (Node *)m_pTiles->getChildren().at(newTile);
        moveToTile(pEnemy, pTile, newTile);
    }
}

int GameScene_Split::getEnemyMoveTile(Sprite * pEnemy)
{
    int enemyTile = pEnemy->getTag();
    int enemyLine = enemyTile / 5;
    int enemyCol = enemyTile - (enemyLine * 5);
    
    EnemyPattern * pPattern = (EnemyPattern *)pEnemy->getUserData();
    int step = pPattern->steps[pPattern->currIndex];
    switch (step)
    {
        case 0 : enemyCol++; break;
        case 1 : enemyCol--; break;
        case 2 : enemyLine++; break;
        case 3 : enemyLine--; break;
    }
    pPattern->currIndex = (pPattern->currIndex + 1) % pPattern->steps.size();
    if (canEnemyMoveTo(enemyCol, enemyLine))
    {
        return enemyLine * 5 + enemyCol;
    }
    return pEnemy->getTag();
}

void GameScene_Split::moveToTile(cocos2d::Sprite * pCharecter, cocos2d::Node * pTile, int tileIndex)
{
    pCharecter->setTag(tileIndex);
    pCharecter->stopAllActions();
    
    float toX = pTile->getPositionX();
    float toY = pTile->getPositionY() + pCharecter->boundingBox().size.height / 2;
    pCharecter->runAction(MoveTo::create(0.25f, Vec2(toX, toY)));
    
    pCharecter->setZOrder(tileIndex / 5);
}

void GameScene_Split::checkCollisions()
{
    std::vector<int> myTiles;
    myTiles.push_back(m_pPlayer->getTag());
    for (int i = 0; i < m_followers.size(); i++)
    {
        myTiles.push_back(m_followers[i]->getTag());
    }
    
    for (int i = 0; i < m_enemies.size(); i++)
    {
        Sprite * pEnemy = m_enemies[i];
        int enemyTile = pEnemy->getTag();
        for (int j = 0; j < myTiles.size(); j++)
        {
            if (myTiles[j] == enemyTile)
            {
                switchScene(GameScene_Split::scene());
                return;
            }
        }
    }
}

void GameScene_Split::onMoveDone(cocos2d::Node * pSender)
{
    checkCollisions();
    
    int safeTile = -1;
    for (int i = 0; -1 == safeTile && i < m_pTiles->getChildrenCount(); i++)
    {
        if (m_pTiles->getChildren().at(i)->getTag() == '3')
        {
            safeTile = i;
        }
    }
    
    std::vector<cocos2d::Sprite *>::iterator iter = m_followers.begin();
    while (m_followers.end() != iter)
    {
        Sprite * pFollower = *iter;
        if (pFollower->getTag() == safeTile)
        {
            removeChild(pFollower);
            m_followers.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

bool GameScene_Split::isFollowerOnTile(int tileIndex)
{
    for (int i = 0; i < m_followers.size(); i++)
    {
        if (tileIndex == m_followers[i]->getTag())
        {
            return true;
        }
    }
    return false;
}

void GameScene_Split::moveFollowers()
{
    if (m_followers.empty())
    {
        return;
    }
    
    for (int i = (int)m_followers.size() - 1; i >= 1; i--)
    {
        Sprite * pFollower = m_followers[i];
        Sprite * pTo = m_followers[i - 1];
        
        Node * pTile = m_pTiles->getChildren().at(pTo->getTag());
        moveToTile(pFollower, pTile, pTo->getTag());
    }
    
    Sprite * pFollower = m_followers[0];
    Sprite * pTo = m_pPlayer;
    
    Node * pTile = m_pTiles->getChildren().at(pTo->getTag());
    moveToTile(pFollower, pTile, pTo->getTag());
}

int GameScene_Split::getMapTileType(int x, int y)
{
    return m_pTiles->getChildren().at(y * 5 + x)->getTag();
}

bool GameScene_Split::canEnemyMoveTo(int x, int y)
{
    if (x < 0)
    {
        return false;
    }
    if (y < 0)
    {
        return false;
    }
    if (x >= 5)
    {
        return false;
    }
    if (y >= 5)
    {
        return false;
    }
    int tile = m_pTiles->getChildren().at(y * 5 + x)->getTag();
    if ('3' == tile || 'f' == tile)
    {
        return false;
    }
    return true;
}

GameScene_Split::EnemyPattern * GameScene_Split::buildEnemyPattern(int type)
{
    EnemyPattern * retVal = new EnemyPattern();
    retVal->currIndex = 0;
    
//    for (int i = 0; i < 10; i++)
//    {
//        retVal->steps.push_back(rand()%4);
//    }

    if (0 == type)
    {
        retVal->steps.push_back(0);
        retVal->steps.push_back(0);
        retVal->steps.push_back(0);
        retVal->steps.push_back(1);
        retVal->steps.push_back(1);
        retVal->steps.push_back(1);
    }
    else
    {
        retVal->steps.push_back(2);
        retVal->steps.push_back(2);
        retVal->steps.push_back(2);
        retVal->steps.push_back(3);
        retVal->steps.push_back(3);
        retVal->steps.push_back(3);
    }
    
    return retVal;
}