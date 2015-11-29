//
//  SLModelUtility.cpp
//  GLP
//
//  Created by Zheng Shao on 5/14/13.
//  Copyright (c) 2013 Saigo Laboratoire. All rights reserved.
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

#include "SLModelUtility.h"
#include <iostream>
#include <string>
#include <boost/format.hpp>

using namespace std;
using namespace Eigen;
using namespace boost;

void SLModelUtility::printResult(SLMODELRESULTYPE resultType, SLModelResult& result)
{
    if ( resultType & SLModelResultTypeQ2 )
        cout << format("%8s: %2.8f") % "Q2" % get< MatrixXd >(result[SLModelResultTypeQ2]).mean();
    
    if ( resultType & SLModelResultTypeRSS )
        cout << format("%8s: %14.8f") % "RSS" % get< MatrixXd >(result[SLModelResultTypeRSS]).mean();
    
    if ( resultType & SLModelResultTypeCOV )
        cout << format("%8s: %2.8f") % "COV" % get< MatrixXd >(result[SLModelResultTypeCOV]).mean();
    
    if ( resultType & SLModelResultTypeACC )
        cout << format("%8s: %2.8f") % "ACC" % get< MatrixXd >(result[SLModelResultTypeACC]).mean();
    
    if ( resultType & SLModelResultTypeAUC )
        cout << format("%8s: %2.8f") % "AUC" % get< MatrixXd >(result[SLModelResultTypeAUC]).mean();
    
    if ( resultType & SLModelResultTypeAIC )
        cout << format("%8s: %14.8f") % "AIC" % get< MatrixXd >(result[SLModelResultTypeAIC]).mean();
    
    if ( resultType & SLModelResultTypeBIC )
        cout << format("%8s: %14.8f") % "BIC" % get< MatrixXd >(result[SLModelResultTypeBIC]).mean();
    
    cout << endl << endl;
}

MatrixXd SLModelUtility::getRSS(const MatrixXd& RES)
{
    return (MatrixXd)ColSSum(RES);
}

MatrixXd SLModelUtility::getQ2(const MatrixXd& RES, const MatrixXd& Y)
{
    return (MatrixXd)(MatrixXd::Ones(1, Y.cols()) - ColSSum(RES).cwiseQuotient(ColSSum(Center(Y))));
}

MatrixXd SLModelUtility::getACC(const MatrixXd& Y, const MatrixXd& predictY)
{
    size_t correct = 0;
    MatrixXd ACC(1,Y.cols());
    
    VectorXd min = Y.colwise().minCoeff();
    VectorXd max = Y.colwise().maxCoeff();
    VectorXd mid = (min + max) / 2;
    for (int i=0; i < Y.cols(); ++i)
    {
        correct = 0;
        for (int j=0; j < Y.rows(); ++j)
        {
            if (predictY(j,i) > mid[i])
            {
                if(Y(j,i) > mid[i])
                {
                    ++correct;
                }
            }
            else
            {
                if(Y(j,i) < mid[i])
                {
                    ++correct;
                }
            }
        }
        ACC(0,i) = (double)correct/Y.rows();
    }
    return ACC;
}

MatrixXd SLModelUtility::getAUC(const MatrixXd& Y, const MatrixXd& predictY)
{
    MatrixXd AUC(1, Y.cols());
    
    double min = Y.minCoeff();
    double max = Y.maxCoeff();
    double mid = (min + max) / 2.0;
    vector < pair < double, double > > ypPair(Y.rows());
    
    for (int i=0; i < Y.cols(); ++i)
    {
        ypPair.clear();
        for (int j=0; j < Y.rows(); ++j)
            ypPair.push_back(make_pair< double, double >((double)Y(j,i), (double)predictY(j,i)));
        
        sort(ypPair.begin(), ypPair.end(), ypcmp<double, double>());
        
        size_t tp = 0;
        size_t fp = 0;
        size_t auc = 0;
        for (int j=0; j < Y.rows(); ++j)
        {
            if (ypPair[j].first > mid)
            {
                ++tp;
            }
            else
            {
                auc += tp;
                ++fp;
            }
        }
        AUC(0,i) = (double)auc/(double)(tp*fp);
    }
    return AUC;
}

#define _PI acos(-1.0)
MatrixXd SLModelUtility::getAIC(const MatrixXd& Y, const MatrixXd& predictY, const size_t numOfParams)
{
    MatrixXd rss  = ColSSum(Y - predictY);
    
    MatrixXd logL = -Y.rows()/2.0*(rss/Y.rows()).array().log()
                    -Y.rows()/2.0*log(2.0*_PI)
                    -Y.rows()/2.0;
    
    return -2.0*logL.array() + 2.0*numOfParams;
}

MatrixXd SLModelUtility::getBIC(const MatrixXd& Y, const MatrixXd& predictY, const size_t numOfParams)
{
    MatrixXd rss  = ColSSum(Y - predictY);
    
    MatrixXd logL = -Y.rows()/2.0*(rss/Y.rows()).array().log()
                    -Y.rows()/2.0*log(2.0*_PI)
                    -Y.rows()/2.0;
    
    return -2.0*logL.array() + numOfParams*log(Y.rows());
}

MatrixXd SLModelUtility::getCOV(const MatrixXd& Y, const MatrixXd& predictY)
{
    MatrixXd Yhat = Center(Y);
    MatrixXd Phat = Center(predictY);
    return (Yhat.transpose() * Phat).diagonal() / Yhat.rows();
}
