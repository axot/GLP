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
        meanY = theY.colwise().mean();
        Y = AutoScale(theY);
        Res = Y;
    }
    
    long appendedXRows = appendedX.rows();
    long appendedXCols = appendedX.cols();
    long oldXCols      = X.cols();
    
    long maxSquaredNormColumn;
    SSum(Res).maxCoeff(&maxSquaredNormColumn);
    VectorXd largestResCol = Res.col(maxSquaredNormColumn);

    meanX.conservativeResize(1, oldXCols+appendedXCols);
    meanX << meanX.leftCols(oldXCols), appendedX.colwise().mean();
    
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
        T.rightCols(1) = (MatrixXd::Identity(X.rows(),X.rows()) - T.leftCols(T.cols()-1)*(T.leftCols(T.cols()-1).transpose())) * X * W.rightCols(1);
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
    ASSERT(type != SLMODELRESULTYPENONE, "No type of result was indicated");
    
    ASSERT(!(type & ~(SLMODELRESULTYPEBETA|SLMODELRESULTYPEQ2|SLMODELRESULTYPERSS|SLMODELRESULTYPEACC)),
           "Only support Beta Q2 RSS for Sparse PLS.");
    
    ASSERT(meanY.cols() != 0 && meanY.rows() != 0, "Train data first");
    
    SLModelResult result;
    
    MatrixXd autoSacaledY = tY - meanY.replicate(tY.rows(), 1);
    MatrixXd autoSacaledX = tX - meanX.replicate(tX.rows(), 1);

    MatrixXd tRES = autoSacaledY - autoSacaledX*Beta;

    if(type & SLMODELRESULTYPEQ2)
    {
        result[SLMODELRESULTYPEQ2] = MatrixXd::Ones(1, autoSacaledY.cols()) - SSum(tRES).cwiseQuotient(SSum(autoSacaledY));
    }
    
    if(type & SLMODELRESULTYPERSS)
    {
        result[SLMODELRESULTYPERSS] = SSum(tRES);
    }
    
    if(type & SLMODELRESULTYPEBETA)
    {
        result[SLMODELRESULTYPEBETA] = Beta;
    }
    
    if(type & SLMODELRESULTYPEACC)
    {
        result[SLMODELRESULTYPEACC] = calcACC(tX, tY);
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
    ASSERT(!(type & ~(SLMODELRESULTYPEBETA|SLMODELRESULTYPEQ2|SLMODELRESULTYPERSS|SLMODELRESULTYPEACC)),
           "Only support Beta Residual Q2 RSS for Sparse PLS.");
    
    ASSERT(meanY.cols() != 0 && meanY.rows() != 0, "Train data first");
    
    SLModelResult result;
    if(type & SLMODELRESULTYPEQ2)
    {
        result[SLMODELRESULTYPEQ2] = MatrixXd::Ones(1, Y.cols()) - SSum(Res).cwiseQuotient(SSum(Y));
    }
    
    if(type & SLMODELRESULTYPERSS)
    {
        result[SLMODELRESULTYPERSS] = SSum(Res);
    }
    
    if(type & SLMODELRESULTYPEBETA)
    {
        result[SLMODELRESULTYPEBETA] = Beta;
    }
    
    if(type & SLMODELRESULTYPEACC)
    {
        result[SLMODELRESULTYPEACC] = calcACC(X, Y);
    }
    return result;
}

MatrixXd SLSparsePls::calcACC(const MatrixXd& tX, const MatrixXd& tY) const
{
    MatrixXd realY    = tY + meanY.replicate(tY.rows(), 1);
    MatrixXd predictY = tX*Beta;
    size_t correct = 0;
    MatrixXd acc(1,realY.cols());
    
    VectorXd min = realY.colwise().minCoeff();
    VectorXd max = realY.colwise().maxCoeff();
    VectorXd mid = (min + max) / 2;
    for (int i=0; i < realY.cols(); ++i)
    {
        correct = 0;
        for (int j=0; j < realY.rows(); ++j)
        {
            if (predictY(j,i) > mid[i])
            {
                if(realY(j,i) > mid[i])
                {
                    ++correct;
                }
            }
            else
            {
                if(realY(j,i) <= mid[i])
                {
                    ++correct;
                }
            }
        }
        acc(0,i) = (double)correct/tY.rows();
    }
    return acc;
}
