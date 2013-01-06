//
//  SLGspan.cpp
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
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

#include "SLGspan.h"
#include <utility>

using namespace std;

bool SLGspan::setParameters(SLGspanParameters parameters)
{
    minsup = parameters.minsup;
    maxpat = parameters.maxpat;
    n      = parameters.n;;
    topk   = parameters.topk;

    return true;
}

bool SLGspan::is_min ()
{
    if (DFS_CODE.size() == 1) return true;
    
    DFS_CODE.toGraph (GRAPH_IS_MIN);
    DFS_CODE_IS_MIN.clear ();
    
    Projected_map3 root;
    EdgeList       edges;
    
    for (int from = 0; from < GRAPH_IS_MIN.size() ; ++from)
        if (Utility::get_forward_root (GRAPH_IS_MIN, GRAPH_IS_MIN[from], edges))
            for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it)
                root[GRAPH_IS_MIN[from].label][(*it)->elabel][GRAPH_IS_MIN[(*it)->to].label].push (0, *it, 0);
    
    Projected_iterator3 fromlabel = root.begin();
    Projected_iterator2 elabel    = fromlabel->second.begin();
    Projected_iterator1 tolabel   = elabel->second.begin();
    
    DFS_CODE_IS_MIN.push (0, 1, fromlabel->first, elabel->first, tolabel->first);
    
    return project_is_min (tolabel->second);
}

bool SLGspan::project_is_min (Projected &projected)
{
    const RMPath& rmpath = DFS_CODE_IS_MIN.buildRMPath ();
    int minlabel         = DFS_CODE_IS_MIN[0].fromlabel;
    int maxtoc           = DFS_CODE_IS_MIN[rmpath[0]].to;
    
    {
        Projected_map1 root;
        bool flg = false;
        int newto = 0;
        
        for (int i = (int)rmpath.size()-1; ! flg  && i >= 1; --i) {
            for (int n = 0; n < projected.size(); ++n) {
                PDFS *cur = &projected[n];
                History history (GRAPH_IS_MIN, cur);
                Edge *e = Utility::get_backward (GRAPH_IS_MIN, history[rmpath[i]], history[rmpath[0]], history);
                if (e) {
                    root[e->elabel].push (0, e, cur);
                    newto = DFS_CODE_IS_MIN[rmpath[i]].from;
                    flg = true;
                }
            }
        }
        
        if (flg) {
            Projected_iterator1 elabel = root.begin();
            DFS_CODE_IS_MIN.push (maxtoc, newto, -1, elabel->first, -1);
            if (DFS_CODE[DFS_CODE_IS_MIN.size()-1] != DFS_CODE_IS_MIN [DFS_CODE_IS_MIN.size()-1]) return false;
            return project_is_min (elabel->second);
        }
    }
    
    {
        bool flg = false;
        int newfrom = 0;
        Projected_map2 root;
        EdgeList edges;
        
        for (int n = 0; n < projected.size(); ++n) {
            PDFS *cur = &projected[n];
            History history (GRAPH_IS_MIN, cur);
            if (Utility::get_forward_pure (GRAPH_IS_MIN, history[rmpath[0]], minlabel, history, edges)) {
                flg = true;
                newfrom = maxtoc;
                for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it)
                    root[(*it)->elabel][GRAPH_IS_MIN[(*it)->to].label].push (0, *it, cur);
            }
        }
        
        for (int i = 0; ! flg && i < (int)rmpath.size(); ++i) {
            for (int n = 0; n < projected.size(); ++n) {
                PDFS *cur = &projected[n];
                History history (GRAPH_IS_MIN, cur);
                if (Utility::get_forward_rmpath (GRAPH_IS_MIN, history[rmpath[i]], minlabel, history, edges)) {
                    flg = true;
                    newfrom = DFS_CODE_IS_MIN[rmpath[i]].from;
                    for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it)
                        root[(*it)->elabel][GRAPH_IS_MIN[(*it)->to].label].push (0, *it, cur);
                }
            }
        }
        
        if (flg) {
            Projected_iterator2 elabel  = root.begin();
            Projected_iterator1 tolabel = elabel->second.begin();
            DFS_CODE_IS_MIN.push (newfrom, maxtoc + 1, -1, elabel->first, tolabel->first);
            if (DFS_CODE[DFS_CODE_IS_MIN.size()-1] != DFS_CODE_IS_MIN [DFS_CODE_IS_MIN.size()-1]) return false;
            return project_is_min (tolabel->second);
        }
    }
    
    return true;
}

istream& SLGspan::read (istream &is)
{
    Graph g;
    while (true) {
        g.read (is);
        if (g.empty()) break;
        transaction.push_back (g);
    }
    return is;
}

//bool SLGspan::can_prune (Projected &projected)
//{
//    double bound; double d; double etadiff;
//    
//    lars_bound(projected, &bound, &d, &etadiff);
//    
//    if (bound < 0.0) return true;
//    
//    // check if it is included in active set
//    if (etadiff < 0.0001){
//        return false;
//    }
//    
//    
//    // rewrite
//    if ((d < tau) && (d > 0.000001)){
//        
//        rule.loc.clear ();
//        int oid = 0xffffffff;
//        for (Projected::iterator it = projected.begin(); it != projected.end(); ++it) {
//            if (oid != it->id) {
//                rule.loc.push_back (it->id);
//            }
//            oid = it->id;
//        }
//        
//        fprintf(stderr,"XXX d: %f\n",d);
//        tau       = d;
//        rule.gain = d;
//        rule.size = DFS_CODE.size();
//        
//        ostringstream ostrs;
//        DFS_CODE.write (ostrs);
//        rule.dfs = ostrs.str ();
//    }
//    
//    return false;
//}

bool SLGspan::can_prune_boost(Projected &projected)
{
    
    double gain = 0;
    double upos = 0;
    double uneg = 0;
    
    if(mode == 2 || mode ==4){ //classification
        gain = -wbias;
        upos = -wbias;
        uneg = wbias;
    }
    
    int support = 0;
    
    int oid = 0xffffffff;
    for (Projected::iterator it = projected.begin(); it != projected.end(); ++it) {
        if (oid != it->id) {
            ++support;
            if(mode == 1 || mode ==3){ //regression
                gain += 1 * y[it->id] * w[it->id];
                if (y[it->id] > 0) upos += 1 * w[it->id];
                else               uneg += 1 * w[it->id];
            }else{
                if(mode == 2 || mode == 4){ //classification
                    gain += 2 * y[it->id] * w[it->id];
                    if (y[it->id] > 0) upos += 2 * w[it->id];
                    else               uneg += 2 * w[it->id];
                }
            }
        }
        oid = it->id;
    }
    double g = fabs (gain);
    
    if (support < minsup || max (upos, uneg) <= tau) return true;
    
    if (g > tau || (g == tau && DFS_CODE.size() < rule.size)) {
        rule.gain = gain;
        rule.size = DFS_CODE.size();
        
        ostringstream ostrs;
        DFS_CODE.write (ostrs);
        rule.dfs = ostrs.str();
        
        rule.loc.clear ();
        int oid = 0xffffffff;
        for (Projected::iterator it = projected.begin(); it != projected.end(); ++it) {
            if (oid != it->id) { // remember used location using mask "oid"
                rule.loc.push_back(it->id);
            }
            oid = it->id;
        }
        if((mode == 1)||(mode == 2)){ // single tau
            tau = g;
        }else{
            if((mode == 3)||(mode == 4)){ // multi tau
                rule_cache.insert (rule);
                if(rule_cache.size() > topk){
                    set <Rule>::iterator it = rule_cache.begin();
                    rule_cache.erase(it);
                    ++it;
                    tau = fabs(it->gain);
                }
            }
        }
    }
    
    return false;
}

//project without restart for test phase

void SLGspan::project (Projected &projected)
{
    //    if (mode == CLASSIFY) {
    
    ostringstream ostrs;
    DFS_CODE.write (ostrs);
    
    // each rule in model without beta value
    string s = ostrs.str();
    int id = doubleArray.exactMatchSearch (s.c_str());
    
    if (id == -2) return;
    
    if (maxpat == DFS_CODE.size()) return;
    
    const RMPath &rmpath = DFS_CODE.buildRMPath ();
    int minlabel         = DFS_CODE[0].fromlabel;
    int maxtoc           = DFS_CODE[rmpath[0]].to;
    
    Projected_map3 new_fwd_root;
    Projected_map2 new_bck_root;
    EdgeList edges;
    
    for (int n = 0; n < projected.size(); ++n) {
        
        int id = projected[n].id;
        
        PDFS *cur = &projected[n];
        
        History history (transaction[id], cur);
        
        // backward
        for (int i = (int)rmpath.size()-1; i >= 1; --i) {
            Edge *e = Utility::get_backward (transaction[id], history[rmpath[i]], history[rmpath[0]], history);
            if (e) new_bck_root[DFS_CODE[rmpath[i]].from][e->elabel].push (id, e, cur);
        }
        
        // pure forward
        if (Utility::get_forward_pure (transaction[id], history[rmpath[0]], minlabel,
                              history, edges)) {
            for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it) {
                new_fwd_root[maxtoc][(*it)->elabel][transaction[id][(*it)->to].label].push (id, *it, cur);
            }
        }
        
        // backtracked forward
        for (int i = 0; i < (int)rmpath.size(); ++i) {
            if (Utility::get_forward_rmpath (transaction[id], history[rmpath[i]], minlabel, history, edges)) {
                for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it)
                    new_fwd_root[DFS_CODE[rmpath[i]].from][(*it)->elabel][transaction[id][(*it)->to].label].push (id, *it, cur);
            }
        }
    }
    
    // backward (recursive)
    for (Projected_iterator2 to = new_bck_root.begin(); to != new_bck_root.end(); ++to) {
        for (Projected_iterator1 elabel = to->second.begin(); elabel != to->second.end(); ++elabel) {
            DFS_CODE.push (maxtoc, to->first, -1, elabel->first, -1);
            project (elabel->second);
            DFS_CODE.pop();
        }
    }
    
    // forward
    for (Projected_riterator3 from = new_fwd_root.rbegin(); from != new_fwd_root.rend(); ++from) {
        for (Projected_iterator2 elabel = from->second.begin(); elabel != from->second.end(); ++elabel) {
            for (Projected_iterator1 tolabel = elabel->second.begin();
                 tolabel != elabel->second.end(); ++tolabel) {
                DFS_CODE.push (from->first, maxtoc+1, -1, elabel->first, tolabel->first);
                project (tolabel->second);
                DFS_CODE.pop ();
            }
        }
    }
    
    return;
}

//project with restart for training phase

void SLGspan::project (Projected &projected, tree<TNODE>::iterator &tnode)
{
    if (! is_min ()) {
        return;
    }
    
    if (maxpat == DFS_CODE.size()){
        return;
    }
    
    int support = 0;
    int oid = 0xffffffff;
    for (Projected::iterator it = projected.begin(); it != projected.end(); ++it) {
        if (oid != it->id)
            ++support;
    }
    if (support < minsup){
        return;
    }
    
    //    }
    
    bool p = false;
    if (tasktype == 0){
        p = can_prune(projected);
    }else{
        p = can_prune_boost(projected,tasktype);
    }
    
    if (p && (tnode->id ==1)){
        return;}
    
    tree<TNODE>::iterator child;
    
    if (!p && (tnode->id == 1)){
        //      printf("*");
        tnode->id = 0;
        child = tnode;  // not making a child
    }
    else{
        //      printf(".");
        TNODE tn;
        
        tn.id = 0;
        tn.dfscode.clear();
        for (vector<DFS>::iterator it = DFS_CODE.begin() ; it != DFS_CODE.end() ; ++it){
            tn.dfscode.push_back(*it);
        }
        
        tn.projected.clear();
        for (Projected::iterator it=projected.begin() ; it != projected.end() ; ++it){
            tn.projected.push(it->id, it->edge, it->prev);
        }
        
        if (tnode != memCache.begin())
            child = memCache.append_child(tnode,tn);
        else
            child = memCache.insert(tnode,tn);
        
        if (p){
            child->id = 1;
            return; // Make mounting node and quit
        }
    }
    
    const RMPath &rmpath = DFS_CODE.buildRMPath ();
    int minlabel         = DFS_CODE[0].fromlabel;
    int maxtoc           = DFS_CODE[rmpath[0]].to;
    
    Projected_map3 new_fwd_root;
    Projected_map2 new_bck_root;
    EdgeList edges;
    
    //printf("%d %d %d\n",projected.size(), child->projected.size(), child->id);
    
    for (int n = 0; n < child->projected.size(); ++n) {
        
        int id = child->projected[n].id;
        
        PDFS *cur = &(child->projected[n]);
        
        History history (transaction[id], cur);
        
        // backward
        for (int i = (int)rmpath.size()-1; i >= 1; --i) {
            Edge *e = Utility::get_backward (transaction[id], history[rmpath[i]], history[rmpath[0]], history);
            if (e) new_bck_root[DFS_CODE[rmpath[i]].from][e->elabel].push (id, e, cur);
        }
        
        // pure forward
        if (Utility::get_forward_pure (transaction[id], history[rmpath[0]], minlabel, history, edges)) {
            for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it) {
                new_fwd_root[maxtoc][(*it)->elabel][transaction[id][(*it)->to].label].push (id, *it, cur);
            }
        }
        
        // backtracked forward
        for (int i = 0; i < (int)rmpath.size(); ++i) {
            if (Utility::get_forward_rmpath (transaction[id], history[rmpath[i]], minlabel, history, edges)) {
                for (EdgeList::iterator it = edges.begin(); it != edges.end();  ++it)
                    new_fwd_root[DFS_CODE[rmpath[i]].from][(*it)->elabel][transaction[id][(*it)->to].label].push (id, *it, cur);
            }
        }
    }
    
    // backward (recursive)
    for (Projected_iterator2 to = new_bck_root.begin(); to != new_bck_root.end(); ++to) {
        for (Projected_iterator1 elabel = to->second.begin(); elabel != to->second.end(); ++elabel) {
            DFS_CODE.push (maxtoc, to->first, -1, elabel->first, -1);
            
            project (elabel->second, child);
            DFS_CODE.pop();
        }
    }
    
    // forward
    for (Projected_riterator3 from = new_fwd_root.rbegin(); from != new_fwd_root.rend(); ++from) {
        for (Projected_iterator2 elabel = from->second.begin(); elabel != from->second.end(); ++elabel) {
            for (Projected_iterator1 tolabel = elabel->second.begin();
                 tolabel != elabel->second.end(); ++tolabel) {
                DFS_CODE.push (from->first, maxtoc+1, -1, elabel->first, tolabel->first);
                project (tolabel->second, child);
                DFS_CODE.pop ();
            }
        }
    }
    
    //printf("---\n");
    
    return;
}

//  double gSpan::calc_gain (const vector <int> &loc)
//    {
//      double gain = - wbias;
//      for (int i = 0; i < loc.size(); ++i)
//        gain += 2 * y[loc[i]] * w[loc[i]];
//      return gain;
//    }

MatrixXd& SLGspan::search()
{
    tasktype = _tasktype;
    size_t l   = transaction.size();
    
    if (tasktype == 0){ // LARS
        tau              = 1000000.0;
        wbias            = 0.0;
    }else{
        if ((tasktype == 1)||(tasktype == 3)) {// Boost regression
            tau = 0.0;
            y.resize (l);
            w.resize(l);
            
            for (int i = 0; i < l; ++i) {
                w[i] = fabs(uw[i]);
                if (uw[i] < 0){
                    y[i] = -1;
                }
                else{
                    y[i] = 1;
                }
            }
        }else{
            if ((tasktype == 2)||(tasktype == 4)){// Boost classification
                tau = 0.0;
                y.resize (l);
                w.resize(l);
                
                for (int i = 0; i < l; ++i) {
                    w[i] = fabs(uw[i]);
                }
            }
        }
    }
    
    // Searching in the ready-built tree
    vector<tree<TNODE>::pre_order_iterator> mount;
    mount.clear();
    
    tree<TNODE>::pre_order_iterator pre=memCache.begin();
    while(pre != memCache.end()){
        
        DFS_CODE.clear();
        for (vector<DFS>::iterator dit = pre->dfscode.begin() ; dit != pre->dfscode.end() ; dit++){
            DFS_CODE.push_back(*dit);
        }
        
        bool p=false;
        if (tasktype == 0){ // LARS
            p = can_prune(pre->projected);
        }else{
            p = can_prune_boost(pre->projected,tasktype);
        }
        
        if ((pre->id == 1) && !p){
            mount.push_back(pre);
        }
        pre++;
    }
    
    for (vector<tree<TNODE>::pre_order_iterator>::iterator it = mount.begin() ; it != mount.end() ; ++it){
        
        tree<TNODE>::pre_order_iterator pre2;
        pre2 = *it;
        DFS_CODE.clear();
        for (vector<DFS>::iterator dit = pre2->dfscode.begin() ; dit != pre2->dfscode.end() ; dit++){
            DFS_CODE.push_back(*dit);
        }
        project(pre2->projected,pre2);     // id=1;
    }
    
    return true;
}
