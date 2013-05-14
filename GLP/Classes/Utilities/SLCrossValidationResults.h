//
//  SLCrossValidationResults.h
//  GLP
//
//  Created by Zheng Shao on 1/4/13.
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

#ifndef __GLP__SLCrossValidationResults__
#define __GLP__SLCrossValidationResults__

#include <map>
#include <iostream>
#include <boost/format.hpp>
#include "../SLUtility.h"

using namespace std;
using namespace Eigen;
using namespace boost;

enum{
    SLCrossValidationResultTypeTrain      = 1 << 0,
    SLCrossValidationResultTypeValidation = 1 << 1,
    SLCrossValidationResultTypeTest       = 1 << 2,
};

typedef unsigned int SLCROSSVALIDATIONRESULTYPE;
typedef vector<SLModelResult> SLGlpMultipleResults;

class SLCrossValidationResults : public map<SLCROSSVALIDATIONRESULTYPE, SLGlpMultipleResults>
{
public:
    SLCrossValidationResults& operator+=(SLCrossValidationResults& b)
    {
        *this = *this + b;
        return *this;
    }

    /* Display Results of Model
     * Input
     *      results: Cross Validation Results
     *   resultType: type of results
     */
    void show(SLMODELRESULTYPE resultType)
    {
        vector < string > stage(3);
        stage[0] = "Train";
        stage[1] = "Validation";
        stage[2] = "Test";
        
        unsigned int currentStage = SLCrossValidationResultTypeTrain;
        for ( int i = 0; i < 3; i++, currentStage <<= 1)
        {
            cout << stage[i] << " " << endl;
            if ( resultType & SLModelResultTypeQ2 )
                cout << format("%8s: %2.8f") % "Q2" % mean(currentStage, SLModelResultTypeQ2);
            
            if ( resultType & SLModelResultTypeRSS )
                cout << format("%8s: %14.8f") % "RSS" % mean(currentStage, SLModelResultTypeRSS);
            
            if ( resultType & SLModelResultTypeACC )
                cout << format("%8s: %2.8f") % "ACC" % mean(currentStage, SLModelResultTypeACC);
            
            if ( resultType & SLModelResultTypeAUC )
                cout << format("%8s: %2.8f") % "AUC" % mean(currentStage, SLModelResultTypeAUC);

            cout << endl;
        }
        cout << endl << endl;
    }
    
    /* Print Cross Vaildate Results:
     * Input
     *     cvType: cross validation type: (train, validation, test)
     * resultType: type of results
     *
     * Return: the result of the cross validation type
     *
     * Discussion: only one type of each cvType and resultType can be used
     */
    string print(SLCROSSVALIDATIONRESULTYPE cvType, SLMODELRESULTYPE resultType)
    {
        ASSERT(SLUtility::isIncludedOnlyOneType(cvType), "only one of cross vaildate type can be calculate");
        ASSERT(SLUtility::isIncludedOnlyOneType(resultType), "only one of cross result type can be calculate");

        stringstream output;
        output.precision(12);
        SLGlpMultipleResults::iterator it;
        size_t i = 0;
        for ( it = (*this)[cvType].begin(); it != (*this)[cvType].end(); ++it )
        {
            output << "\n[" << i << "]:"  << endl;
            output << get<MatrixXd>((*it)[resultType]) << endl;
            ++i;
        }
        return output.str();
    }
    
    /* Each Mean of Cross Vaildate Results:
     * Input
     *     cvType: cross validation type: (train, validation, test)
     * resultType: type of results
     *
     * Return: the mean of k-fold cross validation
     *
     * Discussion: only one type of each cvType and resultType can be used
     */
    VectorXd eachMean(SLCROSSVALIDATIONRESULTYPE cvType, SLMODELRESULTYPE resultType)
    {
        ASSERT(SLUtility::isIncludedOnlyOneType(cvType), "only one of cross vaildate type can be calculate");
        ASSERT(SLUtility::isIncludedOnlyOneType(resultType), "only one of cross result type can be calculate");
        
        SLGlpMultipleResults::iterator it = (*this)[cvType].begin();
        
        long cols = (*this)[cvType].size();
        VectorXd mean(cols);
        
        int i = 0;
        while ( it != (*this)[cvType].end() )
        {
            mean[i] = get<MatrixXd>((*it)[resultType]).mean();
            ++i;
            ++it;
        }
        return mean;
    }

    /* Mean of Cross Vaildate Results:
     * Input
     *     cvType: cross validation type: (train, validation, test)
     * resultType: type of results
     *
     * Return: the result whole mean of the cross validation type
     *
     * Discussion: only one type of each cvType and resultType can be used
     */
    double mean(SLCROSSVALIDATIONRESULTYPE cvType, SLMODELRESULTYPE resultType)
    {
        ASSERT(SLUtility::isIncludedOnlyOneType(cvType), "only one of cross vaildate type can be calculated");
        ASSERT(SLUtility::isIncludedOnlyOneType(resultType), "only one of cross result type can be calculated");

        SLGlpMultipleResults::iterator it = (*this)[cvType].begin();
        
        MatrixXd sum = get<MatrixXd>((*it)[resultType]);

        ++it;
        while ( it != (*this)[cvType].end() )
        {
            sum += get<MatrixXd>((*it)[resultType]);
            ++it;
        }
        return sum.mean()/(*this)[cvType].size();
    }
    
    SLCrossValidationResults operator+(SLCrossValidationResults& b)
    {
        if ( this->size() == 0 && b.size() == 0 )
        {
            return *this;
        }
        
        SLCrossValidationResults result;
        SLCrossValidationResults::iterator it;
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
                SLCROSSVALIDATIONRESULTYPE key = it->first;
                SLGlpMultipleResults value = it->second;
                
                result[key] = value;
                result[key].insert(result[key].end(), b[key].begin(), b[key].end());
            }
        }
        return result;
    }
};

#endif
