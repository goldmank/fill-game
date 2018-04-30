#include "GameScene_Circle.h"

using namespace cocos2d;
using namespace CocosDenshion;

// ===========================================================
// Initialization & cleanup
// ===========================================================

GameScene_Circle::GameScene_Circle()
{
    m_pCircle = NULL;
}

GameScene_Circle::~GameScene_Circle()
{
}

// ===========================================================
// CCLayer methods
// ===========================================================

CCScene* GameScene_Circle::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene_Circle *layer = GameScene_Circle::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// ===========================================================
// GameScene methods
// ===========================================================

void GameScene_Circle::handleCellTouchStart(cocos2d::Touch* pTouch)
{
    startCircle(pTouch->getLocation());
}

void GameScene_Circle::handleCellTouchMove(cocos2d::Touch* pTouch)
{
    
}

void GameScene_Circle::handleCellTouchEnd(cocos2d::Touch* pTouch)
{
    if (NULL != m_pCircle)
    {
        m_pCircle->stopAllActions();
        if (m_pCircle->getScale() < 0.09f)
        {
            m_pCircle->setScale(0.09f);
        }
        
        float d = 1.0f / FILL_SPEED;
        CCNode * pFiller = (CCNode *)m_pCircle->getChildren().at(0);
        CCScaleTo * pScaleIn = CCScaleTo::create(d / 2, 0.85f);
        CCScaleTo * pScaleOut = CCScaleTo::create(d / 2, 1.0f);
        CCSequence * pSeq = CCSequence::createWithTwoActions(pScaleIn, pScaleOut);
        CCFiniteTimeAction* pFillDone = CCCallFuncN::create(this, callfuncN_selector(GameScene_Circle::onFillDone));
        pFiller->runAction(CCSequence::createWithTwoActions(CCEaseSineInOut::create(pSeq), pFillDone));
        
        m_pCircle = NULL;
    }
}

// ===========================================================
// Private methods
// ===========================================================

void GameScene_Circle::startCircle(cocos2d::Vec2 pos)
{
    m_pCircle = Sprite::create("circle.png");
    m_pCircle->setColor(Pref::getInstance()->getColor(COLOR_ENEMY));
    m_pCircle->setPosition(pos);
    addChild(m_pCircle);
    
    Sprite * pInner = Sprite::create("circle.png");
    pInner->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pInner->setScale(0.9f);
    pInner->setPosition(ccp(m_pCircle->boundingBox().size.width / 2, m_pCircle->boundingBox().size.height / 2));
    m_pCircle->addChild(pInner);
    
    m_pCircle->setScale(0);
    m_pCircle->runAction(ScaleTo::create(4.0f, 5.0f));
}

void GameScene_Circle::onFillDone(cocos2d::CCNode * pSender)
{
    CCNode * pCircle = (CCNode *)pSender->getParent();
    pCircle->setColor(Pref::getInstance()->getColor(COLOR_FILL));
    pCircle->removeAllChildren();
    
//    b2BodyDef bodyDef;
//    bodyDef.type = b2_kinematicBody;
//    bodyDef.userData = pCircle;
//    bodyDef.position.Set(pCircle->getPositionX()/PTM_RATIO, pCircle->getPositionY()/PTM_RATIO);
//    b2Body * pBody = getWorld()->CreateBody(&bodyDef);
//    
//    b2CircleShape shape;
//    shape.m_radius = (pCircle->boundingBox().size.width/2)/PTM_RATIO;
//    
//    b2FixtureDef shapeDef;
//    shapeDef.shape = &shape;
//    shapeDef.density = 1.0f;
//    shapeDef.friction = 0.0f;
//    shapeDef.restitution = 1.0f;
//    pBody->CreateFixture(&shapeDef);
}