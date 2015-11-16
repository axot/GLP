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

#ifndef __GLP__SLModelStrategy__
#define __GLP__SLModelStrategy__

#include <iostream>
#include <map>
#include <vector>
#include <boost/variant.hpp>
#include "../SLUtility.h"

using namespace std;

enum{
    SLModelResultTypeNone    = 0,
    SLModelResultTypeBeta    = 1 << 0,
    SLModelResultTypeQ2      = 1 << 1,
    SLModelResultTypeRSS     = 1 << 2,
    SLModelResultTypeAUC     = 1 << 3,
    SLModelResultTypeACC     = 1 << 4,
    SLModelResultTypeAIC     = 1 << 5,
    SLModelResultTypeBIC     = 1 << 6,
    SLModelResultTypeCOV     = 1 << 7,
};

typedef unsigned int SLMODELRESULTYPE;
typedef map<SLMODELRESULTYPE, boost::variant< VectorXd, MatrixXd, string > > SLModelResult;

/**
 * SLModelStrategy class
 */
class SLModelStrategy
{
public:
    /** Train model
     * @param X X matrix of train data
     * @param Y Y matrix of train data
     * @param type type of results
     *
     * @return the results stored in mapped structure.
     */
    virtual SLModelResult train(const MatrixXd& X, const MatrixXd& Y, SLMODELRESULTYPE type) = 0;
        
    /** Classify model
     * @param tX X matrix of test data
     * @param tY Y matrix of test data
     * @param type type of results
     *
     * @return the results stored in mapped structure
     */
    virtual SLModelResult classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const = 0;
        
    /** Setter of Parameters
     * @param parameters Model assignable parameters
     *
     * @return true if sucessed.
     */
    template <typename MP>
    bool setParameters(MP& parameters) { return true; };
    
    /** Getter of Parameters
     *
     * implementng cross validation you have to override this method.
     *
     * @return current parameters.
     */
    template <typename MP>
    MP getParameters() const { MP mp; return mp; }
    
    /*! @name Varibles for binary classify problem
     *
     * for the case of 0/1 label, min is 0, max is 1
     */
    //@{
    double min, max;
    //@}
    
    /*! @name Common calculation methods of performance measures
     *
     */
    //@{
    virtual MatrixXd getRSS(const MatrixXd& RES) const;
    virtual MatrixXd getQ2 (const MatrixXd& RES, const MatrixXd& Y) const;
    virtual MatrixXd getACC(const MatrixXd& Y, const MatrixXd& predictY) const;
    virtual MatrixXd getAUC(const MatrixXd& Y, const MatrixXd& predictY) const;
    virtual MatrixXd getAIC(const MatrixXd& Y, const MatrixXd& predictY, const size_t numOfParams) const;
    virtual MatrixXd getBIC(const MatrixXd& Y, const MatrixXd& predictY, const size_t numOfParams) const;
    virtual MatrixXd getCOV(const MatrixXd& Y, const MatrixXd& predictY) const;
    //@}

private:
    template <typename T1, typename T2>
    struct ypcmp: public binary_function<bool, T1, T2> {
        bool operator () (const std::pair <T1, T2> &x1, const std::pair<T1, T2> &x2) {
            return x1.second > x2.second;
        }
    };
};
#endif
