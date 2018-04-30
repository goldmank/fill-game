#ifndef __GAME_SCENE_MAPPUZZLE_H__
#define __GAME_SCENE_MAPPUZZLE_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

#define MAP_WIDTH 5
#define MAP_HEIGHT 7

class GameScene_MapPuzzle : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_MapPuzzle();
    ~GameScene_MapPuzzle();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_MapPuzzle);
    
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
    
    cocos2d::Sprite * m_pMap[MAP_WIDTH * MAP_HEIGHT];
    cocos2d::Sprite * m_pPlayer;
    cocos2d::Sprite * m_pTouchedCell;
    bool m_bReady;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildMap();
    void moveTo(cocos2d::Sprite * pCell);
    void onMoveDone(cocos2d::Node * pPlayer);
};

#endif // __GAME_SCENE_MAPPUZZLE_H__
