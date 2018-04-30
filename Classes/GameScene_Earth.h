#ifndef __GAME_SCENE_E_H__
#define __GAME_SCENE_E_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_Earth : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Earth();
    ~GameScene_Earth();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Earth);
    
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
    // Fields
    // ===========================================================
    
    b2World * m_pWorld;
    cocos2d::Sprite * m_pGround;
    b2Body * m_pGroundBody;
    cocos2d::Sprite * m_pPlayer;
    b2Body * m_pPlayerBody;
    std::vector<cocos2d::Sprite *> m_groundItems;
    std::vector<cocos2d::Sprite *> m_enemies;
    float m_fLastAddedEnemy;
        
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void buildPlayer();
    void addEnemy();
    void updateWorld(float dt);    
    void removeSprite(cocos2d::Node * pNode);
};

#endif // __GAME_SCENE_E_H__
