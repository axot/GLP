//
//  SLCrossValidationResultHistory.h
//  GLP
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

#ifndef __GLP__SLCrossValidationResultHistory__
#define __GLP__SLCrossValidationResultHistory__

#include <deque>

using namespace std;

class SLCrossValidationResultHistory : public deque<SLCrossValidationResults>
{
public:
    void push_front(const SLCrossValidationResults& val)
    {
        deque<SLCrossValidationResults>::push_front(val);
        while ( this->size() > resultHistorySize+1 )
        {
            this->pop_back();
        }
    }
    
public:
    size_t resultHistorySize;
};

#endif
