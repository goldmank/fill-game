#ifndef __GAME_SCENE_PIXEL_RUN_H__
#define __GAME_SCENE_PIXEL_RUN_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_PixelRun : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_PixelRun();
    ~GameScene_PixelRun();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_PixelRun);
    
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
    cocos2d::Layer * m_pEnemies;
    cocos2d::Sprite * m_pRoads[2];
    cocos2d::Vec2 m_roadStartPos[2];
    cocos2d::Sprite * m_pPlayer;
    float m_fAddEnemyCounter;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildRoads();
    void moveRoads(float dt);
    void moveEnemies(float dt);
    void addPlayer();
    void updatePlayerRoad(int road);
    void addEnemy(int road);
    void checkCollisions();
    void checkEnemyExit();
    void onGameOver();
    void restartGame(cocos2d::Node * pSender);
};

#endif // __GAME_SCENE_PIXEL_RUN_H__
