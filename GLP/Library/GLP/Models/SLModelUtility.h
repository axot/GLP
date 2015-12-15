//
//  SLModelUtility.h
//  GLP
//
//  Created by Zheng Shao on 11/29/15.
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

#ifndef SLModelUtility_hpp
#define SLModelUtility_hpp

#include "SLModelStrategy.h"

class SLModelUtility
{
public:
    static void printResult(SLMODELRESULTYPE resultType, SLModelResult& result);
    
    /*! @name Common calculation methods of performance measures
     *
     */
    //@{
    static MatrixXd getRSS(const MatrixXd& RES);
    static MatrixXd getQ2 (const MatrixXd& RES, const MatrixXd& Y);
    static MatrixXd getACC(const MatrixXd& Y, const MatrixXd& predictY);
    static MatrixXd getAUC(const MatrixXd& Y, const MatrixXd& predictY);
    static MatrixXd getAIC(const MatrixXd& Y, const MatrixXd& predictY, const size_t numOfParams);
    static MatrixXd getBIC(const MatrixXd& Y, const MatrixXd& predictY, const size_t numOfParams);
    static MatrixXd getCOV(const MatrixXd& Y, const MatrixXd& predictY);
    //@}
    
private:
    template <typename T1, typename T2>
    struct ypcmp: public binary_function<bool, T1, T2> {
        bool operator () (const std::pair <T1, T2> &x1, const std::pair<T1, T2> &x2) {
            return x1.second > x2.second;
        }
    };
};

#endif /* SLModelUtility_h */
