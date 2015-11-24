//
//  SLModel.h
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

#ifndef __GLP__SLModel__
#define __GLP__SLModel__

#include <iostream>
#include <Eigen/Core>
#include "SLModelStrategy.h"

using namespace Eigen;

// the Context of strategy pattern
template <typename AStrategy>
class SLModel
{
public:
    SLModelResult train(const MatrixXd& X, const MatrixXd& Y, SLMODELRESULTYPE type)
    {
        return aStrategy.train(X, Y, type);
    }
    
    SLModelResult classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const
    {
        return aStrategy.classify(tX, tY, type);
    }

    template <typename MP>
    MP getParameters() const
    {
        return aStrategy.getParameters();
    }
    
    template <typename MP>
    bool setParameters(MP& modelParameters)
    {
        return aStrategy.setParameters(modelParameters);
    }
    
    AStrategy& getModel() const
    {
        return aStrategy;
    }

private:
    // not assignable parameters
    AStrategy aStrategy;
};

#endif /* defined(__GLP__SLModel__) */
