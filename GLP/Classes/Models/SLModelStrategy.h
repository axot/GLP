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
#include <Eigen/Core>
#include "../SLUtility.h"

using namespace std;
using namespace Eigen;

enum{
    SLGLPRESULTYPENONE    = 0,
    SLGLPRESULTYPEQ2      = 1 << 0,
    SLGLPRESULTYPERSS     = 1 << 1,
    SLGLPRESULTYPEAUC     = 1 << 2,
    SLGLPRESULTYPEACC     = 1 << 3,
    SLGLPRESULTYPEBETA    = 1 << 4
};

typedef unsigned int SLGLPRESULTYPE;
typedef map<SLGLPRESULTYPE, MatrixXd> SLGlpResult;

enum{
    SLGLPCROSSVALIDATIONRESULTYPETRAIN      = 1 << 0,
    SLGLPCROSSVALIDATIONRESULTYPEVALIDATION = 1 << 1,
    SLGLPCROSSVALIDATIONRESULTYPETEST       = 1 << 2,
};

typedef unsigned int SLGLPCROSSVALIDATIONRESULTYPE;
typedef map<SLGLPCROSSVALIDATIONRESULTYPE, SLGlpResult> SLGlpCrossValidationResults;

class SLModelStrategy
{
public:
    SLModelStrategy()
    {
        SLModelStrategy::SLModelParameters param;
        initParameters(param);
    }
    
    class SLModelParameters
    {
    public:
        SLModelParameters() : kFold(10) {}
        
    public:
        int kFold;
    };
    
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
    
    /* K-fold Cross Vaildate:
     * Input
     *      X: X matrix
     *      Y: Y matrix
     *   type: type of results
     *
     * Return: the results stored in mapped structure.
     *
     */
    virtual SLGlpCrossValidationResults crossValidation(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type) const;
    
    /* Init Parameters:
     * Input
     *      parameters: Model assignable parameters
     *
     * Return: true if sucessed.
     *
     */
    template <typename MP>
    bool initParameters(MP parameters)
    {
        kFold = parameters.kFold;
        return true;
    };
    
protected:
    // assignable parameters via initParameters() method
    int kFold;
    
    // not assignable parameters
    mutable VectorXi randomIndexs;
};

#endif
