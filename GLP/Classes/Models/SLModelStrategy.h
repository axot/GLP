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

#include <map>
#include <Eigen/Core>
#include "../SLUtility.h"

using namespace std;
using namespace Eigen;

enum{
    SLTRAINRESULTYPEQ2      = 1,
    SLTRAINRESULTYPERSS     = 1 << 1,
    SLTRAINRESULTYPEAUC     = 1 << 2,
    SLTRAINRESULTYPEACC     = 1 << 3,
    SLTRAINRESULTYPEBETA    = 1 << 4
};

typedef unsigned int SLTRAINRESULTYPE;
typedef map<SLTRAINRESULTYPE, MatrixXd> SLTrainResult;

class SLModelStrategy
{
public:
    /* Train:
     * Input
     *      X: train X matrix
     *      Y: train Y matrix
     *
     * Return: true if sucessed.
     */
    virtual bool train(MatrixXd& X, MatrixXd& Y) = 0;
    
    /* getTrainResult:
     * Input
     *      type: type of results
     *
     * Return: the results stored in mapped structure
     */
    virtual SLTrainResult getTrainResult(SLTRAINRESULTYPE type) = 0;

    /* Validate:
     * Input
     *      X: test X matrix
     *      Y: test Y matrix
     *
     * Output
     *      Beta: the calculated Beta value
     *
     * Return: true if sucessed.
     */
    virtual bool validate(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta) = 0;
    
    /* Classify:
     * Input
     *      X: X matrix
     *      Y: Y matrix
     *
     * Output
     *      Beta: the calculated Beta value
     *
     * Return: true if sucessed.
     */
    virtual bool classify(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta) = 0;

    /* Init Parameters:
     * Input
     *      parameters: Model assignable parameters
     *
     * Return: true if sucessed.
     *
     * Discussion: must override this method
     */
    template <typename MP>
    bool initParameters(MP parameters) { return false; };
};

#endif
