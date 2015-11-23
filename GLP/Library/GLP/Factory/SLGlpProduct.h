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
#include <typeinfo>
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
        cv.setParameters(cvParam);
    }

    // SLModelStrategy
    virtual SLModelResult train(const MatrixXd& X, const MatrixXd& Y, SLMODELRESULTYPE type)
    {
        return model.train(X, Y, type);
    }

    virtual SLModelResult classify(const MatrixXd& tX, const MatrixXd& tY, SLMODELRESULTYPE type) const
    {
        return model.classify(tX, tY, type);
    }

    // SLGraphMiningStrategy
    virtual SLGraphMiningResult search(VectorXd residual,
                               SLGRAPHMININGTASKTYPE taskType,
                               SLGRAPHMININGRESULTYPE resultType)
    {
        return graphMining.search(residual, taskType, resultType);
    }

    virtual SLGraphMiningInnerValues getInnerValues(SLGRAPHMININGINNERVALUE type) const
    {
        return graphMining.getInnerValues(type);
    }

    // Custom Methods
    SLCrossValidationResults crossValidation(const MatrixXd& X,
                                             const MatrixXd& Y,
                                             SLMODELRESULTYPE resultType,
                                             SLCROSSVALIDATIONMETHODSTYPE methodType = SLCrossValidationMethodsUsingWholeData)
    {
        // C++ does not support partial specialization of member function
        if ( typeid(model) == typeid(SLModel<SLSparsePls>) )
            return cv.crossValidation(X, Y, resultType, SLCrossValidationMethodsUsingAppendedXAsClassifyData);
        else
            return cv.crossValidation(X, Y, resultType, methodType);
    }

    void setCrossValidationParameters(typename SLCrossValidation<M>::SLCrossValidationParameters& parameters)
    {
        parameters.modelClone = model.getModel();
        cv.setParameters(parameters);
    }
    
    template <typename MP>
    void setModelParameters(MP & param)
    {
        model.setParameters(param);
        cvParam.modelClone = model.getModel();
        cv.setParameters(cvParam);
    }
    
    const SLCrossValidationResultHistory& getResultHistory() const
    {
        return cv.getResultHistory();
    }
    
    M getModel()
    {
        return model.getModel();
    }
    
    G getGraphMining()
    {
        return graphMining.getGraphMining();
    }
    
private:
    SLModel<M> model;
    SLGraphMining<G> graphMining;
    SLCrossValidation<M> cv;
    typename SLCrossValidation<M>::SLCrossValidationParameters cvParam;
};

#endif /* defined(__GLP__SLGlpCombination__) */
