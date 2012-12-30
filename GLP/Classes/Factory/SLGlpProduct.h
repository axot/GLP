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
    SLGlpProduct (MP& modelParameters, GP& graphMiningParameters)
    {
        ASSERT( model.initParameters(modelParameters) != false,
               "Did you override initParameters method?");
        
        ASSERT( graphMining.initParameters(graphMiningParameters) != false,
               "Did you override initParameters method?");
    }
    
    // Implementation SLModelStrategy
    map<SLTRAINRESULTYPE, MatrixXd> getTrainResult(SLTRAINRESULTYPE type)   { return model.getTrainResult(type); }
    bool train   (MatrixXd& X, MatrixXd& Y, MatrixXd** Beta)                { return model.train(X, Y, Beta); }
    bool validate(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta)                 { return model.validate(X, Y, Beta); }
    bool classify(MatrixXd& X, MatrixXd& Y, MatrixXd& Beta)                 { return model.classify(X, Y, Beta); }
    
    // Implementation SLGraphMiningStrategy
    MatrixXd& search() { return graphMining.search(); }
        
private:
    SLModel<M> model;
    SLGraphMining<G> graphMining;
};

#endif /* defined(__GLP__SLGlpCombination__) */
