//
//  SLGlpProduct.h
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

#ifndef __GLP__SLGlpProduct__
#define __GLP__SLGlpProduct__

#include <iostream>
#include "../Models/SLModel.h"
#include "../GraphMining/SLGraphMining.h"
#include "../SLUtility.h"

using namespace std;

template <typename M, typename G>
class SLGlpProduct : public SLModelStrategy, public SLGraphMiningStrategy
{    
public:
    template <typename MP, typename GP>
    SLGlpProduct(MP& modelParameters, GP& graphMiningParameters)
    {
        model.initParameters(modelParameters);
        graphMining.initParameters(graphMiningParameters);
    }

    // SLModelStrategy
    virtual SLGlpResult train(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type)
    {
        return model.train(X, Y, type);
    }

    virtual SLGlpResult classify(const MatrixXd& tX, const MatrixXd& tY, SLGLPRESULTYPE type) const
    {
        return model.classify(tX, tY, type);
    }
    
    virtual SLGlpCrossValidationResults crossValidation(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type) const
    {
        return model.crossValidation(type);
    }

    // SLGraphMiningStrategy
    virtual MatrixXd& search()
    {
        return graphMining.search();
    }

private:
    SLModel<M> model;
    SLGraphMining<G> graphMining;
};

#endif /* defined(__GLP__SLGlpCombination__) */
