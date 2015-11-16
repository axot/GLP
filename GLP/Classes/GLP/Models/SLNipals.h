//
//  SLNipals.h
//  GLP
//
//  Created by Zheng Shao on 4/14/14.
//  Copyright (c) 2014 Saigo Laboratoire. All rights reserved.
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

#ifndef __GLP__SLNipals__
#define __GLP__SLNipals__

#include <iostream>
#include <cstdio>
#include "SLModelStrategy.h"
#include "SLModel.h"

class SLNipals : public SLModelStrategy
{
public:
    class SLNipalsParameters
    {
    public:
        SLNipalsParameters() : verbose(false) {}
        
    public:
        bool verbose;
    };
    
public:
    SLNipals() : verbose(false) {}
    
    /* Train: X-deflation PLS: Y = XB
     * Input
     *      X: the X matrix
     *      Y: assignable when the inside Y did not be initialized.
     *   type: type of results
     *
     * Return: the results stored in mapped structure.
     */
    virtual SLModelResult train(const MatrixXd& X, const MatrixXd& theY, SLMODELRESULTYPE type);
    
    /* Classify:
     * Input
     *     tX: X matrix of test data
     *     tY: Y matrix of test data
     *   type: type of results
     *
     * Return: the results stored in mapped structure
     */
    virtual SLModelResult classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const;
    
    /* Set Parameters:
     * Input
     *      parameters: Model assignable parameters
     *
     * Return: true if sucessed.
     *
     * Discussion: no optional parameters for X-deflation PLS
     */
    bool setParameters(SLNipalsParameters& parameters);
    
    /* Get Parameters:
     *
     * Return: current parameters.
     */
    SLNipalsParameters getParameters() const { return param; }
    
private:
    SLModelResult getTrainResult(SLMODELRESULTYPE type) const;
    
private:
    // assignable parameters via setParameters() method
    bool verbose;

    // not assignable parameters
    SLNipalsParameters param;
    MatrixXd X;
    MatrixXd Y;
    MatrixXd T;
    MatrixXd Beta;
    MatrixXd Res;
    MatrixXd W;
};

#endif /* defined(__GLP__SLNipals__) */
