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
    class SLSparsePlsParameters
    {
    public:
        SLSparsePlsParameters() {}
        
    public:
    };
    
public:
    SLSparsePls() : verbose(0) {}
        
    /* Train: Sparse PLS regression: Y = XB
     * Input
     *      X: the X matrix which need be appended
     *      Y: only assign when the inside Y did not be initialized.
     *
     * Output
     *      Beta: the calculated Beta value, if NULL passed, nothing will be assigned
     *
     * Return: true if sucessed.
     */
    bool train   (MatrixXd& X, MatrixXd& Y, MatrixXd** Beta);
    bool validate(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta);
    bool classify(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta);
    bool initParameters(SLSparsePlsParameters parameters);
    
    /* getTrainResult:
     * Input
     *      type: type of results
     *
     * Return: the results stored in mapped structure, support Q2 and RSS.
     */
    map<SLTRAINRESULTYPE, MatrixXd> getTrainResult(SLTRAINRESULTYPE type);

private:
    // assignable parameters via initParameters() method
    
    // not assignable parameters
    ssize_t verbose;
    MatrixXd X;
    MatrixXd Y;
    MatrixXd T;
    MatrixXd Beta;
    MatrixXd RES;
    VectorXd residual;
    MatrixXd W;
};

#endif
