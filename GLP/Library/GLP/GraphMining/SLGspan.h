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
#include "SLGraphMiningStrategy.h"
#include "Gspan/Graph.h"
#include "Gspan/Dfs.h"
#include "Gspan/Utility.h"
#include "Gspan/tree.hh"

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
        SLGspanParameters() : minsup(2), maxsup(ULLONG_MAX), maxpat(10), topk(1), doesUseMemoryBoost(false) {}
        
    public:
        // assignable parameters
        size_t minsup;
        size_t maxsup;  // only use for standlone gspan version
        size_t maxpat;  // upper bound on node count
        size_t topk;
        
        bool    doesUseMemoryBoost;
        string  gspFilename;
    };

public:
    SLGspan (): wbias(0.0), tau(0.0) {};

    // Implementation SLGraphMiningStrategy Protocol
    
    /* Search substructure
     * Input
     *      residual: the residual of response Y.
     *                Spectial mode when residual is empty, search all patterns.
     *      taskType: must be specified either train or classify
     *    resultType: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLGraphMiningResult search(VectorXd residual,
                                       SLGRAPHMININGTASKTYPE taskType,
                                       SLGRAPHMININGRESULTYPE resultType);
    /* Get Inner Values
     * Input
     *    type: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLGraphMiningInnerValues getInnerValues(SLGRAPHMININGINNERVALUE type) const;
    
    /* Set Parameters:
     * Input
     *      parameters: Gspan assignable parameters
     *
     * Return: true if sucessed.
     */
    bool setParameters(SLGspanParameters& parameters);
    
    double classify(Graph &g, int flag);
    
    void setTransaction(vector<Graph>);
    
    vector<Graph>& getTransaction();

private:
    void init();
    void initDFSTree(Projected_map3 &root);
    void initMemoryCache(Projected_map3 &root);
    
    bool is_min();
    bool project_is_min(Projected&);
    
    bool can_prune(Projected&);
    
    virtual void project(Projected&);
    virtual void project(Projected&, tree<TNODE>::iterator&);   // use memory cache

    /* Read gsp file into transaction
     * Input
     *  filename: the file name of gsp
     */
    void read(string filename);
    
private:
    // assignable parameters
    size_t minsup;
    size_t maxsup;
    size_t maxpat;
    size_t topk;
    string gspFilename;
    bool   doesUseMemoryBoost;
    
    // not assignable parameters
    SLGRAPHMININGTASKTYPE taskType;
    vector<Graph>   transaction;        // store whole graphs which read from gsp file.
    DFSCode         DFS_CODE;
    DFSCode         DFS_CODE_IS_MIN;
    Graph           GRAPH_IS_MIN;
    
    VectorXd y;                         // sgn(residual)
    VectorXd w;                         // weigth of graph mining, abs(residual)
    
    Rule rule;                          // current rule
    multiset<Rule> rule_cache;          // topk rule cache
    vector<Rule> entireRules;
    vector<size_t> patternMatchedResult;// the index result of classify data matched current patterns
    
    double wbias;                       // 
    double tau;                         // the absolute value of the difference between the weighted frequency
    
    Projected_map3 root;
    tree<TNODE> memoryCache;
};

#endif /* defined(__GLP__SLGspan__) */
