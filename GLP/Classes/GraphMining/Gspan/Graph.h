//
//  Graph.h
//  GLP
//
//  Copyright (C) 2004 Taku Kudo
//  Copyright (C) 2006 Taku Kudo, Hiroto Saigo, Koji Tsuda
//
//  Created by Zheng Shao on 1/4/13.
//  Copyright (c) 2013 Saigo Laboratoire. All rights reserved.
//
//  This is free software with ABSOLUTELY NO WARRANTY.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//  02111-1307, USA
//

#ifndef GLP_Graph_h
#define GLP_Graph_h

#include <iostream>
#include <vector>

struct Edge
{
    int from; // must not be from <= to;
    int to;
    int elabel;
    size_t id;
    Edge(): from(0), to(0), elabel(0), id(0) {};
};

class Vertex
{
public:
    typedef std::vector<Edge>::iterator edge_iterator;
    
    int label;
    std::vector<Edge> edge;
    
public:   
    void push (int from, int to, int elabel)
    {
        edge.resize (edge.size()+1);
        edge[edge.size()-1].from   = from;
        edge[edge.size()-1].to     = to;
        edge[edge.size()-1].elabel = elabel;
        return;
    }
};

class Graph: public std::vector<Vertex>
{
private:
    size_t edge_size_;
    
public:
    double y;       // class label
    double regy;    // regression label
    
public:
    size_t edge_size ()   { return edge_size_; }
    size_t vertex_size () { return (size_t)size(); } // wrapper
    void buildEdge ();
    std::istream &read   (std::istream &); // read
    std::ostream &write  (std::ostream &); // write
};

#endif
