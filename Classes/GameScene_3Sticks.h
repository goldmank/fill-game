#ifndef __TEST_SHOOT_3STICKS_H__
#define __TEST_SHOOT_3STICKS_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_3Sticks : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_3Sticks();
    ~GameScene_3Sticks();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_3Sticks);
    
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
    std::vector<cocos2d::Sprite*> m_objectsVec;
    std::vector<cocos2d::Sprite*> m_enemiesVec;
    float m_fLastEnemyAddTime;
    float m_fLastStarAddTime;
    cocos2d::Sprite* m_pTouchedObject;
    cocos2d::CCLabelTTF * m_pScore;
    cocos2d::CCLabelTTF * m_pStars;
    int m_iScore;
    int m_iStars;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    cocos2d::Sprite * addObject(int type, int direction, float speed);
    void onObjectMoveDone(cocos2d::Node * pSender);
    void checkCollisions();
    void onEnemyHit();
    void onStarCollect();
};

#endif // __TEST_SHOOT_3STICKS_H__
