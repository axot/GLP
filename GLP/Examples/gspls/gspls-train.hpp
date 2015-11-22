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

class SLGsplsTrainParameters
{
public:
    SLGsplsTrainParameters() :
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
        mode(PLSMODEREG),
        colMode(PLSCOLSELVAR)
        {}
    
    static SLGsplsTrainParameters* initWithArgs(int argc, char* argv[])
    {
        SLGsplsTrainParameters* param = new SLGsplsTrainParameters();
        
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
                        param->mode = PLSMODEREG;
                    }
                    else if (optname == "cla"){
                        param->mode = PLSMODECLA;
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
                    param->trainRespFile = strdup(optarg);
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
                    param->colMode = PLSCOLSELAVG;
                    break;
                case 'r':
                    param->colMode = PLSCOLSELRAND;
                    break;
                case 'p':
                    param->preProcess = true;
                    break;
                case '?':
                default:
                    return NULL;
            }
        }
        
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
    SLPLSMODE mode;
    SLPLSCOLSEL colMode;
};

class SLGsplsTrain
{
private:
    SLGsplsTrainParameters _param;
    MatrixXd _trainMat;
    MatrixXd _trainRespMat;
    ptime _timeStart, _timeEnd;
    SLGlpProduct<SLSparsePls, SLGspan>* _gspls;
    
public:
    int validStart;   
    int validLength;

public:
    static SLGsplsTrain* initWithParam(SLGsplsTrainParameters&);
    
    void timeStart();
    
    void timeEnd();
    
    time_duration timeDuration();
    
    MatrixXd& getTrainMat();
    MatrixXd& getTrainRespMat();
    MatrixXd& getValidMat();
    MatrixXd& getValidRespMat();
    
    SLMODELRESULTYPE getResultType();
    
private:
    SLGsplsTrain(){};
};
#endif /* gspls_train_hpp */
