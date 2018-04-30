#ifndef __GAME_SCENE_COWBOY_H__
#define __GAME_SCENE_COWBOY_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_Cowboy : public BaseScene, public b2ContactListener
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Cowboy();
    ~GameScene_Cowboy();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Cowboy);
    
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
    
    void onSwipeLeft();
    void onSwipeRight();
    
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
    cocos2d::Sprite * m_pPlayer;
    b2Body * m_pPlayerBody;
    std::vector<MyContact> m_contacts;
    float m_fAddEnemyTimer;
    cocos2d::Vec2 m_initialTouchPos;
    float m_fInitialTouchTime;
    bool m_bShooting;
    cocos2d::Vec2 m_shootPoint;
    float m_fAddBulletTimer;
    cocos2d::CCLabelTTF * m_pScoreText;
    int m_iScore;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void updateWorld(float dt);
    void buildPlayer(float groundY);
    void checkCollisions();
    void killEnemy(b2Body * pBody);
    void shoot();
    void addEnemy(float x, float y);
    void hitEnemy(cocos2d::Node * pEnemy);
    void onPlayerHit();
    void onShootBulletDone(cocos2d::Node * pSender);
    void updateScoreText();
};

#endif // __GAME_SCENE_COWBOY_H__
