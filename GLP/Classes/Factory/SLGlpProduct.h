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
#include "../Models/SLSparsePls.h"
#include "../GraphMining/SLGraphMining.h"
#include "../SLUtility.h"
#include "../Utilities/SLCrossValidation.h"

using namespace std;

template <typename M, typename G>
class SLGlpProduct : public SLModelStrategy, public SLGraphMiningStrategy
{    
public:
    template <typename MP, typename GP>
    SLGlpProduct(MP& modelParameters, GP& graphMiningParameters)
    {
        model.setParameters(modelParameters);
        graphMining.setParameters(graphMiningParameters);
        typename SLCrossValidation<M>::SLCrossValidationParameters param;
        param.modelClone = model.getModel();
        cv.setParameters(param);
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

    // SLGraphMiningStrategy
    virtual MatrixXd& search()
    {
        return graphMining.search();
    }

    // Custom Methods
    SLCrossValidationResults crossValidation(const MatrixXd& X,
                                             const MatrixXd& Y,
                                             SLGLPRESULTYPE resultType,
                                             SLCROSSVALIDATIONMETHODSTYPE methodType = SLCROSSVALIDATIONMETHODSUSINGWHOLEDATA)
    {
        // C++ does not support partial specialization of member function
        if ( typeid(model) == typeid(SLModel<SLSparsePls>) )
            return cv.crossValidation(X, Y, resultType, SLCROSSVALIDATIONMETHODSUSINGAPPENDEDXASCLASSIFYDATA);
        else
            return cv.crossValidation(X, Y, resultType, methodType);
    }

    void setCrossValidationParameters(typename SLCrossValidation<M>::SLCrossValidationParameters parameters)
    {
        cv.setParameters(parameters);
    }
    
    const SLCrossValidationResultHistory& getResultHistory()
    {
        return cv.getResultHistory();
    }

private:
    SLModel<M> model;
    SLGraphMining<G> graphMining;
    SLCrossValidation<M> cv;
};

#endif /* defined(__GLP__SLGlpCombination__) */
