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

class SLGsplsTrainParameters
{
public:
    SLGsplsTrainParameters() :
        maxpat(10),
        minsup(2),
        n(100),
        topk(1),
        resultHistorySize(3),
        yfile(NULL),
        gspfile(NULL),
        verbose(false),
        boost(false),
        useShuffledData(false),
        useAverageCol(false),
        useRandomCol(false),
        preProcess(false),
        modeReg(false),
        modeCla(false)
        {}
    
    static SLGsplsTrainParameters* initWithArgs(int argc, char* argv[]);
    
public:
    size_t maxpat;
    size_t minsup;
    size_t n;
    size_t topk;
    size_t fold;
    size_t resultHistorySize;
    char *yfile;
    char *gspfile;
    bool verbose;
    bool boost;
    bool useShuffledData;
    bool useAverageCol;
    bool useRandomCol;
    bool preProcess;
    bool modeReg;
    bool modeCla;
};

#endif /* gspls_train_hpp */
