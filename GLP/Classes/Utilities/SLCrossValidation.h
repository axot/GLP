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
#include <vector>
#include <map>
#include <Eigen/Core>
#include "../Models/SLModelStrategy.h"
#include "../Models/SLSparsePls.h"

using namespace std;
using namespace Eigen;

enum{
    SLGLPCROSSVALIDATIONRESULTYPETRAIN      = 1 << 0,
    SLGLPCROSSVALIDATIONRESULTYPEVALIDATION = 1 << 1,
    SLGLPCROSSVALIDATIONRESULTYPETEST       = 1 << 2,
};

typedef unsigned int SLGLPCROSSVALIDATIONRESULTYPE;
typedef vector<SLGlpResult> SLGlpMultipleResults;

class SLGlpCrossValidationResults : public map<SLGLPCROSSVALIDATIONRESULTYPE, SLGlpMultipleResults>
{
public:
    SLGlpCrossValidationResults operator+(SLGlpCrossValidationResults& b)
    {
        if ( this->size() == 0 && b.size() == 0 )
        {
            return *this;
        }
        
        SLGlpCrossValidationResults result;
        SLGlpCrossValidationResults::iterator it;
        if ( b.size() == 0 )
        {
            return *this;
        }
        else if ( this->size() == 0 )
        {
            (*this) = b;
            return *this;
        }
        else {
            for ( it = this->begin(); it != this->end(); ++it )
            {
                SLGLPCROSSVALIDATIONRESULTYPE key = it->first;
                SLGlpMultipleResults value = it->second;
                
                result[key] = value;
                result[key].insert(result[key].end(), b[key].begin(), b[key].end());
            }
        }
        return result;
    }
    
    SLGlpCrossValidationResults& operator+=(SLGlpCrossValidationResults& b)
    {
        *this = *this + b;
        return *this;
    }
    
    string print(SLGLPCROSSVALIDATIONRESULTYPE cvType, SLGLPRESULTYPE resultType)
    {
        stringstream output;
        SLGlpMultipleResults::iterator it;
        size_t i = 0;
        for ( it = (*this)[cvType].begin(); it != (*this)[cvType].end(); ++it )
        {
            output << "\n[" << i << "]:"  << endl;
            output << (*it)[resultType]   << endl;
            ++i;
        }
        return output.str();
    }
    
    double mean(SLGLPCROSSVALIDATIONRESULTYPE cvType, SLGLPRESULTYPE resultType)
    {
        SLGlpMultipleResults::iterator it = (*this)[cvType].begin();
        
        MatrixXd sum = (*it)[resultType];
        ++it;
        while ( it != (*this)[cvType].end() )
        {
            sum += (*it)[resultType];
            ++it;
        }
        return sum.mean()/(*this)[cvType].size();
    }
};

template <typename T>
class SLCrossValidation
{
public:
    class SLCrossValidationParameters
    {
    public:
        SLCrossValidationParameters() : kFold(10) {}
        
    public:
        int kFold;
        T modelClone;
    };

    SLCrossValidation()
    {
        typename SLCrossValidation<T>::SLCrossValidationParameters param;
        setParameters(param);
    }
    
    /* K-fold Cross Vaildate: defualt k: 10
     * Input
     *      X: X matrix
     *      Y: Y matrix
     *   type: type of results
     *
     * Return: the results stored in mapped structure.
     *
     */
    SLGlpCrossValidationResults crossValidation(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type);

    void setParameters(typename SLCrossValidation<T>::SLCrossValidationParameters parameters)
    {
        kFold = parameters.kFold;
        modelClone = parameters.modelClone;
    }

private:
    /* Split method for cross validation:
     
     Discusion: if startIndex < 0, startIndex = total length + startIndex
     if startIndex+length > total length,
     return merged 2 blocks which one part is from begining of the matrix
     */
    MatrixXd cvSplitMatrix(MatrixXd& from, ssize_t startIndex, size_t length) const;
    
    SLGlpMultipleResults crossValidationTrain(const size_t index, const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type);
    SLGlpMultipleResults crossValidationValidationClassify(const size_t index, const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type);
    SLGlpMultipleResults crossValidationTestClassify(const size_t index, const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type);

private:
    // assignable parameters via setParameters() method
    int kFold;
    T modelClone;
    
    // not assignable parameters
    VectorXi randomIndexs;
    vector<T> cvModels;
    
    // for Sparse PLS
    vector<MatrixXd> cvXValidationData;
    vector<MatrixXd> cvXTestData;
};

// Implementation
template <typename T>
SLGlpCrossValidationResults SLCrossValidation<T>::crossValidation(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type)
{
    ASSERT(kFold <= X.rows(), "K-Fold was too big than X rows.");
    ASSERT(kFold >= 4, "K-Fold was too small, it must bigger than 4.");
    
    if (randomIndexs.size() == 0)
    {
        srand((unsigned int)time(NULL));
        randomIndexs.setLinSpaced(X.rows(), 0, (int)X.rows());
        random_shuffle(randomIndexs.derived().data(), randomIndexs.derived().data()+randomIndexs.derived().size());
        
        for (size_t i=0; i<kFold; ++i)
        {
            T model;
            model.setParameters(modelClone.getParameters());
            cvModels.push_back(model);
        }
    }
    
    MatrixXd shuffledX = X;
    MatrixXd shuffledY = Y;
    
#pragma omp parallel for
    for (size_t i=0; i<X.rows(); ++i)
    {
        shuffledX.row(i) = X.row(randomIndexs[i]);
        shuffledY.row(i) = Y.row(randomIndexs[i]);
    }
    
    size_t oneUnitLength = shuffledX.rows()/kFold;
    SLGlpCrossValidationResults result;
    for (size_t i = 0; i < kFold; ++i)
    {
        MatrixXd trainX = cvSplitMatrix(shuffledX, -oneUnitLength*i, (kFold-2)*oneUnitLength);
        MatrixXd trainY = cvSplitMatrix(shuffledY, -oneUnitLength*i, (kFold-2)*oneUnitLength);
        
        MatrixXd validationX = cvSplitMatrix(shuffledX, shuffledX.rows()-oneUnitLength*(i+2), oneUnitLength);
        MatrixXd validationY = cvSplitMatrix(shuffledY, shuffledY.rows()-oneUnitLength*(i+2), oneUnitLength);
        
        MatrixXd testX = cvSplitMatrix(shuffledX, shuffledX.rows()-oneUnitLength*(i+1), oneUnitLength);
        MatrixXd testY = cvSplitMatrix(shuffledY, shuffledY.rows()-oneUnitLength*(i+1), oneUnitLength);
        
        SLGlpCrossValidationResults eachResult;
        eachResult[SLGLPCROSSVALIDATIONRESULTYPETRAIN]      = crossValidationTrain(i, trainX, trainY, type);
        eachResult[SLGLPCROSSVALIDATIONRESULTYPEVALIDATION] = crossValidationValidationClassify(i, validationX, validationY, type);
        eachResult[SLGLPCROSSVALIDATIONRESULTYPETEST]       = crossValidationTestClassify(i, testX, testY, type);
        
        result += eachResult;
    }
    return result;
}

template <typename T>
MatrixXd SLCrossValidation<T>::cvSplitMatrix(MatrixXd& from, ssize_t startIndex, size_t length) const
{
    if (startIndex < 0){
        startIndex = from.rows() + startIndex;
    }
    if (startIndex+length <= from.rows()) {
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
                                                                SLGLPRESULTYPE type)
{
    SLGlpMultipleResults result;
    result.push_back(cvModels[index].train(X, Y, type));
    return result;
}

template <typename T>
SLGlpMultipleResults SLCrossValidation<T>::crossValidationValidationClassify(const size_t index,
                                                                             const MatrixXd& X,
                                                                             const MatrixXd& Y,
                                                                             SLGLPRESULTYPE type)
{
    SLGlpMultipleResults result;
    result.push_back(cvModels[index].classify(X, Y, type));
    return result;
}

template <typename T>
SLGlpMultipleResults SLCrossValidation<T>::crossValidationTestClassify(const size_t index,
                                                                       const MatrixXd& X,
                                                                       const MatrixXd& Y,
                                                                       SLGLPRESULTYPE type)
{
    SLGlpMultipleResults result;
    result.push_back(cvModels[index].classify(X, Y, type));
    return result;
}

// Special Version for Sparse PLS
template<>
SLGlpMultipleResults SLCrossValidation<SLSparsePls>::crossValidationValidationClassify(const size_t index,
                                                                                       const MatrixXd& X,
                                                                                       const MatrixXd& Y,
                                                                                       SLGLPRESULTYPE type)
{
    if(cvXValidationData.size() < cvModels.size()){
        cvXValidationData.push_back(X);
    }else{
        size_t oldCols = cvXValidationData[index].cols();
        cvXValidationData[index].conservativeResize(X.rows(), oldCols+X.cols());
        cvXValidationData[index] << cvXValidationData[index].leftCols(oldCols), X;
    }
    
    SLGlpMultipleResults result;
    result.push_back(cvModels[index].classify(cvXValidationData[index], Y, type));
    return result;
}

template<>
SLGlpMultipleResults SLCrossValidation<SLSparsePls>::crossValidationTestClassify(const size_t index,
                                                                                 const MatrixXd& X,
                                                                                 const MatrixXd& Y,
                                                                                 SLGLPRESULTYPE type)
{
    if(cvXTestData.size() < cvModels.size()){
        cvXTestData.push_back(X);
    }else{
        size_t oldCols = cvXTestData[index].cols();
        cvXTestData[index].conservativeResize(X.rows(), oldCols+X.cols());
        cvXTestData[index] << cvXTestData[index].leftCols(oldCols), X;
    }
    
    SLGlpMultipleResults result;
    result.push_back(cvModels[index].classify(cvXTestData[index], Y, type));
    return result;
}

#endif /* defined(__GLP__SLCrossValidation__) */
