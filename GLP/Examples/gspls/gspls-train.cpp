//
//  gspls-train.cpp
//  GLP
//
//  Created by Zheng Shao on 11/17/15.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
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

#include "gspls-train.hpp"

string usage()
{
    return
    "gspls is a part of GLP v1.0\n\n"
    "   Usage: gspls-train [-mLnkfytsbv] [gsp file]\n\n"
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

MatrixXd& getTrainMat()
{
    
}

MatrixXd& SLGsplsTrain::getTrainRespMat()
{
    return this->_trainRespMat;
}

MatrixXd& getValidMat()
{
    return this->_trainMat[this->validStart] 
}

MatrixXd& getValidRespMat()
{
    return this->_vaildRespMat;
}

void SLGsplsTrain::timeStart()
{
    this->_timeStart = ptime(microsec_clock::local_time());
}

void SLGsplsTrain::timeEnd()
{
    this->_timeEnd = ptime(microsec_clock::local_time());
}

time_duration SLGsplsTrain::timeDuration()
{
    return time_duration(this->_timeEnd - this->_timeStart);
}

SLGsplsTrain* SLGsplsTrain::initWithParam(SLGsplsTrainParameters& param)
{
    SLGsplsTrain* train = new SLGsplsTrain();
    train->setParam(param);
    param->colMode->setDelegate(train);
    
    SLSparsePls::SLSparsePlsParameters splsParam;
    splsParam.verbose = param.verbose;
    splsParam.mode    = param.mode;
    splsParam.colMode = param.colMode;
    
    SLGspan::SLGspanParameters gspanParam;
    gspanParam.minsup             = param.minsup;
    gspanParam.maxpat             = param.maxpat;
    gspanParam.topk               = param.topk;
    gspanParam.doesUseMemoryBoost = param.boost;
    gspanParam.gspFilename        = string(param.trainFile);
    
    train->_splsParam  = splsParam;
    train->_gspanParam = gspanParam
    train->_gspls      = SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam);

    SLGspan trainGspan   = train->_gspls->getGraphMining();
    SLGspan validGspan   = trainGspan;
    vector<Graph> graphs = trainGspan->getTranscation();
    
    // use 10% data as validation
    vector<Graph> validGraphs(graphs.end()-this->_validLength+1, graphs.end());
    validGspan.setTransaction(validGraphs);
    
    // use left data as train
    vector<Graph> trainGraphs(graphs.begin(), graphs.end()-this->_validLength);
    trainGspan.setTransaction(trainGraphs);
    
    return train;
}

void SLGsplsTrain::setParam(SLGsplsTrainParameters& param)
{
    this->param = param;
    
    if (param.respFile != NULL)
        EigenExt::loadMatrixFromFile(this->_trainRespMat, this->_param.respFile);
    
    this->_validLength  = floor(this->_trainRespMat.rows() * SLGsplsTrain::VALID_RATIO);
    this->_validRespMat = this->_trainRespMat.bottomRows(this->_validLength);
    this->_trainRespMat = this->_trainRespMat.topRows(this->_trainRespMat.rows() - this->_validLength);
}

// TODO: consider strategy pattern
void SLGsplsTrain::gpsan()
{
    gspanResult = gspls.search(this->_param->colMode->getSelectedColumn(),
                               SLGraphMiningTasktypeTrain,
                               this->_param->colMode->getGraphMingResultType());
    MatrixXd x = get<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]);
}

void SLGsplsTrain::spls()
{
    
}

void SLGsplsTrain::calcResidual()
{
    
}

bool SLGsplsTrain::doesOverfit()
{
    
}

int main(int argc, char* argv[])
{
    // parse arguments
    SLGsplsTrainParameters* param = SLGsplsTrainParameters::initWithArgs(argc, argv);
    if(param == NULL){
        cerr << usage() << endl;
        exit(-1);
    }
    
    SLGsplsTrain* train = SLGsplsTrain::initWithParam(*param);
    
    // let's begin
    train->timeStart();
    
    int i = 0;
    while ( i < param->n) {
        // do gspan
        gspanResult = train->gspan();
        
        // spls
        train->spls();
        
        // calc res
        train->calcResidual();
        
        // overfit detection
        if (train->doesOverfit()) break;
    }
    
    // calc time elapsed
    train->timeEnd();
    
    // write result to file
    
    return 0;
}
