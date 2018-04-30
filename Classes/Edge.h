//
//  Edge.h
//  fill
//
//  Created by Kfir Goldman on 1/15/15.
//
//

#ifndef __fill__Edge__
#define __fill__Edge__

#include "cocos2d.h"

class Edge
{
public:
    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    Edge(cocos2d::Vec2 * p1, cocos2d::Vec2 * p2);
    ~Edge();
    
    // ===========================================================
    // Public methods
    // ===========================================================
    
    inline cocos2d::Vec2 * getPoint(int index) { return m_pPoints[index]; }
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    cocos2d::Vec2 * m_pPoints[2];
};

#endif /* defined(__fill__Edge__) */
