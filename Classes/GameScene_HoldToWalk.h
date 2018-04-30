#ifndef __GAME_SCENE_HOLD_TO_WALK_H__
#define __GAME_SCENE_HOLD_TO_WALK_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_HoldToWalk : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_HoldToWalk();
    ~GameScene_HoldToWalk();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_HoldToWalk);
    
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
    // Fields
    // ===========================================================
    
    bool m_bDone;
    cocos2d::Layer * m_pGameLayer;
    cocos2d::Sprite * m_pPlayer;
    float m_fMoveSpeed;
    bool m_bMove;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void onGameOver();
    void restartGame(cocos2d::Node * pSender);
    void addEnemy(float x, float y);
    void createPlayer();
};

#endif // __GAME_SCENE_HOLD_TO_WALK_H__
