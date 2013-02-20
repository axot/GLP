//
//  SLSparsePls.cpp
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
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
#include "SLSparsePls.h"

using namespace std;

// Public Methods
SLModelResult SLSparsePls::train(const MatrixXd& appendedX, const MatrixXd& theY, SLMODELRESULTYPE type)
{
    if (Y.cols() == 0 || Y.rows() == 0)
    {
        Y = theY;
        Res = Y;
    }
    
    ssize_t appendedXRows = appendedX.rows();
    ssize_t appendedXCols = appendedX.cols();
    ssize_t oldXCols      = X.cols();
    
    ssize_t maxSquaredNormColumn;
    SSum(Res).maxCoeff(&maxSquaredNormColumn);
    VectorXd largestResCol = Res.col(maxSquaredNormColumn);

    X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
    X.rightCols(appendedXCols).setZero();
    X << X.leftCols(oldXCols), appendedX;
    
    W.conservativeResize(oldXCols+appendedXCols, W.cols()+1);
    W.bottomRows(appendedXCols).setZero();
    W.rightCols(1).setZero();
    
    W.rightCols(1) = X.transpose()*largestResCol;
    W.rightCols(1).normalize();
        
    T.conservativeResize(appendedXRows, T.cols()+1);
    T.rightCols(1).setZero();
    
    if(T.cols() > 1)
    {
        size_t oldTCols = (size_t)(T.cols()-1);
        T.rightCols(1) = (MatrixXd::Identity(X.rows(),X.rows()) - T.leftCols(oldTCols)*(T.leftCols(oldTCols).transpose())) * X * W.rightCols(1);
    }
    else
        T.rightCols(1) = X * W.rightCols(1);
    
    T.rightCols(1).normalize();
    
    Beta = W*(W.transpose()*X.transpose()*X*W).householderQr().solve(W.transpose()*X.transpose()*Y);
    
    if (verbose)
    {
        LOG(Y);
        LOG(X.rightCols(appendedXCols));
        LOG(W);
        LOG(largestResCol);
        LOG(Beta);
        LOG(X*Beta.col(0));
        getchar();
    }
    Res = Y - X*Beta;
    
    return getTrainResult(type);
}

SLModelResult SLSparsePls::classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const
{
    ASSERT(type != SLModelResultTypeNone, "No type of result was indicated");
    
    ASSERT(!(type & ~(SLModelResultTypeBeta | SLModelResultTypeQ2 | SLModelResultTypeRSS | SLModelResultTypeACC)),
           "Only support Beta Q2 RSS ACC for Sparse PLS.");
    
    ASSERT(Beta.cols() != 0 && Beta.rows() != 0, "Train data first");
    
    SLModelResult result;
    
    MatrixXd tRES = tY - tX*Beta;

    if(type & SLModelResultTypeQ2)
    {
        result[SLModelResultTypeQ2] = MatrixXd::Ones(1, tY.cols()) - SSum(tRES).cwiseQuotient(SSum(tY));
    }
    
    if(type & SLModelResultTypeRSS)
    {
        result[SLModelResultTypeRSS] = SSum(tRES);
    }
    
    if(type & SLModelResultTypeBeta)
    {
        result[SLModelResultTypeBeta] = Beta;
    }
    
    if(type & SLModelResultTypeACC)
    {
        result[SLModelResultTypeACC] = calcACC(tX, tY);
    }
    return result;
}

bool SLSparsePls::setParameters(SLSparsePlsParameters& parameters)
{
    verbose = parameters.verbose;
    param = parameters;
    return true;
}

// Private Methods
SLModelResult SLSparsePls::getTrainResult(SLMODELRESULTYPE type) const
{
    ASSERT(!(type & ~(SLModelResultTypeBeta | SLModelResultTypeQ2 | SLModelResultTypeRSS | SLModelResultTypeACC)),
           "Only support Beta Residual Q2 RSS ACC for Sparse PLS.");
    
    ASSERT(Beta.cols() != 0 && Beta.rows() != 0, "Train data first");
    
    SLModelResult result;
    if(type & SLModelResultTypeQ2)
    {
        result[SLModelResultTypeQ2] = MatrixXd::Ones(1, Y.cols()) - SSum(Res).cwiseQuotient(SSum(Y));
    }
    
    if(type & SLModelResultTypeRSS)
    {
        result[SLModelResultTypeRSS] = SSum(Res);
    }
    
    if(type & SLModelResultTypeBeta)
    {
        result[SLModelResultTypeBeta] = Beta;
    }
    
    if(type & SLModelResultTypeACC)
    {
        result[SLModelResultTypeACC] = calcACC(X, Y);
    }
    return result;
}

MatrixXd SLSparsePls::calcACC(const MatrixXd& tX, const MatrixXd& tY) const
{
    MatrixXd predictY = tX*Beta;
    size_t correct = 0;
    MatrixXd acc(1,tY.cols());
    
    VectorXd min = tY.colwise().minCoeff();
    VectorXd max = tY.colwise().maxCoeff();
    VectorXd mid = (min + max) / 2;
    for (int i=0; i < tY.cols(); ++i)
    {
        correct = 0;
        for (int j=0; j < tY.rows(); ++j)
        {
            if (predictY(j,i) > mid[i])
            {
                if(tY(j,i) > mid[i])
                {
                    ++correct;
                }
            }
            else
            {
                if(tY(j,i) < mid[i])
                {
                    ++correct;
                }
            }
        }
        acc(0,i) = (double)correct/tY.rows();
    }
    return acc;
}
