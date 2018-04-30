#include "GameScene_FightLine.h"
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

GameScene_FightLine::GameScene_FightLine()
{
    srand(time(NULL));
    m_pTouchedItem = NULL;
}

GameScene_FightLine::~GameScene_FightLine()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_FightLine::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_FightLine *layer = GameScene_FightLine::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// BaseScene methods
// ===========================================================

void GameScene_FightLine::handleInit(float screenWidth, float screenHeight)
{
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
 
    buildUI();
}

void GameScene_FightLine::handleUpdate(float dt)
{
}

bool GameScene_FightLine::handleOnTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
    return true;
}

void GameScene_FightLine::handleOnTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    
}

void GameScene_FightLine::handleOnTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
}

void GameScene_FightLine::handleOnTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* event)
{
    handleOnTouchEnded(pTouch, event);
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_FightLine::buildUI()
{
    Sprite * pAction = Sprite::createWithSpriteFrameName("action_magic.png");
    float x = getScreenWidth() * 0.5 - pAction->boundingBox().size.width * 1.3f * (INVENTORY_COUNT-1) * 0.5f;
    float y = 0;
    float oppY = getScreenHeight();
    
    const char * actions[] = { "action_warrior.png", "action_shield.png", "action_magic.png", "action_temp.png" };
    for (int i = 0; i < INVENTORY_COUNT; i++)
    {
        m_pInventory[i] = buildActionBtn(x, y + pAction->boundingBox().size.height * 0.8f, actions[i], i, 5);
        m_pOppInventory[i] = buildActionBtn(x, oppY - pAction->boundingBox().size.height * 0.8f, actions[i], i, 5);
        
        x += pAction->boundingBox().size.width * 1.3f;
    }
    
    Sprite * pBase = Sprite::createWithSpriteFrameName("base.png");
    x = getScreenWidth() * 0.5 - pBase->boundingBox().size.width * 1.9f * (BASE_COUNT-1) * 0.5f;
    y = getScreenHeight() * 0.1f;
    oppY = getScreenHeight() * 0.9f;
    for (int i = 0; i < BASE_COUNT; i++)
    {
        m_pBases[i] = Sprite::createWithSpriteFrameName("base.png");
        m_pBases[i]->setPosition(x, y + m_pBases[i]->boundingBox().size.height * 0.8f);
        addChild(m_pBases[i]);
        
        m_pOppBases[i] = Sprite::createWithSpriteFrameName("base.png");
        m_pOppBases[i]->setPosition(x, oppY - m_pOppBases[i]->boundingBox().size.height * 0.8f);
        addChild(m_pOppBases[i]);
        
        x += m_pBases[i]->boundingBox().size.width * 1.9f;
    }
}

Sprite * GameScene_FightLine::buildActionBtn(float x, float y, const char * btn, int type, int count)
{
    Sprite * pBtn = Sprite::createWithSpriteFrameName(btn);
    pBtn->setPosition(x, y);
    pBtn->setTag(type);
    addChild(pBtn);
    
    Sprite * pTag = Sprite::createWithSpriteFrameName("tag.png");
    pTag->setPosition(pBtn->boundingBox().size.width - pTag->boundingBox().size.width / 2, pBtn->boundingBox().size.height - pTag->boundingBox().size.height/ 2);
    pTag->setTag(count);
    pBtn->addChild(pTag);
    
    return pBtn;
}

void GameScene_FightLine::selectAction(cocos2d::Sprite * pAction)
{
    
}

void GameScene_FightLine::releaseFromBase(cocos2d::Sprite * pBase)
{
    
}