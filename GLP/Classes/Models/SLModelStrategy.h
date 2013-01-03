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

#ifndef GLP_SLModelStrategy_h
#define GLP_SLModelStrategy_h

#include <iostream>
#include <map>
#include <vector>
#include <Eigen/Core>
#include "../SLUtility.h"

using namespace std;
using namespace Eigen;

enum{
    SLGLPRESULTYPENONE    = 0,
    SLGLPRESULTYPEBETA    = 1 << 0,
    SLGLPRESULTYPEQ2      = 1 << 1,
    SLGLPRESULTYPERSS     = 1 << 2,
    SLGLPRESULTYPEAUC     = 1 << 3,
    SLGLPRESULTYPEACC     = 1 << 4,
    SLGLPRESULTYPEAIC     = 1 << 5,
    SLGLPRESULTYPEBIC     = 1 << 6,
};

typedef unsigned int SLGLPRESULTYPE;
typedef map<SLGLPRESULTYPE, MatrixXd> SLGlpResult;

class SLModelStrategy
{
public:    
    /* Train:
     * Input
     *      X: X matrix of train data
     *      Y: Y matrix of train data
     *   type: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLGlpResult train(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type) = 0;
        
    /* Classify:
     * Input
     *     tX: X matrix of test data
     *     tY: Y matrix of test data
     *   type: type of results
     *
     * Return: the results stored in mapped structure
     */
    virtual SLGlpResult classify(const MatrixXd& tX, const MatrixXd& tY, SLGLPRESULTYPE type) const = 0;
        
    /* Set Parameters:
     * Input
     *      parameters: Model assignable parameters
     *
     * Return: true if sucessed.
     *
     */
    template <typename MP>
    bool setParameters(MP parameters) { return true; };
    
    /* Get Parameters:
     *
     *      Return: current parameters.
     *  Discussion: default return nothing, for cross validation you have to override this method.
     */
    template <typename MP>
    MP getParameters() { MP mp; return mp; }
};

#endif
