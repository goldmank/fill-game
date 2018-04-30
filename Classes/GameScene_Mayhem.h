#ifndef __GAME_SCENE_MAYHEM_H__
#define __GAME_SCENE_MAYHEM_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_Mayhem : public BaseScene, public b2ContactListener
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Mayhem();
    ~GameScene_Mayhem();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Mayhem);
        
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
    // EnemyDetails
    // ===========================================================
    
    struct EnemyDetails
    {
        bool m_bRunToPlayer;
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    bool m_bDone;
    b2World * m_pWorld;
    cocos2d::CCNode * m_pPlayer;
    b2Body * m_pPlayerBody;
    float m_fMoveXSpeed;
    std::vector<MyContact> m_contacts;
    float m_fGunAngle;
    cocos2d::CCSprite * m_pGun;
    int m_iMoveTouchId;
    int m_iShootTouchId;    
    cocos2d::Sprite * m_pGunHolder;
    float m_fBulleteCreateTimer;
    float m_fAddEnemyTimer;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void updateWorld(float dt);
    void buildPlayer(float groundY);
    void checkCollisions();
    void killEnemy(b2Body * pBody);
    void shoot();
    void addEnemy();
    void onPlayerHit();
};

#endif // __GAME_SCENE_MAYHEM_H__
