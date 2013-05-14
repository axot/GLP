//
//  SLModelStrategy.cpp
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

#include "SLModelStrategy.h"

MatrixXd SLModelStrategy::getRSS(const MatrixXd& RES) const
{
    return (MatrixXd)SSum(RES);
}

MatrixXd SLModelStrategy::getQ2(const MatrixXd& RES, const MatrixXd& Y) const
{
    return (MatrixXd)(MatrixXd::Ones(1, Y.cols()) - SSum(RES).cwiseQuotient(SSum(Center(Y))));
}

MatrixXd SLModelStrategy::getACC(const MatrixXd& Y, const MatrixXd& predictY) const
{
    size_t correct = 0;
    MatrixXd acc(1,Y.cols());
    
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
        acc(0,i) = (double)correct/Y.rows();
    }
    return acc;
}

MatrixXd SLModelStrategy::getAUC(const MatrixXd& Y, const MatrixXd& predictY) const
{
    vector < pair < double, double > > ypPair(Y.rows());
    MatrixXd acc(1,Y.cols());
    VectorXd min = Y.colwise().minCoeff();
    VectorXd max = Y.colwise().maxCoeff();
    VectorXd mid = (min + max) / 2.0;
    
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
            if (ypPair[j].first > mid[i])
            {
                ++tp;
            }
            else
            {
                auc += tp;
                ++fp;
            }
        }
        acc(0,i) = (double)auc/(double)(tp*fp);
    }
    return acc;
}
