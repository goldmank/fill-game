#ifndef __GAME_SCENE_SPLIT_H__
#define __GAME_SCENE_SPLIT_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_Split : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Split();
    ~GameScene_Split();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Split);
    
    // ===========================================================
    // Public methods
    // ===========================================================
    
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
    // EnemyPattern
    // ===========================================================
    
    struct EnemyPattern
    {
        std::vector<int> steps;
        int currIndex;
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    cocos2d::Layer * m_pTiles;
    cocos2d::Node * m_pTouchedTile;
    cocos2d::Sprite * m_pPlayer;
    std::vector<cocos2d::Sprite *> m_followers;
    std::vector<cocos2d::Sprite *> m_enemies;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void buildPlayer();
    void moveEnemies();
    int getEnemyMoveTile(cocos2d::Sprite * pEnemy);
    void moveToTile(cocos2d::Sprite * pCharecter, cocos2d::Node * pTile, int tileIndex);
    void checkCollisions();
    void onMoveDone(cocos2d::Node * pSender);
    bool isFollowerOnTile(int tileIndex);
    void moveFollowers();
    int getMapTileType(int x, int y);
    bool canEnemyMoveTo(int x, int y);
    EnemyPattern * buildEnemyPattern(int type);
};

#endif // __GAME_SCENE_SPLIT_H__
