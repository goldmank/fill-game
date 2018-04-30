#ifndef __TEST_SHOOT_3STICKS3D_H__
#define __TEST_SHOOT_3STICKS3D_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

class GameScene_Rumble3D : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Rumble3D();
    ~GameScene_Rumble3D();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Rumble3D);
    
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
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
};

#endif // __TEST_SHOOT_3STICKS3D_H__
