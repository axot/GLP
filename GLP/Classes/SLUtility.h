//
//  SLUtility.h
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
#ifndef __GLP__SLUtility__
#define __GLP__SLUtility__

#include "Extensions/MatrixExt.h"

#define LOG(x)          cout << #x << ":\n" << x << '\n' << endl;
#define SSum(x)         x.colwise().squaredNorm()
#define Center(x)       EigenExt::centering(x)

#define ASSERT(condition, message) \
do { \
    if (! (condition)) { \
        std::cerr << "Assertion failed `" #condition "`, file " << __FILE__ \
                  << ", line " << __LINE__ << ": " << message << std::endl; \
        std::exit(EXIT_FAILURE); \
    } \
}while (false)

class SLUtility
{
public:
    static bool isIncludedOnlyOneType(size_t type)
    {
        if ( type == 0 )
        {
            return false;
        }
        else
        {
            while(true)
            {
                if ( type&1 )
                {
                    if ( type == 1 )
                        return true;
                    else
                        return false;
                }
                type >>= 1;
            }
        }
        return false;
    }
};

#endif
