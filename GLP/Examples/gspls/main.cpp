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
    SLGraphMiningResult gspanResult;
    while (i < param->n) {
        // column selection
        MatrixXd selectedCol = param->colMode->getSelectedColumn(&train->getTrainResidualMat());
        
        // do gspan
        gspanResult = train->gspan(selectedCol);
        MatrixXd x  = get<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]);

        // spls
        SLModelResult trainResult = train->spls(x);
        
        // overfit detection
        if (train->isOverfit()) break;
        
        ++i;
    }
    
    // calc time elapsed
    train->timeEnd();
    
    // write result to file
    ofstream outDFS("DFS.txt", ios::out);
    for ( size_t i = 0; i < param->topk*3; ++i )
        outDFS << get< vector<Rule> >(gspanResult[SLGraphMiningResultTypeRules])[i].dfs << endl;
    outDFS.close();

    return 0;
}
