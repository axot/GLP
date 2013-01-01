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
SLGlpResult SLSparsePls::train(const MatrixXd& appendedX, const MatrixXd& theY, SLGLPRESULTYPE type)
{
    if (Y.cols() == 0 || Y.rows() == 0)
    {
        meanY = theY.colwise().mean();
        Y = AutoScale(theY);
        RES = Y;
    }
    
    long appendedXRows = appendedX.rows();
    long appendedXCols = appendedX.cols();
    long oldXCols      = X.cols();
    
    long maxSquaredNormColumn;
    SSum(RES).maxCoeff(&maxSquaredNormColumn);
    
    residual = RES.col(maxSquaredNormColumn);

    meanX.conservativeResize(1, oldXCols+appendedXCols);
    meanX << meanX.leftCols(oldXCols), appendedX.colwise().mean();
    
    X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
    X.rightCols(appendedXCols).setZero();
    X << X.leftCols(oldXCols), AutoScale(appendedX);
    
    W.conservativeResize(oldXCols+appendedXCols, W.cols()+1);
    W.bottomRows(appendedXCols).setZero();
    W.rightCols(1).setZero();
    
    W.rightCols(1) = X.transpose()*residual;
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
        LOG(residual);
        LOG(Beta);
        LOG(X*Beta.col(0));
        getchar();
    }
    RES = Y - X*Beta;
    
    return getTrainResult(type);
}

SLGlpResult SLSparsePls::classify(const MatrixXd& tX, const MatrixXd& tY, SLGLPRESULTYPE type) const
{
    ASSERT(type != SLGLPRESULTYPENONE, "No type of result was indicated");
    
    ASSERT(!(type & ~(SLGLPRESULTYPEBETA|SLGLPRESULTYPEQ2|SLGLPRESULTYPERSS)),
           "Only support Beta Q2 RSS for Sparse PLS.");
    
    ASSERT(meanY.cols() != 0 && meanY.rows() != 0, "Train data first");
    
    SLGlpResult result;
    
    MatrixXd autoSacaledY = tY - meanY.replicate(tY.rows(), 1);
    MatrixXd autoSacaledX = tX - meanX.replicate(tX.rows(), 1);

    MatrixXd tRES = autoSacaledY - autoSacaledX*Beta;

    if(type & SLGLPRESULTYPEQ2)
    {
        result[SLGLPRESULTYPEQ2] = MatrixXd::Ones(1, autoSacaledY.cols()) - SSum(tRES).cwiseQuotient(SSum(autoSacaledY));
    }
    if(type & SLGLPRESULTYPERSS)
    {
        result[SLGLPRESULTYPERSS] = SSum(tRES);
    }
    if(type & SLGLPRESULTYPEBETA)
    {
        result[SLGLPRESULTYPEBETA] = Beta;
    }
    return result;
}

bool SLSparsePls::initParameters(SLSparsePlsParameters parameters)
{
    SLModelStrategy::initParameters(parameters);
    return true;
}

// Private Methods
SLGlpResult SLSparsePls::getTrainResult(SLGLPRESULTYPE type) const
{
    ASSERT(!(type & ~(SLGLPRESULTYPEBETA|SLGLPRESULTYPEQ2|SLGLPRESULTYPERSS)),
           "Only support Beta Q2 RSS for Sparse PLS.");
    
    ASSERT(meanY.cols() != 0 && meanY.rows() != 0, "Train data first");
    
    SLGlpResult result;
    if(type & SLGLPRESULTYPEQ2)
    {
        result[SLGLPRESULTYPEQ2] = MatrixXd::Ones(1, Y.cols()) - SSum(RES).cwiseQuotient(SSum(Y));
    }
    if(type & SLGLPRESULTYPERSS)
    {
        result[SLGLPRESULTYPERSS] = SSum(RES);
    }
    if(type & SLGLPRESULTYPEBETA)
    {
        result[SLGLPRESULTYPEBETA] = Beta;
    }
    return result;
}
