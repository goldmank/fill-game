#ifndef __GAME_SCENE_MAPPUZZLE2_H__
#define __GAME_SCENE_MAPPUZZLE2_H__

#include "cocos2d.h"
#include "BaseScene.h"
#include "Pref.h"

#define MAP_WIDTH 6
#define MAP_HEIGHT 6

class GameScene_Map2PlayersFight : public BaseScene
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    GameScene_Map2PlayersFight();
    ~GameScene_Map2PlayersFight();
    
    // ===========================================================
    // CCLayer methods
    // ===========================================================
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene_Map2PlayersFight);
    
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
    // Cell
    // ===========================================================
    
    struct Cell
    {
        int index;
        cocos2d::Sprite * m_pTile;
        std::vector<Cell *> m_neighbours;
    };
    
    struct Game
    {
        std::vector<int> m_moves;
        std::vector<int> m_oppMoves;
    };
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    cocos2d::Sprite * m_pMap[MAP_WIDTH * MAP_HEIGHT];
    cocos2d::Sprite * m_pPlayer;
    cocos2d::Sprite * m_pOpponent;
    cocos2d::Sprite * m_pTouchedCell;
    bool m_bReady;
    bool m_bOppTurn;
    std::vector<Game> m_games;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
    void buildMap();
    void moveTo(cocos2d::Sprite * pPlayer, cocos2d::Sprite * pCell);
    void onMoveDone(cocos2d::Node * pPlayer);
    void doOppStep();
    int canGoToCellInRow(int from, int to);
    int canGoToCellInCol(int from, int to);
    
    void test(int * pMap, int playerIndex, int oppIndex, std::vector<int> stepsPlayer, std::vector<int> stepsOpp);
    int * copyMap(int * pOther);
    void printMap(int * pMap);
    std::vector<int> getPossibleMoves(int * pMap, int index);
    void play(std::vector<int> stepsPlayer, std::vector<int> stepsOpp);
};

#endif // __GAME_SCENE_MAPPUZZLE2_H__
