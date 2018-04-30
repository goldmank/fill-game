#ifndef __GAME_SCENE_SWIPE_TO_CIRCLE_H__
#define __GAME_SCENE_SWIPE_TO_CIRCLE_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_SwipeToCircle : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_SwipeToCircle();
    ~GameScene_SwipeToCircle();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_SwipeToCircle);
    
protected:
    
    // ===========================================================
    // BaseScene methods
    // ===========================================================
    
    void handleInit(float width, float height);
    void handleUpdate(float dt);
    
    cocos2d::ccColor3B getFillColor() { return Pref::getInstance()->getColor(COLOR_BG); }
    
    bool handleOnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
    
private:
    
    // ===========================================================
    // CircleData
    // ===========================================================
    
    struct CircleData
    {
        float m_fAngle;
        float m_fRadius;
        bool m_bMovingIn;
        bool m_bHitCenter;
        float m_fAddAngle;
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    bool m_bDone;
    cocos2d::Sprite * m_pCenter;
    cocos2d::Node * m_pTouchedCircle;
    float m_fColorCenterTime;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void onGameOver();
    void restartGame(cocos2d::Node * pSender);
    void colorCenter(cocos2d::Color3B color);
};

#endif // __GAME_SCENE_SWIPE_TO_CIRCLE_H__
