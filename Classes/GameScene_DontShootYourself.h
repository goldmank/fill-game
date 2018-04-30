#ifndef __GAME_SCENE_D_H__
#define __GAME_SCENE_D_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

class GameScene_DontShootYourself : public BaseScene, public b2ContactListener
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_DontShootYourself();
    ~GameScene_DontShootYourself();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_DontShootYourself);
    
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
    
    cocos2d::Sprite * m_pPlayer;
    b2Body * m_pPlayerBody;
    std::vector<cocos2d::Sprite *> m_enemies;
    std::vector<cocos2d::Sprite *> m_guide;
    std::vector<cocos2d::Vec2> m_guidePoints;
    int m_guidePointIndex;
    bool m_bDrawGuide;
    float m_fLastAddedEnemy;
    b2World * m_pWorld;
    cocos2d::Vec2 m_oldPlayerPos;
    cocos2d::Vec2 m_forceTo;
    float m_fForceTimer;
    float m_fDestAngle;
    bool m_bGotHit;
    std::vector<MyContact> m_contacts;
    cocos2d::CCLabelTTF * m_pCounter;
    cocos2d::CCLabelTTF * m_pStars;
    float m_fLastAddedStar;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld();
    void buildPlayer();
    void addEnemy();
    void addStar();
    void updateWorld(float dt);
    void checkCollisions();
    void addGuideDot(cocos2d::Vec2 pos);
    void clearGuideDots();
    void removeSprite(cocos2d::Node * pNode);
    void collectStar(cocos2d::Node * pStar);
};

#endif // __GAME_SCENE_D_H__
