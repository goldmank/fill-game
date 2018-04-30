#ifndef __GAME_SCENE_FRUITNINJA_H__
#define __GAME_SCENE_FRUITNINJA_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"
#include "../cocos2d/external/Box2D/Box2D.h"
#include "GeometricRecognizer.h"

#define PTM_RATIO 32

class GameScene_FruitNinja : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_FruitNinja();
    ~GameScene_FruitNinja();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_FruitNinja);
    
    // ===========================================================
    // Public methods
    // ===========================================================
    
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
    
private:
   
    // ===========================================================
    // ObjectData
    // ===========================================================
    
    struct ObjectData
    {
        float m_fStartTime;
        int m_iType;
        float m_fInitialScale;
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    b2World * m_pWorld;
    float m_fLastEnemyAddTime;
    DollarRecognizer::GeometricRecognizer m_gestureDetector;
    DollarRecognizer::Path2D m_gesture;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void updateWorld(float dt);
    void addObject(float x, float y, float v, float a);
};

#endif // __GAME_SCENE_FRUITNINJA_H__
