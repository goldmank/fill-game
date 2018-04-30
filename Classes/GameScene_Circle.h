#ifndef __GAME_SCENE_CIRCLE_H__
#define __GAME_SCENE_CIRCLE_H__

#include "GameSceneBlocks.h"

class GameScene_Circle : public GameSceneBlocks
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
    
    GameScene_Circle();
    ~GameScene_Circle();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Circle);
    
protected:
    
    // ===========================================================
    // GameScene methods
    // ===========================================================
    
    void handleCellTouchStart(cocos2d::Touch* pTouch);
    void handleCellTouchMove(cocos2d::Touch* pTouch);
    void handleCellTouchEnd(cocos2d::Touch* pTouch);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    std::vector<WorkCircle> m_workCircles;
    std::vector<WorkCircle> m_filledCircles;
    cocos2d::Sprite * m_pCircle;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void startCircle(cocos2d::Vec2 pos);
    void onFillDone(cocos2d::CCNode * pSender);
};

#endif // __GAME_SCENE_CIRCLE_H__
