//
//  SLModelStrategy.h
//  GLP
//
//  Created by Zheng Shao on 12/20/12.
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

#ifndef __GLP__SLModelStrategy__
#define __GLP__SLModelStrategy__

#include <iostream>
#include <map>
#include <vector>
#include <Eigen/Core>
#include <boost/variant.hpp>
#include "../SLUtility.h"

using namespace std;
using namespace Eigen;

enum{
    SLModelResultTypeNone    = 0,
    SLModelResultTypeBeta    = 1 << 0,
    SLModelResultTypeQ2      = 1 << 1,
    SLModelResultTypeRSS     = 1 << 2,
    SLModelResultTypeAUC     = 1 << 3,
    SLModelResultTypeACC     = 1 << 4,
    SLModelResultTypeAIC     = 1 << 5,
    SLModelResultTypeBIC     = 1 << 6,
};

typedef unsigned int SLMODELRESULTYPE;
typedef map<SLMODELRESULTYPE, boost::variant< VectorXd, MatrixXd, string > > SLModelResult;

class SLModelStrategy
{
public:    
    /**
     * @brief Train stage
     * @param X X matrix of train data
     * @param Y Y matrix of train data
     * @param type type of results
     *
     * @result the results stored in mapped structure.
     */
    virtual SLModelResult train(const MatrixXd& X, const MatrixXd& Y, SLMODELRESULTYPE type) = 0;
        
    /* Classify:
     * Input
     *     tX: X matrix of test data
     *     tY: Y matrix of test data
     *   type: type of results
     *
     * Return: the results stored in mapped structure
     */
    virtual SLModelResult classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const = 0;
        
    /* Set Parameters:
     * Input
     *      parameters: Model assignable parameters
     *
     * Return: true if sucessed.
     */
    template <typename MP>
    bool setParameters(MP& parameters) { return true; };
    
    /* Get Parameters:
     *
     *      Return: current parameters.
     *  Discussion: for implementng cross validation you have to override this method.
     */
    template <typename MP>
    MP getParameters() const { MP mp; return mp; }
};

#endif
