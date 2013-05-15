//
//  Dfs.h
//  GLP
//
//  Copyright (C) 2004 Taku Kudo
//  Copyright (C) 2006 Taku Kudo, Hiroto Saigo, Koji Tsuda
//
//  Created by Zheng Shao on 1/5/13.
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

#ifndef GLP_Gspan_Dfs_h
#define GLP_Gspan_Dfs_h

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Graph.h"

using namespace std;

typedef vector<int>    RMPath;
typedef vector<Edge*>  EdgeList;

class DFS
{
public:
    int from;
    int to;
    int fromlabel;
    int elabel;
    int tolabel;
    
public:
    friend bool operator != (const DFS &d1, const DFS &d2) { return (! (d1 == d2)); }
    friend bool operator == (const DFS &d1, const DFS &d2)
    {
        return (d1.from == d2.from && d1.to == d2.to && d1.fromlabel == d2.fromlabel
                && d1.elabel == d2.elabel && d1.tolabel == d2.tolabel);
    }
    DFS(): from(0), to(0), fromlabel(0), elabel(0), tolabel(0) {};
};

struct DFSCode: public vector <DFS>
{
private:
    RMPath rmpath;
    
public:
    const RMPath& buildRMPath ();
    bool toGraph (Graph &);
    void push (int from, int to, int fromlabel, int elabel, int tolabel)
    {
        resize (size() + 1);
        DFS &d = (*this)[size()-1];
        d.from = from; d.to = to; d.fromlabel = fromlabel; d.elabel = elabel; d.tolabel = tolabel;
    }
    void pop () {  resize (size()-1); }
    ostream &write  (ostream &); // write
};

struct PDFS // projected DFS
{
    int id;
    Edge        *edge;
    PDFS        *prev;
    PDFS(): id(0), edge(0), prev(0) {};
};

class History: public vector<Edge*>
{
private:
    vector<int> edge;
    vector<int> vertex;
    
public:
    bool hasEdge   (size_t id) { return (bool)edge[id]; }
    bool hasVertex (size_t id) { return (bool)vertex[id]; }
    void build     (Graph &, PDFS *);
    History() {};
    History(Graph& g, PDFS *p) { build (g, p); }
};

class Event
{
public:
    size_t type;
    size_t dfscode;
    size_t treesize;
    double time1;
    double time2;
    vector <double> beta;
    vector <double> gamma;
    vector <size_t> active_dfs;
};

class Rule
{
public:
    string dfs;
    double gain;
    size_t size;
    vector <size_t> loc;
    
public:
    friend bool operator < (const Rule &r1, const Rule &r2)
    {
//        return r1.dfs < r2.dfs;
        return fabs(r1.gain) < fabs(r2.gain);
    }
    
    bool operator == (const string &dfs) const
    {
        return (*this).dfs == dfs;
    }

    bool operator == (const Rule &r1) const
    {
        return (*this).dfs == r1.dfs;
    }
};

class Projected: public vector<PDFS>
{
public:
    void push (int id, Edge *edge, PDFS *prev)
    {
        resize (size() + 1);
        PDFS &d = (*this)[size()-1];
        d.id = id; d.edge = edge; d.prev = prev;
    }
};

enum{
    TNODEYETEXPLORE = 1 << 0,
};

typedef size_t TNODEIDTYPE;

struct TNODE
{
    TNODEIDTYPE id;
    DFSCode dfscode;
    Projected projected;
};

#endif
