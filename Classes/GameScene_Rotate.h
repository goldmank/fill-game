#ifndef __GAME_SCENE_ROTATE_H__
#define __GAME_SCENE_ROTATE_H__

#include "GameScene.h"

class GameScene_Rotate : public GameScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Rotate();
    ~GameScene_Rotate();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Rotate);
    
protected:
    
    // ===========================================================
    // GameScene methods
    // ===========================================================
    
    void prepareEnemies(cocos2d::CCLayer * pEnemies);
    void updateEnemies(cocos2d::CCLayer * pEnemies, float dt);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    b2Body * m_pBody;  
};

#endif // __GAME_SCENE_ROTATE_H__
