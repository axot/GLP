//
//  SLModelStrategy.h
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

#ifndef GLP_SLModelStrategy_h
#define GLP_SLModelStrategy_h

#include <Eigen/Core>

using namespace Eigen;

enum{
    Q2      = 0,
    RSS     = 1 << 0,
    AUC     = 1 << 1,
    ACC     = 1 << 2
};

typedef unsigned int RESULTYPE;

class SLModelStrategy
{
public:
    /* Train:
     * Input
     *      X: train X matrix
     *      Y: train Y matrix
     *
     * Output
     *      Beta: the calculated Beta value
     *
     * return: true if sucessed.
     */
    virtual bool train(MatrixXd& X, MatrixXd& Y, MatrixXd *Beta) = 0;
    
    /* Validate:
     * Input
     *      X: test X matrix
     *      Y: test Y matrix
     *
     * Output
     *      Beta: the calculated Beta value
     *
     * return: true if sucessed.
     */
    virtual bool validate(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta) = 0;
    
    /* Train:
     * Input
     *      X: X matrix
     *      Y: Y matrix
     *
     * Output
     *      Beta: the calculated Beta value
     *
     * return: true if sucessed.
     */
    virtual bool classify(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta) = 0;
    
private:
    MatrixXd X;
    MatrixXd Y;
    MatrixXd Beta;
};

#endif
