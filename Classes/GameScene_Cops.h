#ifndef __GAME_SCENE_COPS_H__
#define __GAME_SCENE_COPS_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_Cops : public BaseScene, public b2ContactListener
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Cops();
    ~GameScene_Cops();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Cops);
    
    // ===========================================================
    // Accelerometer methods
    // ===========================================================
    
    void onAcceleration(cocos2d::Acceleration *acc, cocos2d::Event *event);
    
    // ===========================================================
    // Box2d collision methods
    // ===========================================================
    
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
    
protected:
    
    // ===========================================================
    // BaseScene methods
    // ===========================================================
    
    void handleInit(float width, float height);
    void handleUpdate(float dt);
    
    cocos2d::ccColor3B getFillColor() { return Pref::getInstance()->getColor(COLOR_BORDER); }
    
    bool handleOnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void handleOnTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
    
    virtual void onSwipeLeft();
    virtual void onSwipeRight();
    virtual void onSwipeUp();
    virtual void onSwipeDown();
    
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
    
    bool m_bDone;
    b2World * m_pWorld;
    cocos2d::CCNode * m_pPlayer;
    b2Body * m_pPlayerBody;
    cocos2d::Vec2 m_initialTouchPos;
    float m_fInitialTouchTime;
    std::vector<MyContact> m_contacts;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void updateWorld(float dt);
    void buildPlayer(float groundY);
    void throwObject();
    void checkCollisions();
    void killEnemy(b2Body * pBody);
};

#endif // __GAME_SCENE_COPS_H__
