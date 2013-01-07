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

#ifndef GLP_SLGspan_h
#define GLP_SLGspan_h

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <Eigen/Core>
#include "SLGraphMiningStrategy.h"
#include "Gspan/Graph.h"
#include "Gspan/Dfs.h"
#include "Gspan/Utility.h"
#include "Gspan/tree.hh"

using namespace std;
using namespace Eigen;

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
        SLGspanParameters() : minsup(2), maxpat(10), n(10), topk(10) {}
        
    public:
        // assignable parameters
        size_t minsup;
        size_t maxpat;
        size_t n;
        size_t topk;
    };

public:
    SLGspan (): wbias(0.0), tau(0.0) {};

    // Implementation SLGraphMiningStrategy Protocol
//    virtual MatrixXd& search();
    bool setParameters(SLGspanParameters parameters);

private:
    bool is_min();
    bool project_is_min(Projected&);
    
//    bool can_prune(Projected &);
    bool can_prune_boost(Projected&);
    
    void projectForTrain(Projected&, tree<TNODE>::iterator&);
    void projectForTest(Projected&);

    istream& read(std::istream&);
    
private:
    // assignable parameters
    size_t minsup;
    size_t maxpat;
    size_t n;
    size_t topk;
    
    // not assignable parameters
    vector<Graph>   transaction;
    DFSCode         DFS_CODE;
    DFSCode         DFS_CODE_IS_MIN;
    Graph           GRAPH_IS_MIN;
    
    VectorXd y;
    VectorXd w;
    
    Rule rule;
    set<Rule> rule_cache;
    vector <int> result;
    ostream *os;
    
    double wbias;
    double tau;
    
    tree<TNODE> memCache;
};

#endif /* defined(__GLP__SLGspan__) */
