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
const float SLGsplsTrain::VALID_RATIO = 0.1f;

MatrixXd SLPlsColumnSelectionAverage::getSelectedColumn(MatrixXd* mat)
{
    if (mat == NULL) return _selectedCol;
    
    VectorXd ResMean(mat->rows());
    ResMean.setZero();
    
    for (ssize_t i=0; i<mat->cols(); ++i)
        ResMean += mat->col(i);
    
    _selectedCol = ResMean/mat->cols();
    return _selectedCol;
}

MatrixXd SLPlsColumnSelectionRandom::getSelectedColumn(MatrixXd* mat)
{
    if (mat == NULL) return _selectedCol;

    _selectedCol = mat->col(rand());
    return _selectedCol;
}

MatrixXd SLPlsColumnSelectionVariance::getSelectedColumn(MatrixXd* mat)
{
    if (mat == NULL) return _selectedCol;
    
    ssize_t selectedColIndex;
    ColVariance((*mat)).maxCoeff(&selectedColIndex);

    _selectedCol = mat->col(selectedColIndex);
    return _selectedCol;
}

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
                param->respFile = strdup(optarg);
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
                param->colMode = (IColumnSelection<SLSparsePls>*) new SLPlsColumnSelectionAverage();
                break;
            case 'r':
                param->colMode = (IColumnSelection<SLSparsePls>*) new SLPlsColumnSelectionRandom();
                break;
            case 'p':
                param->preProcess = true;
                break;
            case '?':
            default:
                return NULL;
        }
    }
    
    param->trainFile = strdup(argv[argc-1]);
    
    if (param->mode == NULL)
        param->mode = (SLPlsModeRregession*) new SLPlsModeRregession();
        
        if (param->colMode == NULL)
            param->colMode = (IColumnSelection<SLSparsePls>*) new SLPlsColumnSelectionVariance();
            
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

MatrixXd& SLGsplsTrain::getTrainResidualMat()
{
    return _trainResidualMat;
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
    train->_param = param;
    
    // init spls parameter
    SLSparsePls::SLSparsePlsParameters splsParam;
    splsParam.verbose = param.verbose;
    splsParam.mode    = param.mode;
    splsParam.colMode = param.colMode;
    
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

    // init _trainResidualMat same as _trainRespMat
    train->_trainResidualMat = train->_trainRespMat;

    // set transaction data
    SLGspan& trainGspan  = train->_gspls->getGraphMining();
    train->_validGspan   = new SLGspan();
    vector<Graph> graphs = trainGspan.getTransaction();
    train->_validGspan->setParameters(gspanParam);

    // set validation transactions
    vector<Graph> validGraphs(graphs.end() - train->_validLength, graphs.end());
    train->_validTransaction = validGraphs;
    
    // use left data as train
    vector<Graph> trainGraphs(graphs.begin(), graphs.end() - train->_validLength);
    trainGspan.setTransaction(trainGraphs);
    
    // graph changed, rebuild dfs tree;
    trainGspan.rebuildDFSTree();
    
    return train;
}

SLGraphMiningResult SLGsplsTrain::gspan(MatrixXd& selectedColumn)
{
    return _gspls->search(selectedColumn,
                          SLGraphMiningTasktypeTrain,
                          SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules);
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

    _trainResidualMat = get<MatrixXd>(result[SLModelResultTypeRes]);
    
    return result;
}

bool SLGsplsTrain::isOverfit(vector<Rule> rules)
{
    MatrixXd result(_validTransaction.size(), rules.size());
    
    for (size_t i = 0; i < _validTransaction.size(); ++i) {
        result.row(i) = _validGspan->classify(rules, _validTransaction[i]);
    }
    
    SLModelResult scores = _gspls->classify(result, _validRespMat, _param.mode->getResultType());
    
    cerr << "Valid  Q2: " << scores[SLModelResultTypeQ2]  << endl;
    cerr << "Valid AUC: " << scores[SLModelResultTypeAUC] << endl;
    cerr << "Valid ACC: " << scores[SLModelResultTypeACC] << endl;
    cerr << "Valid RSS: " << scores[SLModelResultTypeRSS] << endl;

    // TODO: IMP
    static int count = 10;
    return --count ? false : true;
}

void SLGsplsTrain::saveResults()
{
    ofstream outDFS("DFS.txt", ios::out);
    for ( size_t i = 0; i < this->_param.topk*3; ++i )
        outDFS << get< vector<Rule> >(gspanResult[SLGraphMiningResultTypeRules])[i].dfs << endl;
    outDFS.close();
}

