#ifndef __TEST_MR_RUN__
#define __TEST_MR_RUN__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_MrRun : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_MrRun();
    ~GameScene_MrRun();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_MrRun);
    
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
    
    // ===========================================================
    // Protected methods
    // ===========================================================
    
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    bool m_bDone;
    cocos2d::Layer * m_pGameLayer;
    cocos2d::Sprite * m_pPlayer;
    float m_fSpeed;
    float m_fJumpStartTimer;
    bool m_bJumping;
    bool m_bTouchDown;
    float m_fFallStartTime;
    cocos2d::Sprite * m_pFallToTile;
    int m_iDistance;
    cocos2d::CCLabelTTF * m_pDistanceText;
    bool m_bDisableTouch;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildLevel();
    void onGameOver();
    void startJumpUp();
};

#endif // __TEST_MR_RUN__
