#ifndef __GAME_SCENE_2BALLS_H__
#define __GAME_SCENE_2BALLS_H__

#include "GameSceneBlocks.h"

class GameScene_2Balls : public GameSceneBlocks
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_2Balls(int level);
    ~GameScene_2Balls();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene(int level);
    
    // implement the "static node()" method manually
    CREATE_FUNC_PARAM(GameScene_2Balls, int, level);
        
protected:
    
    // ===========================================================
    // GameScene methods
    // ===========================================================
    
    void prepareEnemies(cocos2d::CCLayer * pEnemies);
    void updateEnemies(cocos2d::CCLayer * pEnemies, float dt);
    void onRestart(cocos2d::CCNode * pSender);
    void enemySlowdown(cocos2d::CCLayer * pEnemies, bool slowdown);
    int getLevelNumber();
    
    void addEnemy(cocos2d::CCLayer * pEnemies, float x, float y, float size);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    int m_iLevel;
    int m_iSpeed;
    
    // ===========================================================
    // Private methods
    // ===========================================================
        
    void fixEnemiesSpeed();
};

#endif // __GAME_SCENE_2BALLS_H__
