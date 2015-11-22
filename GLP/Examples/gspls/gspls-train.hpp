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
    class IMode
    {
        virtual SLMODELRESULTYPE getResultType();
    };
    
    class IColumnSelection
    {
    protected:
        SLGsplsTrain* parent;
        
    public:
        void setDelegate(SLGsplsTrain* train)
        {
            this->parent = train;
        }
        
    protected:
        virtual SLGRAPHMININGTASKTYPE getGraphMingResultType();
        virtual MatrixXd getSelectedColumn();
    };
    
    class ModeRregession : IMode
    {
        SLMODELRESULTYPE getResultType()
        {
            return
                SLModelResultTypeQ2   |
                SLModelResultTypeRSS  |
                SLModelResultTypeBeta |
                SLModelResultTypeAIC  |
                SLModelResultTypeBIC  |
                SLModelResultTypeCOV;
        }
    };
    
    class ModeClassification : IMode
    {
        SLMODELRESULTYPE getResultType()
        {
            return
                SLModelResultTypeRSS  |
                SLModelResultTypeBeta |
                SLModelResultTypeACC  |
                SLModelResultTypeAUC  |
                SLModelResultTypeCOV;
        }
    };
    
    class ColumnSelectionAverage : IColumnSelection
    {
        SLGRAPHMININGTASKTYPE getGraphMingResultType()
        {
            return SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules;
        }
        
        MatrixXd getSelectedColumn()
        {
            if (parent->_trainResidualMat.rows() < 1) parent->_trainResidualMat = parent->_trainRespMat;
            
            VectorXd ResMean(parent->_trainResidualMat.rows());
            ResMean.setZero();
            
            for (ssize_t i=0; i<parent->_trainResidualMat.cols(); ++i)
                ResMean += parent->_trainResidualMat.col(i);
            
            MatrixXd result = ResMean/parent->_trainResidualMat.cols();
            return result;
        }
    };
    
    class ColumnSelectionRandom : IColumnSelection
    {
        SLGRAPHMININGTASKTYPE getGraphMingResultType()
        {
            return SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules;
        }
        
        MatrixXd getSelectedColumn()
        {
            long randomColumnIndex;
            
            randomColumnIndex = rand();
            parent->_splsParam.randIndex = randomColumnIndex;
            parent->_gspls->setModelParameters(parent->_splsParam);
            return parent->_trainResidualMat.col(randomColumnIndex);
        }
    };
    
    class ColumnSelectionVariance : IColumnSelection
    {
        SLMODELRESULTYPE getModelResultType()
        {
            return SLModelResultTypeQ2   |
            SLModelResultTypeRSS  |
            SLModelResultTypeBeta |
            SLModelResultTypeAIC  |
            SLModelResultTypeBIC  |
            SLModelResultTypeCOV;
        }
        
        SLGRAPHMININGTASKTYPE getGraphMingResultType()
        {
            return SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules;
        }
        
        MatrixXd getSelectedColumn()
        {
            long maxSquaredNormColumn;
            ColVariance(parent->_trainResidualMat).maxCoeff(&maxSquaredNormColumn);
            
            return parent->_trainResidualMat.col(maxSquaredNormColumn);
        }
    };

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
        
        static TrainParameters* initWithArgs(int argc, char* argv[])
        {
            TrainParameters* param = new TrainParameters();
            
            if (argc < 2) {
                return NULL;
            }
            
            static struct option long_options[] =
            {
                {"reg", no_argument, 0, 0},
                {"cla", no_argument, 0, 0},
                {0, 0, 0, 0}
            };
            
            int option_index = 0;
            int opt;
            while ((opt = getopt_long(argc,
                                      (char **)argv,
                                      "L:m:n:k:f:y:t:vbsarp",
                                      long_options,
                                      &option_index)) != -1)
            {
                switch(opt)
                {
                    case 0:
                    {
                        if (long_options[option_index].flag != 0) break;
                        
                        string optname = string(long_options[option_index].name);
                        if (optname == "reg"){
                            param->mode = (IMode*) new ModeRregession();
                        }
                        else if (optname == "cla"){
                            param->mode = (Imode*) new ModeClassification();
                        }
                        else{
                            return NULL;
                        }
                        break;
                    }
                    case 'L':
                        param->maxpat = atoi(optarg);
                        break;
                    case 'm':
                        param->minsup = atoi(optarg);
                        break;
                    case 'n':
                        param->n = atoi(optarg);
                        break;
                    case 'k':
                        param->topk = atoi(optarg);
                        break;
                    case 'f':
                        param->fold = atoi(optarg);
                        break;
                    case 'y':
                        param->trainFile = strdup(optarg);
                        break;
                    case 't':
                        param->resultHistorySize = atoi(optarg);
                        break;
                    case 'v':
                        param->verbose = true;
                        break;
                    case 'b':
                        param->boost = true;
                        break;
                    case 's':
                        param->useShuffledData = true;
                        break;
                    case 'a':
                        param->colMode = (IColumnSelection*) new ColumnSelectionAverage();
                        break;
                    case 'r':
                        param->colMode = (IColumnSelection*) new ColumnSelectionRandom();
                        break;
                    case 'p':
                        param->preProcess = true;
                        break;
                    case '?':
                    default:
                        return NULL;
                }
            }
            
            if (param->mode == NULL)
                param->mode = (IMode*) new ModeRregession();

            if (param->colMode == NULL)
                param->colMode = (IColumnSelection*) new ColumnSelectionVariance();
            
            return param;
        }
        
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
        IMode mode*;
        IColumnSelection* colMode;
    };
    
private:
    TrainParameters _param;
    SLSparsePls::SLSparsePlsParameters _splsParam;
    SLGspan::SLGspanParameters _gspanParam;

    MatrixXd _trainMat;
    MatrixXd _trainRespMat;
    MatrixXd _trainResidualMat;
    MatrixXd _validMat;
    MatrixXd _validRespMat;
    MatrixXd _validResidualMat;
    friend class IColumnSelection;
    
    // use 10% train data as validation
    int _validLength;
    
    ptime _timeStart, _timeEnd;
    SLGlpProduct<SLSparsePls, SLGspan>* _gspls;
    
public:
    static const float VALID_RATIO;
    
public:
    static SLGsplsTrain* initWithParam(TrainParameters&);
    void setParam(TrainParameters&);
    
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
