//
//  Graph.h
//  fill
//
//  Created by Kfir Goldman on 1/15/15.
//
//

#ifndef __fill__Graph__
#define __fill__Graph__

#include "cocos2d.h"
#include "Edge.h"

class Graph
{
public:

    
    // ===========================================================
    // Initialization & cleanup
    // ===========================================================
    
    Graph();
    ~Graph();

    // ===========================================================
    // Public methods
    // ===========================================================
    
    void addPoint(float x, float y);
    void addEdge(float x1, float y1, float x2, float y2);
    void addEdge(int p1Index, int p2Index);
    
private:
    
    // ===========================================================
    // Fields
    // ===========================================================
    
    std::vector<cocos2d::Vec2> m_points;
    std::vector<Edge> m_edges;
    
    // ===========================================================
    // Private methods
    // ===========================================================
    
   
};

#endif /* defined(__fill__Graph__) */
