//
//  Utility.h
//  GLP
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

#ifndef GLP_Gspan_Utility_h
#define GLP_Gspan_Utility_h

#include <iostream>
#include <functional>
#include <utility>
#include "Graph.h"
#include "Dfs.h"

using namespace std;

class Utility
{
public:
    static bool  get_forward_pure   (Graph&, Edge *, int, History&, EdgeList &);
    static bool  get_forward_rmpath (Graph&, Edge *, int, History&, EdgeList &);
    static bool  get_forward_root   (Graph&, Vertex&, EdgeList &);
    static Edge* get_backward       (Graph&, Edge *, Edge *, History&);
};

#endif /* defined(__GLP__Utility__) */
