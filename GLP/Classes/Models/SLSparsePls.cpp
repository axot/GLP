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

bool SLSparsePls::train(MatrixXd& appendedX, MatrixXd& theY)
{
    if (Y.cols() == 0 || Y.rows() == 0)
    {
        Y = AutoScale(theY);
        RES = Y;
    }
    
    long appendedXRows = appendedX.rows();
    long appendedXCols = appendedX.cols();
    long oldXCols      = X.cols();
    
    long maxSquaredNormColumn;
    SSum(RES).maxCoeff(&maxSquaredNormColumn);
    
    residual = RES.col(maxSquaredNormColumn);

    X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
    X.rightCols(appendedXCols).setZero();
    X << X.leftCols(oldXCols), appendedX;

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
    
    return true;
}

SLTrainResult SLSparsePls::getTrainResult(SLTRAINRESULTYPE type)
{
    ASSERT(!(type&SLTRAINRESULTYPEACC || type&SLTRAINRESULTYPEAUC), "Only support Beta Q2 RSS for Sparse PLS.");
    
    SLTrainResult result;
    if(type & SLTRAINRESULTYPEQ2)
    {
        result[SLTRAINRESULTYPEQ2] = MatrixXd::Ones(1, Y.cols()) - SSum(RES).cwiseQuotient(SSum(Y));
    }
    if(type & SLTRAINRESULTYPERSS)
    {
        result[SLTRAINRESULTYPERSS] = SSum(RES);
    }
    if(type & SLTRAINRESULTYPEBETA)
    {
        result[SLTRAINRESULTYPEBETA] = Beta;
    }
    return result;
}

bool SLSparsePls::validate(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta)
{
    return false;
}

bool SLSparsePls::classify(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta)
{
    return false;
}

bool SLSparsePls::initParameters(SLSparsePlsParameters parameters)
{
    return true;
}
