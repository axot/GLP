//
//  gspls-train.hpp
//  GLP
//
//  Created by Zheng Shao on 11/17/15.
//  Copyright (c) 2012-2015 Saigo Laboratoire. All rights reserved.
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

#ifndef gspls_train_hpp
#define gspls_train_hpp

#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <boost/typeof/typeof.hpp>
#include <boost/format.hpp>
#include <boost/random.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cfloat>

#include <GLP/SLGlp.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

class SLPlsColumnSelectionAverage : public IColumnSelection<SLSparsePls>
{
private:
    MatrixXd _selectedCol;
    
public:
    virtual MatrixXd getSelectedColumn(MatrixXd* mat = NULL);
};

class SLPlsColumnSelectionRandom : public IColumnSelection<SLSparsePls>
{
private:
    MatrixXd _selectedCol;
    
public:
    virtual MatrixXd getSelectedColumn(MatrixXd* mat = NULL);
};

class SLPlsColumnSelectionVariance : public IColumnSelection<SLSparsePls>
{
private:
    MatrixXd _selectedCol;
    
public:
    virtual MatrixXd getSelectedColumn(MatrixXd* mat = NULL);
};

class SLGsplsTrain
{
public:
    class TrainParameters
    {
    public:
        TrainParameters() :
        maxpat(10),
        minsup(2),
        n(100),
        topk(1),
        trainFile(NULL),
        respFile(NULL),
        verbose(false),
        boost(false),
        mode(NULL),
        colMode(NULL),
        validLength(-1)
        {}
        
        static TrainParameters* initWithArgs(int argc, char* argv[]);
        
    public:
        size_t maxpat;
        size_t minsup;
        size_t n;
        size_t topk;
        size_t fold;
        SLResultHistory resultHist;
        char* trainFile;
        char* respFile;
        bool verbose;
        bool boost;
        IMode<SLSparsePls>* mode;
        IColumnSelection<SLSparsePls>* colMode;
        int validLength;
    };
    
private:
    TrainParameters _param;
    SLSparsePls::SLSparsePlsParameters _splsParam;
    SLGspan::SLGspanParameters _gspanParam;

    MatrixXd _featuresMat;
    MatrixXd _trainMat;
    MatrixXd _trainRespMat;
    MatrixXd _trainResidualMat;
    MatrixXd _validMat;
    MatrixXd _validRespMat;
    MatrixXd _validResidualMat;
    
    SLGspan* _validGspan;
    vector<Graph> _validTransaction;
    
    friend class ColumnSelectionAverage;
    friend class ColumnSelectionRandom;
    friend class ColumnSelectionVariance;
    
    bool _isOverfit;

    ptime _timeStart, _timeEnd;
    string _fileSuffix;
    
    SLGlpProduct<SLSparsePls, SLGspan>* _gspls;
    
public:
    SLGraphMiningResult gspanResult;
    SLModelResult splsResult;

    static const float VALID_RATIO;
    
public:
    static SLGsplsTrain* initWithParam(TrainParameters&);
    
    SLGraphMiningResult gspan(MatrixXd& selectedColumn);
    SLModelResult spls(MatrixXd&);
    
    void saveResults(size_t index);
    bool isOverfit(vector<Rule> rules);

    void timeStart();
    void timeEnd();
    time_duration timeDuration();
    
    void setFileSuffix(string);
    
    MatrixXd& getTrainMat();
    MatrixXd& getTrainRespMat();
    MatrixXd& getTrainResidualMat();
    MatrixXd& getValidMat();
    MatrixXd& getValidRespMat();
    
private:
    SLGsplsTrain(){};
};

#endif /* gspls_train_hpp */
