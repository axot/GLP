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

class SLGsplsTrain
{
    class ColumnSelectionBase : public IColumnSelection<SLGsplsTrain>
    {
    public:
        virtual SLGRAPHMININGTASKTYPE getGraphMingResultType();
    };
        
    class ColumnSelectionAverage : ColumnSelectionBase
    {
        SLGRAPHMININGTASKTYPE getGraphMingResultType()
        {
            return SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules;
        }
        
        MatrixXd getSelectedColumn()
        {
            if (dataSource->_trainResidualMat.rows() < 1)
                dataSource->_trainResidualMat = dataSource->_trainRespMat;
            
            VectorXd ResMean(dataSource->_trainResidualMat.rows());
            ResMean.setZero();
            
            for (ssize_t i=0; i<dataSource->_trainResidualMat.cols(); ++i)
                ResMean += dataSource->_trainResidualMat.col(i);
            
            MatrixXd result = ResMean/dataSource->_trainResidualMat.cols();
            return result;
        }
    };
    
    class ColumnSelectionRandom : ColumnSelectionBase
    {
        SLGRAPHMININGTASKTYPE getGraphMingResultType()
        {
            return SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules;
        }
        
        MatrixXd getSelectedColumn()
        {
            long randomColumnIndex;
            
            randomColumnIndex = rand();
            dataSource->_splsParam.randIndex = randomColumnIndex;
            dataSource->_gspls->setModelParameters(dataSource->_splsParam);
            return dataSource->_trainResidualMat.col(randomColumnIndex);
        }
    };
    
    class ColumnSelectionVariance : ColumnSelectionBase
    {
        SLGRAPHMININGTASKTYPE getGraphMingResultType()
        {
            return SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules;
        }
        
        MatrixXd getSelectedColumn()
        {
            long maxSquaredNormColumn;
            ColVariance(dataSource->_trainResidualMat).maxCoeff(&maxSquaredNormColumn);
            
            return dataSource->_trainResidualMat.col(maxSquaredNormColumn);
        }
    };

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
        ColumnSelectionBase* colMode;
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
    
    void setParam(TrainParameters&);
    
    MatrixXd calcResidual(SLModelResult& trainResult);
    
    bool isOverfit();
    
    MatrixXd gspan();
    
    SLModelResult spls(MatrixXd&);
    
    void timeStart();
    
    void timeEnd();
    
    time_duration timeDuration();
    
    MatrixXd& getTrainMat();
    MatrixXd& getTrainRespMat();
    MatrixXd& getValidMat();
    MatrixXd& getValidRespMat();
    
private:
    SLGsplsTrain(){};
};

const float SLGsplsTrain::VALID_RATIO = 0.1f;
#endif /* gspls_train_hpp */
