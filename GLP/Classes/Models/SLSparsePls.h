//
//  SLSparsePls.h
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

#ifndef GLP_SLSparsePls_h
#define GLP_SLSparsePls_h

#include <iostream>
#include <Eigen/Core>
#include "SLModelStrategy.h"

using namespace Eigen;

class SLSparsePls : public SLModelStrategy
{
public:
    class SLSparsePlsParameters : public SLModelStrategy::SLModelParameters
    {
    public:
        SLSparsePlsParameters() {}
        
    public:
    };
    
public:
    SLSparsePls() : verbose(false) {}
        
    /* Train: Sparse PLS regression: Y = XB
     * Input
     *      X: the X matrix which need be appended
     *      Y: assignable when the inside Y did not be initialized.
     *   type: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLGlpResult train(const MatrixXd& appendedX, const MatrixXd& theY, SLGLPRESULTYPE type);
    
    /* Classify:
     * Input
     *     tX: X matrix of test data
     *     tY: Y matrix of test data
     *   type: type of results
     *
     * Return: the results stored in mapped structure
     */
    virtual SLGlpResult classify(const MatrixXd& tX, const MatrixXd& tY, SLGLPRESULTYPE type) const;
    
    /* Init Parameters:
     * Input
     *      parameters: Model assignable parameters
     *
     * Return: true if sucessed.
     *
     * Discussion: no optional parameters for Sparse PLS
     */
    bool initParameters(SLSparsePlsParameters parameters);
    
private:
    SLGlpResult getTrainResult(SLGLPRESULTYPE type) const;

private:
    // assignable parameters via initParameters() method
    
    // not assignable parameters
    bool verbose;
    MatrixXd X;
    MatrixXd Y;
    MatrixXd meanX;
    MatrixXd meanY;
    MatrixXd T;
    MatrixXd Beta;
    MatrixXd RES;
    VectorXd residual;
    MatrixXd W;
};

#endif
