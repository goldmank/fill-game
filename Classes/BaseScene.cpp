#include "BaseScene.h"
#include "Pref.h"

using namespace cocos2d;
using namespace CocosDenshion;

// =========================================================== 
// Initialization & cleanup
// ===========================================================

BaseScene::BaseScene()
: m_iScreenWidth(0),
  m_iScreenHeight(0),
  m_fTime(0)
{
    m_pTouchListener = EventListenerTouchOneByOne::create();
    m_pTouchListener->setSwallowTouches(true);
    m_pTouchListener->onTouchBegan = CC_CALLBACK_2(BaseScene::onTouchBegan, this);
    m_pTouchListener->onTouchMoved = CC_CALLBACK_2(BaseScene::onTouchMoved, this);
    m_pTouchListener->onTouchEnded = CC_CALLBACK_2(BaseScene::onTouchEnded, this);
    m_pTouchListener->onTouchCancelled = CC_CALLBACK_2(BaseScene::onTouchCancelled, this);
    m_pTouchListener->retain();
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_pTouchListener, this);
}

BaseScene::~BaseScene()
{
    getEventDispatcher()->removeEventListener(m_pTouchListener);
    m_pTouchListener->release();
    m_pTouchListener = nullptr;
}

// ===========================================================
// CCLayer methods
// ===========================================================

bool BaseScene::init()
{
    Color3B c = getFillColor();
	if (!CCLayerColor::initWithColor(ccc4(c.r, c.g, c.b, 255)))
    {
        return false;
    }
	
	setTouchEnabled(true);
	schedule(schedule_selector(BaseScene::update));

    CCSize size = CCDirector::sharedDirector()->getWinSize();
	m_iScreenWidth = size.width;
	m_iScreenHeight = size.height;
    
	handleInit(size.width, size.height);
			
    return true;
}

// =========================================================== 
// Public methods
// ===========================================================

void BaseScene::update(float dt)
{
	m_fTime += dt;	
	handleUpdate(dt);
}

// =========================================================== 
// Common methods
// ===========================================================
	
void BaseScene::handleInit(float width, float height)
{
}

void BaseScene::handleUpdate(float dt)
{
}

// =========================================================== 
// Helper methods
// ===========================================================

int BaseScene::getScreenWidth()
{
	return m_iScreenWidth;
}

int BaseScene::getScreenHeight()
{
	return m_iScreenHeight;
}

bool BaseScene::isClicked(CCNode * pSprite, Touch * pTouch)
{
	if (NULL == pSprite)
	{
		return false;
	}
	CCSize size = pSprite->boundingBox().size;
	CCPoint point = pSprite->convertTouchToNodeSpace(pTouch);
	if (point.x > 0 && point.y > 0 && point.x < size.width && point.y < size.height)
	{
		return true;
	}
	return false;
}

float BaseScene::getTime()
{
	return m_fTime;
}

void BaseScene::switchScene(CCScene * pScene)
{
	CCTransitionScene *transition =  CCTransitionCrossFade::create(0.4f, pScene);
	CCDirector::sharedDirector()->replaceScene(transition);
}

cocos2d::ccColor3B BaseScene::getFillColor()
{
    return ccc3(255, 255, 255);
}

// ===========================================================
// Private methods
// ===========================================================

bool BaseScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return handleOnTouchBegan(touch, event);
}

void BaseScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
    handleOnTouchMoved(touch, event);
}

void BaseScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    handleOnTouchEnded(touch, event);
}

void BaseScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
    handleOnTouchCancelled(touch, event);
}