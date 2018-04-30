#ifndef __GAME_SCENE_RUNUP_H__
#define __GAME_SCENE_RUNUP_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_RunUp : public BaseScene, public b2ContactListener
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_RunUp();
    ~GameScene_RunUp();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_RunUp);
    
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
    // GroundDetails
    // ===========================================================
    
    struct GroundDetails
    {
        std::vector<b2Body *> m_bodies;
        std::vector<float> m_holes;
    };
    
    // ===========================================================
    // EnemyDetails
    // ===========================================================
    
    struct EnemyDetails
    {
        int m_iDirection;
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    bool m_bDone;
    cocos2d::Layer * m_pGameLayer;
    b2World * m_pWorld;
    cocos2d::CCNode * m_pPlayer;
    b2Body * m_pPlayerBody;
    float m_fMoveXSpeed;
    std::vector<MyContact> m_contacts;
    std::vector<GroundDetails> m_grounds;
    std::vector<b2Body *> m_pWalls;
    bool m_bJumping;
    float m_fLastDy;
    int m_iCurrGround;
    float m_fLastJumpTime;
    float m_fHitJumperTime;
    int m_iStars;
    float m_fAddEnemyCounter;
    float m_fHitSafeCounter;
    cocos2d::CCLabelTTF * m_pDistanceText;
    cocos2d::CCLabelTTF * m_pStarsText;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    b2Body * createGroundBody(float x, float y, float w, float h, cocos2d::Color3B color, int tag, float angle);
    GroundDetails addGround(float y, int * values, float angle = 0);
    void buildWorld(float groundY);
    void updateWorld(float dt);
    void buildPlayer(float groundY);
    void checkCollisions();
    void doJump();
    void addEnemy(float y);
    void killEnemy(b2Body * pBody);
    void onPlayerHit();
    void onPlayerRestartAfterHit();
    void onStartCollected(cocos2d::Node * pStar);
};

#endif // __GAME_SCENE_RUNUP_H__
