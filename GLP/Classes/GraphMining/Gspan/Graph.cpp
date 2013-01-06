//
//  Graph.cpp
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

#include <strstream>
#include <map>
#include <set>
#include <iterator>
#include "Graph.h"

using namespace std;

template <class T, class Iterator>
void tokenize (const char *str, Iterator iterator)
{
    istrstream is (str, strlen(str));
    copy (istream_iterator <T> (is), istream_iterator <T> (), iterator);
}

void Graph::buildEdge ()
{
    char buf[512];
    map <string, unsigned int> tmp;
    
    unsigned int id = 0;
    for (int from = 0; from < (int)size (); ++from) {
        for (Vertex::edge_iterator it = (*this)[from].edge.begin ();
             it != (*this)[from].edge.end (); ++it) {
            if (from <= it->to) sprintf (buf, "%d %d %d", from,     it->to,   it->elabel);
            else                sprintf (buf, "%d %d %d", it->to,   from,     it->elabel);
            if (tmp.find (buf) == tmp.end()) {
                it->id = id;
                tmp[buf] = id;
                ++id;
            } else {
                it->id = tmp[buf];
            }
        }
    }
    
    edge_size_ = id;
}

istream &Graph::read (istream &is)
{
    vector <string> result;
    char line[1024];
    
    clear ();
    
    while (true) {
        
        size_t pos = is.tellg ();
        if (! is.getline (line, 1024)) break;
        result.clear ();
        tokenize<string>(line, back_inserter (result));
        
        if (result.empty()) {
            // do nothing
        } else if (result[0] == "t") {
            if (! empty()) { // use as delimiter
                is.seekg (pos, ios_base::beg);
                break;
            } else {
                value = atof(result[3].c_str());
            }
        } else if (result[0] == "v" && result.size() >= 3) {
            unsigned int id    = atoi (result[1].c_str());
            this->resize (id + 1);
            (*this)[id].label = atoi (result[2].c_str());
        } else if (result[0] == "e" && result.size() >= 4) {
            int from   = atoi (result[1].c_str());
            int to     = atoi (result[2].c_str());
            int elabel = atoi (result[3].c_str());
            
            if ((int)size () <= from || (int)size () <= to) {
                cerr << "Fromat Error:  define vertex lists before edges" << endl;
                exit (-1);
            }
            
            (*this)[from].push (from, to,   elabel);
            (*this)[to].push   (to,   from, elabel);
        }
    }
    
    buildEdge ();
    
    return is;
}

ostream &Graph::write (ostream &os)
{
    char buf[512];
    set <string> tmp;
    
    for (int from = 0; from < (int)size (); ++from) {
        os << "v " << from << " " << (*this)[from].label << endl;
        
        for (Vertex::edge_iterator it = (*this)[from].edge.begin ();
             it != (*this)[from].edge.end (); ++it) {
            if (from <= it->to) {
                sprintf (buf, "%d %d %d", from, it->to,   it->elabel);
            } else {
                sprintf (buf, "%d %d %d", it->to,   from, it->elabel);
            }
            tmp.insert (buf);
        }
    }
    
    for (set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        os << "e " << *it << endl;
    }
    
    return os;
}
