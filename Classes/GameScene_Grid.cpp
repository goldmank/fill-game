#include "GameScene_Grid.h"

using namespace cocos2d;
using namespace CocosDenshion;

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Grid::GameScene_Grid()
{
}

GameScene_Grid::~GameScene_Grid()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Grid::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Grid *layer = GameScene_Grid::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// GameScene methods
// ===========================================================

void GameScene_Grid::prepareEnemies(cocos2d::CCLayer * pEnemies)
{
    float w = getScreenWidth();
    float h = getScreenHeight();
    onMoveDone(NULL);
    
    m_fLastAddEnemyTime = 0;
}

void GameScene_Grid::updateEnemies(cocos2d::CCLayer * pEnemies, float dt)
{
    m_fLastAddEnemyTime += dt;
    if (m_fLastAddEnemyTime > 0.5f && pEnemies->getChildrenCount() < 2)
    {
        m_fLastAddEnemyTime = 0;
        onMoveDone(NULL);
    }
}

void GameScene_Grid::onRestart(cocos2d::CCNode * pSender)
{
    switchScene(GameScene_Grid::scene());
}

void GameScene_Grid::enemySlowdown(cocos2d::CCLayer * pEnemies, bool slowdown)
{
    if (slowdown)
    {
        for (int i = 0; i < pEnemies->getChildrenCount(); i++)
        {
            Node * pChild = (Node *)pEnemies->getChildren().at(i);
            pChild->stopAllActions();
        }
    }
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Grid::addEnemy(cocos2d::CCLayer * pEnemies, int direction, float pos)
{
    Layer * pCells = getCells();
    
    float x = 0;
    float y = 0;
    float dx = 0;
    float dy = 0;
    switch (direction)
    {
        case 0 : // left
        case 1 : // right
        {
            CCNode * pCell = pCells->getChildren().at(0);
            y = pCell->getPositionY() + pCell->boundingBox().size.height;
            int offset = pos * (getRowsCount() - 1);
            y += offset * pCell->boundingBox().size.height * 1.1f;
            if (hasEnemyOnLine(pEnemies, y))
            {
                return;
            }
            
            if (0 == direction)
            {
                x = -pCell->boundingBox().size.width;
                dx = getScreenWidth() + pCell->boundingBox().size.width * 2;
            }
            else
            {
                x = getScreenWidth() + pCell->boundingBox().size.width / 2;
                dx = -(getScreenWidth() + pCell->boundingBox().size.width * 2);
            }
        } break;
    }
    
    Sprite * pEnemy = Sprite::create("rect.png");
    pEnemy->setScale(ENEMY_SIZE*2);
    pEnemy->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    pEnemy->setPosition(ccp(x, y));
    pEnemy->setRotation(45);
    
    pEnemies->addChild(pEnemy);
    
    CCMoveBy * pMove = CCMoveBy::create(3.0f, ccp(dx, dy));
    CCFiniteTimeAction* pMoveDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_Grid::onMoveDone));
    pEnemy->runAction(CCSequence::createWithTwoActions(pMove, pMoveDone));
}

void GameScene_Grid::onMoveDone(cocos2d::CCNode * pSender)
{
    if (NULL != pSender)
    {
        getEnemies()->removeChild(pSender);
    }
    addEnemy(getEnemies(), rand() % 2, (rand() % 100) / 100.0f);
}

bool GameScene_Grid::hasEnemyOnLine(cocos2d::CCLayer * pEnemies, float y)
{
    for (int i = 0; i < pEnemies->getChildrenCount(); i++)
    {
        Node * pChild = (Node *)pEnemies->getChildren().at(i);
        if ((int)pChild->getPositionY() == (int)y)
        {
            return true;
        }
    }
    return false;
}