#ifndef __GAME_SCENE_FIGHTLINE_H__
#define __GAME_SCENE_FIGHTLINE_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

#define BASE_COUNT 4
#define INVENTORY_COUNT 4

class GameScene_FightLine : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_FightLine();
    ~GameScene_FightLine();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_FightLine);
    
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
    // Fields
    // ===========================================================
    
    cocos2d::Sprite * m_pInventory[INVENTORY_COUNT];
    cocos2d::Sprite * m_pBases[BASE_COUNT];
    cocos2d::Sprite * m_pOppInventory[INVENTORY_COUNT];
    cocos2d::Sprite * m_pOppBases[BASE_COUNT];
    cocos2d::Sprite * m_pTouchedItem;
    cocos2d::Sprite * m_pSelectedAction;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildUI();
    cocos2d::Sprite * buildActionBtn(float x, float y, const char * btn, int type, int count);
    void selectAction(cocos2d::Sprite * pAction);
    void releaseFromBase(cocos2d::Sprite * pBase);
};

#endif // __GAME_SCENE_FIGHTLINE_H__
