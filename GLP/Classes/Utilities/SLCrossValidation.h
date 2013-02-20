//
//  SLCrossValidation.h
//  GLP
//
//  Created by Zheng Shao on 1/2/13.
//  Copyright (c) 2013 Saigo Laboratoire. All rights reserved.
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

#ifndef __GLP__SLCrossValidation__
#define __GLP__SLCrossValidation__

#if (defined _OPENMP)
#include <omp.h>
#endif

#include <iostream>
#include <Eigen/Core>
#include <boost/typeof/typeof.hpp>
#include "../Models/SLModelStrategy.h"
#include "../SLUtility.h"
#include "SLCrossValidationResults.h"
#include "SLCrossValidationResultHistory.h"

using namespace std;
using namespace boost;
using namespace Eigen;

enum{
    SLCrossValidationMethodsUsingWholeData                  = 0,
    SLCrossValidationMethodsUsingAppendedXAsClassifyData    = 1 << 0,
};

typedef unsigned int SLCROSSVALIDATIONMETHODSTYPE;

template <typename T>
class SLCrossValidation
{
public:
    class SLCrossValidationParameters
    {
    public:
        SLCrossValidationParameters() : kFold(10), resultHistorySize(5), doesUseShuffleData(false) {}
        
    public:
        size_t kFold;
        size_t resultHistorySize;
        bool doesUseShuffleData;
        T modelClone;
    };

    SLCrossValidation()
    {
        typename SLCrossValidation<T>::SLCrossValidationParameters param;
        setParameters(param);
    }
    
    /* K-fold Cross Vaildate:
     * Input
     *          X: X matrix
     *          Y: Y matrix
     * resultType: type of results
     * methodType: type of methods, default using whole data.
     *
     * Defualts
     * methodType: SLCROSSVALIDATIONMETHODSUSINGWHOLEDATA
     *
     * Return: the results stored in mapped structure.
     */
    SLCrossValidationResults crossValidation(const MatrixXd& X,
                                             const MatrixXd& Y,
                                             SLMODELRESULTYPE resultType,
                                             SLCROSSVALIDATIONMETHODSTYPE methodType = SLCrossValidationMethodsUsingWholeData);
    
    /* Set Parameters:
     * Input
     *      parameters: Cross Validation assignable parameters
     *
     */
    void setParameters(typename SLCrossValidation<T>::SLCrossValidationParameters parameters)
    {
        doesUseShuffleData = parameters.doesUseShuffleData;
        kFold = parameters.kFold;
        resultHistory.resultHistorySize = parameters.resultHistorySize;
        modelClone = parameters.modelClone;
    }

    /* Cross Validation Result History:
     *
     *      Return: the result history of cross validation.
     *
     *  Discussion: the structure of result history:
     *               ___
     *              | 0 |   <- current result
     *              | 1 |   <- the last time of result
     *              | 2 |   <- the time before last of result
     *              |...|
     */
    const SLCrossValidationResultHistory& getResultHistory() const
    {
        return resultHistory;
    }
    
private:
    /* Split method for cross validation:
     *
     * Discusion: if startIndex < 0, startIndex = total length + startIndex
     *            if startIndex+length > total length,
     *            return merged 2 blocks, one part is the begining of the matrix
     */
    MatrixXd cvSplitMatrix(MatrixXd& from, ssize_t startIndex, size_t length) const;
    
    SLGlpMultipleResults crossValidationTrain(const size_t index,
                                              const MatrixXd& X,
                                              const MatrixXd& Y,
                                              SLMODELRESULTYPE resultType,
                                              SLCROSSVALIDATIONMETHODSTYPE methodType);
    
    SLGlpMultipleResults crossValidationValidationClassify(const size_t index,
                                                           const MatrixXd& X,
                                                           const MatrixXd& Y,
                                                           SLMODELRESULTYPE resultType,
                                                           SLCROSSVALIDATIONMETHODSTYPE methodType);
    
    SLGlpMultipleResults crossValidationTestClassify(const size_t index,
                                                     const MatrixXd& X,
                                                     const MatrixXd& Y,
                                                     SLMODELRESULTYPE resultType,
                                                     SLCROSSVALIDATIONMETHODSTYPE methodType);
    
private:
    // assignable parameters via setParameters() method
    bool doesUseShuffleData;
    size_t kFold;
    SLCrossValidationResultHistory resultHistory;
    T modelClone;
    
    // not assignable parameters
    VectorXi randomIndexs;
    vector<T> cvModels;
    
    // for sparse models
    vector<MatrixXd> cvXValidationData;
    vector<MatrixXd> cvXTestData;
};

// Implementation
template <typename T>
SLCrossValidationResults SLCrossValidation<T>::crossValidation(const MatrixXd& X,
                                                               const MatrixXd& Y,
                                                               SLMODELRESULTYPE resultType,
                                                               SLCROSSVALIDATIONMETHODSTYPE methodType)
{
    ASSERT(kFold <= (size_t)X.rows(), "K-Fold was too big than X rows.");
    ASSERT(kFold >= 4, "K-Fold was too small, it must bigger than 4.");
    
    if (randomIndexs.size() == 0)
    {
        srand((unsigned int)time(NULL));
        randomIndexs.setLinSpaced(X.rows(), 0, X.rows());
        
        if ( doesUseShuffleData )
        {
            random_shuffle(randomIndexs.derived().data(), randomIndexs.derived().data()+randomIndexs.derived().size());
        }
        
        for (size_t i=0; i<kFold; ++i)
        {
            T model;
            BOOST_AUTO(modelParameters, modelClone.getParameters());
            model.setParameters(modelParameters);
            cvModels.push_back(model);
        }
    }
    
    MatrixXd shuffledX = X;
    MatrixXd shuffledY = Y;
    
#pragma omp parallel for
    for (int i=0; i<X.rows(); ++i)
    {
        shuffledX.row(i) = X.row(randomIndexs[i]);
        shuffledY.row(i) = Y.row(randomIndexs[i]);
    }
    
    size_t oneUnitLength = shuffledX.rows()/kFold;
    SLCrossValidationResults result;
    for (size_t i = 0; i < kFold; ++i)
    {
        MatrixXd trainX = cvSplitMatrix(shuffledX, -oneUnitLength*i, (kFold-2)*oneUnitLength);
        MatrixXd trainY = cvSplitMatrix(shuffledY, -oneUnitLength*i, (kFold-2)*oneUnitLength);
        
        MatrixXd validationX = cvSplitMatrix(shuffledX, shuffledX.rows()-oneUnitLength*(i+2), oneUnitLength);
        MatrixXd validationY = cvSplitMatrix(shuffledY, shuffledY.rows()-oneUnitLength*(i+2), oneUnitLength);
        
        MatrixXd testX = cvSplitMatrix(shuffledX, shuffledX.rows()-oneUnitLength*(i+1), oneUnitLength);
        MatrixXd testY = cvSplitMatrix(shuffledY, shuffledY.rows()-oneUnitLength*(i+1), oneUnitLength);
        
        SLCrossValidationResults eachResult;
        eachResult[SLCrossValidationResultTypeTrain]      = crossValidationTrain(i, trainX, trainY, resultType, methodType);
        eachResult[SLCrossValidationResultTypeValidation] = crossValidationValidationClassify(i, validationX, validationY, resultType, methodType);
        eachResult[SLCrossValidationResultTypeTest]       = crossValidationTestClassify(i, testX, testY, resultType, methodType);
        
        result += eachResult;
    }
    
    resultHistory.push_front(result);
    return result;
}

template <typename T>
MatrixXd SLCrossValidation<T>::cvSplitMatrix(MatrixXd& from, ssize_t startIndex, size_t length) const
{
    if (startIndex < 0){
        startIndex = from.rows() + startIndex;
    }
    if ((int)(startIndex+length) <= from.rows()) {
        return from.block(startIndex, 0, length, from.cols());
    }
    size_t beginLength = startIndex+length-from.rows();
    
    MatrixXd m(length, from.cols());
    m << from.block(0, 0, beginLength, from.cols()), from.block(startIndex, 0, length-beginLength, from.cols());
    return m;
}

template <typename T>
SLGlpMultipleResults SLCrossValidation<T>::crossValidationTrain(const size_t index,
                                                                const MatrixXd& X,
                                                                const MatrixXd& Y,
                                                                SLMODELRESULTYPE resultType,
                                                                SLCROSSVALIDATIONMETHODSTYPE methodType)
{
    SLGlpMultipleResults result;
    result.push_back(cvModels[index].train(X, Y, resultType));
    return result;
}

template <typename T>
SLGlpMultipleResults SLCrossValidation<T>::crossValidationValidationClassify(const size_t index,
                                                                             const MatrixXd& X,
                                                                             const MatrixXd& Y,
                                                                             SLMODELRESULTYPE resultType,
                                                                             SLCROSSVALIDATIONMETHODSTYPE methodType)
{
    SLGlpMultipleResults result;
    if ( methodType & SLCrossValidationMethodsUsingAppendedXAsClassifyData )
    {
        if(cvXValidationData.size() < cvModels.size()){
            cvXValidationData.push_back(X);
        }else{
            size_t oldCols = cvXValidationData[index].cols();
            cvXValidationData[index].conservativeResize(X.rows(), oldCols+X.cols());
            cvXValidationData[index] << cvXValidationData[index].leftCols(oldCols), X;
        }
        
        result.push_back(cvModels[index].classify(cvXValidationData[index], Y, resultType));
    }
    else
    {
        result.push_back(cvModels[index].classify(X, Y, resultType));
    }
    
    return result;
}

template <typename T>
SLGlpMultipleResults SLCrossValidation<T>::crossValidationTestClassify(const size_t index,
                                                                       const MatrixXd& X,
                                                                       const MatrixXd& Y,
                                                                       SLMODELRESULTYPE resultType,
                                                                       SLCROSSVALIDATIONMETHODSTYPE methodType)
{
    SLGlpMultipleResults result;
    if ( methodType & SLCrossValidationMethodsUsingAppendedXAsClassifyData )
    {
        if(cvXTestData.size() < cvModels.size()){
            cvXTestData.push_back(X);
        }else{
            size_t oldCols = cvXTestData[index].cols();
            cvXTestData[index].conservativeResize(X.rows(), oldCols+X.cols());
            cvXTestData[index] << cvXTestData[index].leftCols(oldCols), X;
        }
        result.push_back(cvModels[index].classify(cvXTestData[index], Y, resultType));
    }
    else
    {
        result.push_back(cvModels[index].classify(X, Y, resultType));
    }
    
    return result;
}

#endif /* defined(__GLP__SLCrossValidation__) */
