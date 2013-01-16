//
//  SLGraphMiningStrategy.h
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

#ifndef __GLP__SLGraphMiningStrategy__
#define __GLP__SLGraphMiningStrategy__

#include <iostream>
#include <map>
#include <Eigen/Core>
#include "../SLUtility.h"

using namespace std;
using namespace Eigen;

enum{
    SLGRAPHMININGRESULTYPENONE  = 0,
    SLGRAPHMININGRESULTYPEX     = 1 << 0,
};

typedef unsigned int SLGRAPHMININGRESULTYPE;
typedef map<SLGRAPHMININGRESULTYPE, MatrixXd> SLGraphMiningResult;

enum{
    SLGRAPHMININGTASKTYPENOTDEFINED   = 0,
    SLGRAPHMININGTASKTYPETRAIN        = 1 << 0,
    SLGRAPHMININGTASKTYPECLASSIFY     = 1 << 1,
};

typedef unsigned int SLGRAPHMININGTASKTYPE;

enum{
    SLGRAPHMININGINNERVALUENOTDEFINED   = 0,
    SLGRAPHMININGINNERVALUEY            = 1 << 0,
};

typedef unsigned int SLGRAPHMININGINNERVALUE;
typedef map<SLGRAPHMININGINNERVALUE, MatrixXd> SLGraphMiningInnerValues;

class SLGraphMiningStrategy
{    
public:
    /* Search substructure
     * Input
     *      residual: the residual of Y
     *      taskType: must be specified either train or classify
     *    resultType: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLGraphMiningResult search(VectorXd residual,
                                       SLGRAPHMININGTASKTYPE taskType,
                                       SLGRAPHMININGRESULTYPE resultType) = 0;
    
    /* Get Inner Values
     * Input
     *    type: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLGraphMiningInnerValues getInnerValues(SLGRAPHMININGINNERVALUE type) const = 0;

    /* Set Parameters:
     * Input
     *      parameters: Graph Mining assignable parameters
     *
     * Return: true if sucessed.
     */
    template <typename GP>
    bool setParameters(GP& parameters) { return true; };
};

#endif /* defined(__GLP__SLGraphMiningStrategy__) */
