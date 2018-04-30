#ifndef __TEST_FILL_H__
#define __TEST_FILL_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"

#define PTM_RATIO 32

#define GAME_MAX_TIME 60
#define ENEMY_SPEED 4

#define BORDER_SIZE (getScreenHeight() * 0.01f)
#define TAG_RIGHT_WALL (-2)
#define TAG_TOP_WALL (-3)
#define ENEMY_SIZE (0.12f)
#define FILL_SPEED (1.0f)

#define GAME_TIME (60)

#define BOUNS_TIME_DURATION (10)
#define BOUNS_SPEED_DURATION (10)
#define BOUNS_FILL_DURATION (10)

#define TAG_EMPTY 0
#define TAG_FILLING 1
#define TAG_D_FILLING 2
#define TAG_FILLED 3

class GameSceneBlocks : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameSceneBlocks();
    ~GameSceneBlocks();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameSceneBlocks);
    
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
    
    virtual void handleCellTouchStart(cocos2d::Touch* pTouch);
    virtual void handleCellTouchMove(cocos2d::Touch* pTouch);
    virtual void handleCellTouchEnd(cocos2d::Touch* pTouch);
    
    // ===========================================================
    // Protected methods
    // ===========================================================
    
    virtual void prepareEnemies(cocos2d::CCLayer * pEnemies) {}
    virtual void updateEnemies(cocos2d::CCLayer * pEnemies, float dt) {}
    virtual void onRestart(cocos2d::CCNode * pSender) {}
    virtual void enemySlowdown(cocos2d::CCLayer * pEnemies, bool slowdown) {}
    virtual int getLevelNumber() { return 1; }
    
    b2World * getWorld() { return m_pWorld; }
    cocos2d::CCLayer * getEnemies() { return m_pEnemies; }
    cocos2d::CCLayer * getCells() { return m_pCells; }
    int getColsCount() { return m_iCols; }
    int getRowsCount() { return m_iRows; }
    void setFilledToBlocker(bool enable) { m_bFilledToBlocker = enable; }
    void setDoubleFilledCel(int index);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    bool m_bDone;
    cocos2d::CCLayer * m_pCells;
    cocos2d::CCLayer * m_pEnemies;
    b2World * m_pWorld;
    cocos2d::CCLabelTTF * m_pScoreLabel;
    cocos2d::CCLabelTTF * m_pTimeLabel;
    cocos2d::CCLabelTTF * m_pScore;
    cocos2d::CCLabelTTF * m_pTime;
    int m_iDestScore;
    int m_iScore;
    int m_iTime;
    float m_fScoreAnimCounter;
    cocos2d::CCLabelTTF * m_pTimeBounsCount;
    cocos2d::CCLabelTTF * m_pFillBounsCount;
    cocos2d::CCLabelTTF * m_pSpeedBounsCount;
    cocos2d::CCNode * m_pClickedBtn;
    int m_iCols;
    int m_iRows;
    cocos2d::CCNode * m_pRestart;
    cocos2d::CCNode * m_pHome;
    cocos2d::CCNode * m_pShop;
    cocos2d::CCNode * m_pShare;
    float m_fTimer;
    bool m_bPaused;
    cocos2d::CCNode * m_pTimeBounsProgress;
    cocos2d::CCNode * m_pSpeedBounsProgress;
    cocos2d::CCNode * m_pFillBounsProgress;
    float m_fTimeBounsTimer;
    float m_fSpeedBounsTimer;
    float m_fFillBounsTimer;
    std::vector<cocos2d::CCNode *> m_filledCells;
    bool m_bFilledToBlocker;
    float m_fBounsAppearTimer;
    float m_fFooterHeight;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    float addHeader();
    cocos2d::Node * buildTimer();
    float addFooter();
    void buildWorld(float screenWidth, float screenHeight, float headerHeight, float footerHeight);
    void runWorld(float dt);
    void checkCellTouch(cocos2d::Touch* pTouch);
    void onFillCellDone(cocos2d::CCNode * pSender);
    void checkCollisions();
    void onGameOver();
    void updateScoreText();
    void updateTimeText();
    void updateBounsText(cocos2d::CCLabelTTF * pText, int value);
    cocos2d::CCNode * getBounsBtnClicked(cocos2d::Vec2 loc);
    bool isBounsBtnClicked(cocos2d::CCLabelTTF * pBtn, cocos2d::Vec2 loc);
    void showGameOverScreen(bool cleared, const char * titleText);
    void onHome(cocos2d::CCNode * pSender);
    void onShop(cocos2d::CCNode * pSender);
    void onShare(cocos2d::CCNode * pSender);
    void onTimeBounsClicked();
    void onFillBounsClicked();
    void onSpeedBounsClicked();
    void showBuyPopup();
    void pauseGame();
    void resumeGame();
    void updateBounsTimers(float dt);
};

#endif // __TEST_FILL_H__
