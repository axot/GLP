//
//  main.cpp
//  GLP
//
//  Created by Zheng Shao on 11/23/15.
//
//

#include <cstdio>
#include "gspls-train.hpp"

using namespace std;

string usage()
{
    return
    "gspls is a part of GLP v1.0\n\n"
    "   Usage: gspls train [-mLnkfytsbv] [gsp file]\n\n"
    " Options: \n"
    "          [--reg | --cla] regression or classification mode\n"
    "          [-m] min frequency of common graphs, default: 2\n"
    "          [-L] max graph size for gspan mining, default: 10\n"
    "          [-n] max iterator number, default: 100\n"
    "          [-k] number of sub graphs abstract by gspan once time, default: 5\n"
    "          [-f] folds of cross Valid, default: 10\n"
    "          [-y] distinct response Y matrix file\n"
    "          [-a] use average residual column, defult is using max variance column\n"
    "          [-r] use random residual column, defult is using max variance column\n"
    "          [-t] the threshold value which used to avoid overfiting default: 3(times)\n"
    "          [-s] shuffle data(preprocess)\n"
    "          [-b] use memory boosting\n"
    "          [-p] centering and scaling label for regression mode\n"
    "          [-v] verbose\n"
    "\n"
    "  Author: Zheng Shao\n"
    " Contact: axot@axot.org\n"
    "Homepage: http://saigo-www.bio.kyutech.ac.jp/~axot";
}

int main(int argc, char* argv[])
{
    // parse arguments
    SLGsplsTrain::TrainParameters* param = SLGsplsTrain::TrainParameters::initWithArgs(argc, argv);
    if(param == NULL){
        cerr << usage() << endl;
        exit(-1);
    }
    
    SLGsplsTrain* train = SLGsplsTrain::initWithParam(*param);
    
    // let's begin
    train->timeStart();
    
    int i = 0;
    while (i < param->n) {
        cerr << "iter: " << i+1 << endl;
        
        // column selection
        cerr << "column selection" << endl;
        MatrixXd selectedCol = param->colMode->getSelectedColumn(&train->getTrainResidualMat());

        // do gspan
        cerr << "gspan" << endl;
        train->gspanResult = train->gspan(selectedCol);
        
        // spls
        cerr << "spls train" << endl;
        MatrixXd features = get<MatrixXd>(train->gspanResult[SLGraphMiningResultTypeX]);
        train->splsResult = train->spls(features);
       
        // overfit detection
        cerr << "overfit detection" << endl;
        vector<Rule> rules = get< vector<Rule> >(train->gspanResult[SLGraphMiningResultTypeRules]);
        if (train->isOverfit(rules)) break;

        cerr << endl;
        ++i;
    }
    
    // calc time elapsed
    train->timeEnd();
    
    // write result to file
    train->saveResults();

    return 0;
}
