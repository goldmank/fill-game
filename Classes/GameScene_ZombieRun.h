#ifndef __GAME_SCENE_ZR_H__
#define __GAME_SCENE_ZR_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_ZombieRun : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_ZombieRun();
    ~GameScene_ZombieRun();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_ZombieRun);
    
protected:
    
    // ===========================================================
    // BaseScene methods
    // ===========================================================
    
    void handleInit(float width, float height);
    void handleUpdate(float dt);
    
    bool handleOnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    b2World * m_pWorld;
    float m_fLastAddedMonster;
    cocos2d::Vec2 m_touchPos;
    cocos2d::Vec2 m_touchStart;
    float m_fTouchStartTime;
    bool m_bTouchDown;
    bool m_bTouchStart;
    b2Body * m_pDraggedMonster;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void updateWorld(float dt);
    void addMonster();
};

#endif // __GAME_SCENE_ZR_H__
