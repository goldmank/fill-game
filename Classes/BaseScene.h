#ifndef __BASE_SCENE_H__
#define __BASE_SCENE_H__

#include "cocos2d.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class BaseScene : public cocos2d::LayerColor
{
public:
	// =========================================================== 
    // Initialization & cleanup
    // ===========================================================

	BaseScene();
	virtual ~BaseScene();

	// =========================================================== 
    // CCLayer methods
    // ===========================================================

    virtual bool init();

	// =========================================================== 
    // Public methods
    // ===========================================================

	void update(float dt);
    
protected:
	
    // ===========================================================
    // Fields
    // ===========================================================
    
    virtual bool handleOnTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {}
    virtual void handleOnTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {}
    virtual void handleOnTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {}
    virtual void handleOnTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) {}
    
	// =========================================================== 
    // Common methods
    // ===========================================================
	
	virtual void handleInit(float width, float height);
	virtual void handleUpdate(float dt);
	
	// =========================================================== 
    // Helper methods
    // ===========================================================

	int getScreenWidth();
	int getScreenHeight();
	bool isClicked(cocos2d::CCNode * pSprite, cocos2d::Touch * pTouch);
	float getTime();
	void switchScene(cocos2d::CCScene * pScene);
    virtual cocos2d::ccColor3B getFillColor();
		
private:

	// =========================================================== 
    // Fields
    // ===========================================================

    cocos2d::EventListenerTouchOneByOne * m_pTouchListener;
	int m_iScreenWidth;
	int m_iScreenHeight;
	float m_fTime;
    
	// =========================================================== 
    // Private methods
    // ===========================================================

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif // __BASE_SCENE_H__
