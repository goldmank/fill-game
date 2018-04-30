#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

//#define FILL_COLOR ccc3(176, 203, 205)
//#define BORDER_COLOR ccc3(139, 153, 156)
//#define ENEMY_COLOR ccc3(93, 149, 224)

#define GAME_MAX_TIME 60
#define ENEMY_SPEED 8

#define BORDER_SIZE (getScreenHeight() * 0.01f)
#define TAG_RIGHT_WALL (-2)
#define TAG_TOP_WALL (-3)
#define ENEMY_SIZE (0.2f)
#define FILL_SPEED (2)

class GameScene : public BaseScene
{
public:
    
    // ===========================================================
    // WorkCircle
    // ===========================================================
    
    struct WorkCircle
    {
        cocos2d::ProgressTimer * m_pCircleProgress;
        cocos2d::Sprite * m_pCircle;
        float m_fReleaseTime;
        float m_r;
        float m_x;
        float m_y;
    };
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene();
    ~GameScene();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);
    
    // ===========================================================
    // Public methods
    // ===========================================================
    
    bool isDone() { return m_bDone; }

    void test(std::vector<WorkCircle> circles);
    std::vector<WorkCircle> getFilledCirles();
    void addFilledCircle(WorkCircle wc);
    
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
    
    // ===========================================================
    // GameScene methods
    // ===========================================================
    
    virtual void prepareEnemies(cocos2d::CCLayer * pEnemies) {}
    virtual void updateEnemies(cocos2d::CCLayer * pEnemies, float dt) {}
    virtual void onRestart(cocos2d::CCNode * pSender) {}
    virtual void enemySlowdown(cocos2d::CCLayer * pEnemies, bool slowdown) {}
    
    b2World * getWorld() { return m_pWorld; }
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    b2World * m_pWorld;
    b2Body * m_pGround;
    b2Fixture * m_pGroundFixture;
    b2Body * m_pCenterBody;
    bool * m_pFillBuffer;
    cocos2d::CCLayer * m_pEnemies;
    cocos2d::CCRenderTexture * m_pCanvas;
    bool m_bDone;
    float m_bShowGameOver;
    pthread_mutex_t m_checkMutex;
    pthread_t m_checkThread;   
    cocos2d::CCLabelTTF * m_pCounter;
    int m_iFillCount;
    float m_fCounterSize;
    cocos2d::CCProgressTimer * m_pTimer;
    std::vector<WorkCircle> m_workCircles;
    std::vector<WorkCircle> m_filledCircles;
        
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildWorld(float screenWidth, float screenHeight);
    void startCircle(cocos2d::Vec2 pos);
    void addCenter();
    void onGameOver();
    void finishCircleFill(WorkCircle & wc);
    void runWorld(float dt);
    void checkCollision();
    void onCounterFadeDone(cocos2d::CCNode * pSender);
};

#endif // __GAME_SCENE_H__
