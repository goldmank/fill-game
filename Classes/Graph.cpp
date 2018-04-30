//
//  Graph.cpp
//  fill
//
//  Created by Kfir Goldman on 1/15/15.
//
//

#include "Graph.h"

using namespace cocos2d;

// ===========================================================
// Initialization & cleanup
// ===========================================================

Graph::Graph()
{
    
}

Graph::~Graph()
{
    
}

// ===========================================================
// Public methods
// ===========================================================

void Graph::addPoint(float x, float y)
{
    m_points.push_back(Vec2(x, y));
}

void Graph::addEdge(float x1, float y1, float x2, float y2)
{
    addPoint(x1, y1);
    addPoint(x2, y2);
    addEdge((int)m_points.size() - 2, (int)m_points.size() - 1);
}

void Graph::addEdge(int p1Index, int p2Index)
{
    Vec2 * p1 = &m_points[p1Index];
    Vec2 * p2 = &m_points[p2Index];
    m_edges.push_back(Edge(p1, p2));
}