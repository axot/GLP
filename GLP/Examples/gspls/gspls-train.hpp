//
//  gspls-train.hpp
//  GLP
//
//  Created by Zheng Shao on 11/17/15.
//
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
        resultHistorySize(3),
        trainFile(NULL),
        respFile(NULL),
        verbose(false),
        boost(false),
        useShuffledData(false),
        preProcess(false),
        mode(NULL),
        colMode(NULL)
        {}
        
        static TrainParameters* initWithArgs(int argc, char* argv[]);
        
    public:
        size_t maxpat;
        size_t minsup;
        size_t n;
        size_t topk;
        size_t fold;
        size_t resultHistorySize;
        char* trainFile;
        char* respFile;
        bool verbose;
        bool boost;
        bool useShuffledData;
        bool preProcess;
        IMode<SLSparsePls>* mode;
        IColumnSelection<SLSparsePls>* colMode;
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
    friend class ColumnSelectionAverage;
    friend class ColumnSelectionRandom;
    friend class ColumnSelectionVariance;
    
    // use 10% train data as validation
    int _validLength;

    ptime _timeStart, _timeEnd;
    SLGlpProduct<SLSparsePls, SLGspan>* _gspls;
    
public:
    static const float VALID_RATIO;
    
public:
    static SLGsplsTrain* initWithParam(TrainParameters&);
            
    bool isOverfit();
    
    SLGraphMiningResult gspan(MatrixXd& selectedColumn);
    
    SLModelResult spls(MatrixXd&);
    
    void timeStart();
    
    void timeEnd();
    
    time_duration timeDuration();
    
    MatrixXd& getTrainMat();
    MatrixXd& getTrainRespMat();
    MatrixXd& getTrainResidualMat();
    MatrixXd& getValidMat();
    MatrixXd& getValidRespMat();
    
private:
    SLGsplsTrain(){};
};

#endif /* gspls_train_hpp */
