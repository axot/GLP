//
//  SLNipals.cpp
//  GLP
//
//  Created by Zheng Shao on 4/14/14.
//  Copyright (c) 2014 Saigo Laboratoire. All rights reserved.
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

#include <Eigen/QR>
#include "../SLUtility.h"
#include "SLNipals.h"

using namespace std;

// Public Methods
SLModelResult SLNipals::train(const MatrixXd& theX, const MatrixXd& theY, SLMODELRESULTYPE type)
{
    if (Y.cols() == 0 || Y.rows() == 0)
    {
        Y = theY;
        Res = Y;
    }
    if (X.cols() == 0 || X.rows() == 0)
    {
        X = theX;
    }
    
    ssize_t maxSquaredNormColumn;
    ColSSum(Res).maxCoeff(&maxSquaredNormColumn);
    VectorXd largestYCol = Res.col(maxSquaredNormColumn);
    
    W.conservativeResize(X.cols(), W.cols()+1);
    W.rightCols(1).setZero();
    
    W.rightCols(1) = X.transpose()*largestYCol;
    W.rightCols(1).normalize();

    T.conservativeResize(X.rows(), T.cols()+1);
    T.rightCols(1).setZero();
    T.rightCols(1) = X * W.rightCols(1);
    
    T.rightCols(1).normalize();
    
    Beta = W*(W.transpose()*X.transpose()*X*W).householderQr().solve(W.transpose()*X.transpose()*Y);
    Res = Y - X*Beta;
    
//    if (verbose)
//    {
//        cout << "train mode" << endl;
//        LOG(X.cols());
//        LOG(X.rows());
//        LOG(Y.cols());
//        LOG(Y.rows());
//    }
    return getTrainResult(type);
}

SLModelResult SLNipals::classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const
{
    ASSERT(type != SLModelResultTypeNone, "No type of result was indicated");
    
    ASSERT(!(type & ~(SLModelResultTypeBeta |
                      SLModelResultTypeQ2   |
                      SLModelResultTypeRSS  |
                      SLModelResultTypeACC  |
                      SLModelResultTypeAUC  |
                      SLModelResultTypeAIC  |
                      SLModelResultTypeBIC  |
                      SLModelResultTypeCOV)),
           "Only support Beta Q2 RSS ACC AUC AIC BIC COV for X-deflation PLS.");
    
    ASSERT(Beta.cols() != 0 && Beta.rows() != 0, "Train data first");
    
    SLModelResult result;
    MatrixXd tRES = tY - tX*Beta;
    if (verbose)
    {
        cout << "classify mode" << endl;
        LOG(Beta);
        LOG(tY);
        LOG(tRES);
    }

    if(type & SLModelResultTypeQ2)
    {
        result[SLModelResultTypeQ2] = getQ2(tRES, tY);
    }
    
    if(type & SLModelResultTypeRSS)
    {
        result[SLModelResultTypeRSS] = getRSS(tRES);
    }
    
    if(type & SLModelResultTypeBeta)
    {
        result[SLModelResultTypeBeta] = (MatrixXd)(Beta);
    }
    
    if(type & SLModelResultTypeACC)
    {
        result[SLModelResultTypeACC] = getACC(tY, tX*Beta);
    }
    
    if(type & SLModelResultTypeAUC)
    {
        result[SLModelResultTypeAUC] = getAUC(tY, tX*Beta);
    }
    
    if(type & SLModelResultTypeAIC)
    {
        result[SLModelResultTypeAIC] = getAIC(tY, tX*Beta, X.cols());
    }
    
    if(type & SLModelResultTypeBIC)
    {
        result[SLModelResultTypeBIC] = getBIC(tY, tX*Beta, X.cols());
    }
    
    if(type & SLModelResultTypeCOV)
    {
        result[SLModelResultTypeCOV] = getCOV(tY, tX*Beta);
    }
    return result;
}

bool SLNipals::setParameters(SLNipalsParameters& parameters)
{
    verbose = parameters.verbose;
    param = parameters;
    return true;
}

// Private Methods
SLModelResult SLNipals::getTrainResult(SLMODELRESULTYPE type) const
{
    ASSERT(!(type & ~(SLModelResultTypeBeta |
                      SLModelResultTypeQ2   |
                      SLModelResultTypeRSS  |
                      SLModelResultTypeACC  |
                      SLModelResultTypeAUC  |
                      SLModelResultTypeAIC  |
                      SLModelResultTypeBIC  |
                      SLModelResultTypeCOV)),
           "Only support Beta Q2 RSS ACC AUC AIC BIC COV for X-deflation PLS.");
    
    ASSERT(Beta.cols() != 0 && Beta.rows() != 0, "Train data first");
    
    SLModelResult result;
    if(type & SLModelResultTypeQ2)
    {
        result[SLModelResultTypeQ2] = getQ2(Res, Y);
    }
    
    if(type & SLModelResultTypeRSS)
    {
        result[SLModelResultTypeRSS] = getRSS(Res);
    }
    
    if(type & SLModelResultTypeBeta)
    {
        result[SLModelResultTypeBeta] = (MatrixXd)Beta;
    }
    
    if(type & SLModelResultTypeACC)
    {
        result[SLModelResultTypeACC] = getACC(Y, X*Beta);
    }
    
    if(type & SLModelResultTypeAUC)
    {
        result[SLModelResultTypeAUC] = getAUC(Y, X*Beta);
    }
    
    if(type & SLModelResultTypeAIC)
    {
        result[SLModelResultTypeAIC] = getAIC(Y, X*Beta, X.cols());
    }
    
    if(type & SLModelResultTypeBIC)
    {
        result[SLModelResultTypeBIC] = getBIC(Y, X*Beta, X.cols());
    }
    
    if(type & SLModelResultTypeCOV)
    {
        result[SLModelResultTypeCOV] = getCOV(Y, X*Beta);
    }
    return result;
}
