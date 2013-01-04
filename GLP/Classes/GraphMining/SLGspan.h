//
//  SLGspan.h
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
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

#ifndef __GLP__SLGspan__
#define __GLP__SLGspan__

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include "SLGraphMiningStrategy.h"
#include "Gspan/Graph.h"
#include "Gspan/Dfs.h"
#include "Gspan/Utility.h"
#include "Gspan/tree.hh"
#include "Gspan/darts.h"
#include "Gspan/mmap.h"

using namespace std;

typedef map<int, Projected>         Projected_map1;
typedef map<int, Projected_map1>    Projected_map2;
typedef map<int, Projected_map2>    Projected_map3;

typedef map<int, Projected>::iterator              Projected_iterator1;
typedef map<int, Projected_map1>::iterator         Projected_iterator2;
typedef map<int, Projected_map2>::iterator         Projected_iterator3;
typedef map<int, Projected_map2>::reverse_iterator Projected_riterator3;

class SLGspan : public SLGraphMiningStrategy
{
public:
    class SLGspanParameters
    {
    public:
        SLGspanParameters() : a(1) {}
        
    public:
        // assignable parameters
        int a;
    };

public:
    SLGspan (): ID(0), minsup(0), maxpat(0), wbias(0.0), tau(0.0), least_tau(0.0),
    alpha(0), bias(0.0), os(0) {};

    // Implementation SLGraphMiningStrategy Protocol
    MatrixXd& search();
    bool setParameters(SLGspanParameters parameters);

private:
    bool is_min ();
    bool project_is_min (Projected &);
    bool is_same_code ();
    
    bool can_prune (Projected &);
    bool can_prune_boost (Projected &, size_t);
    
    void project (Projected &, tree<TNODE>::iterator &); // for training mode
    void project (Projected &); // for test mode

private:
    // assignable parameters
    int a;
    
    // not assignable parameters
    vector<Graph>   transaction;
    DFSCode         DFS_CODE;
    DFSCode         DFS_CODE_IS_MIN;
    Graph           GRAPH_IS_MIN;
    
    vector <double> y;
    vector <double> w;
    set<Rule>       rule_cache;
    
    size_t ID;
    size_t minsup;
    size_t maxpat;
    size_t maxitr;
    size_t tasktype;
    size_t n;
    size_t topk;
    double       wbias;
    double       tau;
    double       least_tau;
    
    clock_t stime, etime1, etime2;
    list<double> tau_list;
    
    double rho0;
    double eta0;
    vector <double> uw;
    vector <double> uv;
    
    Rule rule;
    Darts::DoubleArray da;
    MeCab::Mmap<char> mmap;
    size_t alphaSize;
    double *alpha;
    double bias;    // = -Beta.sum()
    map <string, double> rules;
    vector <int> result;
    ostream *os;
    
    tree<TNODE> tr;
};

#endif /* defined(__GLP__SLGspan__) */
