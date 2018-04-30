#ifndef __GAME_SCENE_SHADOW_H__
#define __GAME_SCENE_SHADOW_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_Shadow : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Shadow();
    ~GameScene_Shadow();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Shadow);
    
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
    cocos2d::CCRenderTexture * m_pCanvas;
    cocos2d::Sprite * m_pBg;
    cocos2d::Sprite * m_pCircle;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void onGameOver();
    void restartGame(cocos2d::Node * pSender);
};

#endif // __GAME_SCENE_SHADOW_H__
