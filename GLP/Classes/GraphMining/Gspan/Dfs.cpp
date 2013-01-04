//
//  Dfs.cpp
//  GLP
//
//  Copyright (C) 2004 Taku Kudo, All rights reserved.
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

#include "Dfs.h"

bool DFSCode::toGraph (Graph &g)
{
    g.clear ();
    
    for (DFSCode::iterator it = begin(); it != end(); ++it) {
        g.resize (std::max (it->from, it->to) + 1);
        
        if (it->fromlabel != -1) g[it->from].label = it->fromlabel;
        if (it->tolabel   != -1) g[it->to].label   = it->tolabel;
        g[it->from].push (it->from, it->to,   it->elabel);
        g[it->to].push   (it->to,   it->from, it->elabel);
    }
    
    g.buildEdge ();
    
    return true;
}

std::ostream &DFSCode::write (std::ostream &os)
{
    if (size() == 0) return os;
    
    os << "(" << (*this)[0].fromlabel << ") " << (*this)[0].elabel << " (0f" << (*this)[0].tolabel << ")";
    
    for (unsigned int i = 1; i < size(); ++i) {
        if ((*this)[i].from < (*this)[i].to) {
            os << " " << (*this)[i].elabel << " (" << (*this)[i].from << "f" << (*this)[i].tolabel << ")";
        } else {
            os << " " << (*this)[i].elabel << " (b" << (*this)[i].to << ")";
        }
    }
    
    return os;
}

const RMPath &DFSCode::buildRMPath ()
{
    rmpath.clear ();
    
    int old_from = -1;
    
    for (int i = (int)size()-1; i >= 0; --i) {
        if ((*this)[i].from < (*this)[i].to && // forward
            (rmpath.empty() || old_from == (*this)[i].to)) {
            rmpath.push_back (i);
            old_from = (*this)[i].from;
        }
    }
    
    return rmpath;
}

void History::build (Graph &graph, PDFS *e)
{
    // first build history
    clear ();
    edge.clear ();   edge.resize   (graph.edge_size());
    vertex.clear (); vertex.resize (graph.size());
    
    if (e) {
        push_back (e->edge);
        edge  [e->edge->id] = vertex[e->edge->from] =  vertex[e->edge->to]   = 1;
        for (PDFS *p = e->prev; p ; p = p->prev) {
            push_back (p->edge);
            edge  [p->edge->id] = vertex[p->edge->from] =  vertex[p->edge->to]   = 1;
        }
        std::reverse (begin(), end());
    }
}
