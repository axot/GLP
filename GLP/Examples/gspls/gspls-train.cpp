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

SLGsplsTrain::TrainParameters* SLGsplsTrain::TrainParameters::initWithArgs(int argc, char* argv[])
{
    SLGsplsTrain::TrainParameters* param = new SLGsplsTrain::TrainParameters();
    
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
                    param->mode = (SLPlsModeRregession*) new SLPlsModeRregession();
                }
                else if (optname == "cla"){
                    param->mode = (SLPlsModeClassification*) new SLPlsModeClassification();
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
                param->colMode = (ColumnSelectionBase*) new ColumnSelectionAverage();
                break;
            case 'r':
                param->colMode = (ColumnSelectionBase*) new ColumnSelectionRandom();
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
        param->mode = (SLPlsModeRregession*) new SLPlsModeRregession();
        
        if (param->colMode == NULL)
            param->colMode = (ColumnSelectionBase*) new ColumnSelectionVariance();
            
            return param;
}

MatrixXd& SLGsplsTrain::getTrainRespMat()
{
    return _trainRespMat;
}

MatrixXd& SLGsplsTrain::getValidRespMat()
{
    return _validRespMat;
}

void SLGsplsTrain::timeStart()
{
    _timeStart = ptime(microsec_clock::local_time());
}

void SLGsplsTrain::timeEnd()
{
    _timeEnd = ptime(microsec_clock::local_time());
}

time_duration SLGsplsTrain::timeDuration()
{
    return time_duration(_timeEnd - _timeStart);
}

SLGsplsTrain* SLGsplsTrain::initWithParam(TrainParameters& param)
{
    SLGsplsTrain* train = new SLGsplsTrain();
    train->setParam(param);
    
    // assign data source
    param.colMode->dataSource = train;
    
    // init spls parameter
    SLSparsePls::SLSparsePlsParameters splsParam;
    splsParam.verbose = param.verbose;
    splsParam.mode    = (IMode<SLSparsePls>*)param.mode;
    splsParam.colMode = (IColumnSelection<SLSparsePls>*)param.colMode;
    
    // init gspan parameter
    SLGspan::SLGspanParameters gspanParam;
    gspanParam.minsup             = param.minsup;
    gspanParam.maxpat             = param.maxpat;
    gspanParam.topk               = param.topk;
    gspanParam.doesUseMemoryBoost = param.boost;
    gspanParam.gspFilename        = string(param.trainFile);
    
    train->_splsParam  = splsParam;
    train->_gspanParam = gspanParam;
    train->_gspls      = SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam);

    // load train gsp file
    if (param.respFile != NULL)
        EigenExt::loadMatrixFromFile(train->_trainRespMat, train->_param.respFile);
    
    // calculate valid data length
    train->_validLength  = floor(train->_trainRespMat.rows() * VALID_RATIO);

    // setup response data
    train->_validRespMat = train->_trainRespMat.bottomRows(train->_validLength);
    train->_trainRespMat = train->_trainRespMat.topRows(train->_trainRespMat.rows() - train->_validLength);

    // set transaction data
    SLGspan trainGspan   = train->_gspls->getGraphMining();
    SLGspan validGspan   = trainGspan;
    vector<Graph> graphs = trainGspan->getTranscation();
    
    // use 10% data as validation
    vector<Graph> validGraphs(graphs.end() - train->_validLength + 1, graphs.end());
    validGspan.setTransaction(validGraphs);
    
    // use left data as train
    vector<Graph> trainGraphs(graphs.begin(), graphs.end() - train->_validLength);
    trainGspan.setTransaction(trainGraphs);
    
    return train;
}

MatrixXd SLGsplsTrain::gspan()
{
    SLGraphMiningResult gspanResult = _gspls->search(_param.colMode->getSelectedColumn(),
                                                    SLGraphMiningTasktypeTrain,
                                                    _param.colMode->getGraphMingResultType());
    MatrixXd x = get<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]);
    return x;
}

SLModelResult SLGsplsTrain::spls(MatrixXd& feature)
{
    SLModelResult result = _gspls->train(feature, _trainRespMat, _param.mode->getResultType());
    
    long rows    = feature.rows();
    long cols    = feature.cols();
    long preCols = _featuresMat.cols();
    _featuresMat.conservativeResize(rows, preCols+cols);
    _featuresMat.rightCols(cols).setZero();
    _featuresMat << _featuresMat.leftCols(preCols), feature;

    return result;
}

MatrixXd SLGsplsTrain::calcResidual(SLModelResult& trainResult)
{
    _trainResidualMat = _trainRespMat - _featuresMat*get<MatrixXd>(trainResult[SLModelResultTypeBeta]);
    return _trainResidualMat;
}

bool SLGsplsTrain::isOverfit()
{
    
}
