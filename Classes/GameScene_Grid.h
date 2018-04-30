#ifndef __GAME_SCENE_2BALLS_H__
#define __GAME_SCENE_2BALLS_H__

#include "GameSceneBlocks.h"

class GameScene_Grid : public GameSceneBlocks
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Grid();
    ~GameScene_Grid();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Grid);
    
protected:
    
    // ===========================================================
    // GameScene methods
    // ===========================================================
    
    void prepareEnemies(cocos2d::CCLayer * pEnemies);
    void updateEnemies(cocos2d::CCLayer * pEnemies, float dt);
    void onRestart(cocos2d::CCNode * pSender);
    void enemySlowdown(cocos2d::CCLayer * pEnemies, bool slowdown);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    float m_fLastAddEnemyTime;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void addEnemy(cocos2d::CCLayer * pEnemies, int direction, float pos);
    void onMoveDone(cocos2d::CCNode * pSender);
    bool hasEnemyOnLine(cocos2d::CCLayer * pEnemies, float y);
};

#endif // __GAME_SCENE_2BALLS_H__
