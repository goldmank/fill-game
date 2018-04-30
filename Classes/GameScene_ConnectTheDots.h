#ifndef __GAME_SCENE_CTD_H__
#define __GAME_SCENE_CTD_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_ConnectTheDots : public BaseScene, public b2ContactListener
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_ConnectTheDots();
    ~GameScene_ConnectTheDots();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_ConnectTheDots);
    
    // ===========================================================
    // Public methods
    // ===========================================================
    
protected:
    
    // ===========================================================
    // Box2d collision methods
    // ===========================================================
    
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
    
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
    // MyContact
    // ===========================================================
    
    struct MyContact
    {
        b2Fixture *fixtureA;
        b2Fixture *fixtureB;
        bool operator==(const MyContact& other) const
        {
            return (fixtureA == other.fixtureA) && (fixtureB == other.fixtureB);
        }
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    std::vector<cocos2d::Sprite *> m_enemies;
    b2World * m_pWorld;
    std::vector<MyContact> m_contacts;
    std::vector<cocos2d::Sprite *> m_leftCells;
    std::vector<cocos2d::Sprite *> m_rightCells;
    cocos2d::Sprite * m_pLeftSelected;
    cocos2d::Sprite * m_pRightSelected;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void addEnemy();
    void updateWorld(float dt);
    void checkCollisions();
    void removeSprite(cocos2d::Node * pNode);
};

#endif // __GAME_SCENE_CTD_H__
