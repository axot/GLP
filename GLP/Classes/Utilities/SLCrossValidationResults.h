//
//  SLCrossValidationResults.h
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

#ifndef GLP_SLCrossValidationResults_h
#define GLP_SLCrossValidationResults_h

#include <map>
#include <iostream>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

enum{
    SLCROSSVALIDATIONRESULTYPETRAIN      = 1 << 0,
    SLCROSSVALIDATIONRESULTYPEVALIDATION = 1 << 1,
    SLCROSSVALIDATIONRESULTYPETEST       = 1 << 2,
};

typedef unsigned int SLCROSSVALIDATIONRESULTYPE;
typedef vector<SLGlpResult> SLGlpMultipleResults;

class SLCrossValidationResults : public map<SLCROSSVALIDATIONRESULTYPE, SLGlpMultipleResults>
{
public:
    SLCrossValidationResults operator+(SLCrossValidationResults& b)
    {
        if ( this->size() == 0 && b.size() == 0 )
        {
            return *this;
        }
        
        SLCrossValidationResults result;
        SLCrossValidationResults::iterator it;
        if ( b.size() == 0 )
        {
            return *this;
        }
        else if ( this->size() == 0 )
        {
            (*this) = b;
            return *this;
        }
        else {
            for ( it = this->begin(); it != this->end(); ++it )
            {
                SLCROSSVALIDATIONRESULTYPE key = it->first;
                SLGlpMultipleResults value = it->second;
                
                result[key] = value;
                result[key].insert(result[key].end(), b[key].begin(), b[key].end());
            }
        }
        return result;
    }
    
    SLCrossValidationResults& operator+=(SLCrossValidationResults& b)
    {
        *this = *this + b;
        return *this;
    }
    
    string print(SLCROSSVALIDATIONRESULTYPE cvType, SLGLPRESULTYPE resultType)
    {
        stringstream output;
        SLGlpMultipleResults::iterator it;
        size_t i = 0;
        for ( it = (*this)[cvType].begin(); it != (*this)[cvType].end(); ++it )
        {
            output << "\n[" << i << "]:"  << endl;
            output << (*it)[resultType]   << endl;
            ++i;
        }
        return output.str();
    }
    
    double mean(SLCROSSVALIDATIONRESULTYPE cvType, SLGLPRESULTYPE resultType)
    {
        SLGlpMultipleResults::iterator it = (*this)[cvType].begin();
        
        MatrixXd sum = (*it)[resultType];
        ++it;
        while ( it != (*this)[cvType].end() )
        {
            sum += (*it)[resultType];
            ++it;
        }
        return sum.mean()/(*this)[cvType].size();
    }
};

#endif
