//
//  Edge.cpp
//  fill
//
//  Created by Kfir Goldman on 1/15/15.
//
//

#include "Edge.h"

// ===========================================================
// Initialization & cleanup
// ===========================================================

Edge::Edge(cocos2d::Vec2 * p1, cocos2d::Vec2 * p2)
{
    m_pPoints[0] = p1;
    m_pPoints[1] = p2;
}

Edge::~Edge()
{    
}