//
//  gspls-train.cpp
//  GLP
//
//  Created by Zheng Shao on 11/17/15.
//  Copyright (c) 2012-2015 Saigo Laboratoire. All rights reserved.
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
                              "L:m:n:k:f:y:t:o:vbsarp",
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
                    cout << "Regression Mode" << endl;
                    param->mode = (SLPlsModeRegression*) new SLPlsModeRegression();
                }
                else if (optname == "cla"){
                    cout << "Classification Mode" << endl;
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
                param->resultHist.length = atoi(optarg);
                break;
            case 'o':
                param->validLength = atoi(optarg);
                break;
            case 'v':
                param->verbose = true;
                break;
            case 'b':
                param->boost = true;
                break;
            case 'a':
                param->colMode = (IColumnSelection<SLSparsePls>*) new SLPlsColumnSelectionAverage();
                break;
            case 'r':
                param->colMode = (IColumnSelection<SLSparsePls>*) new SLPlsColumnSelectionRandom();
                break;
            case '?':
            default:
                return NULL;
        }
    }
    
    param->trainFile = strdup(argv[argc-1]);
    
    if (param->mode == NULL)
        param->mode = (SLPlsModeRegression*) new SLPlsModeRegression();
        
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

void SLGsplsTrain::setFilePrefix(string prefix)
{
    _filePrefix = prefix;
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
    if (param.validLength < 0){
        param.validLength  = floor(train->_trainRespMat.rows() * VALID_RATIO);
    }
    // setup response data
    train->_validRespMat = train->_trainRespMat.bottomRows(param.validLength);
    train->_trainRespMat = train->_trainRespMat.topRows(train->_trainRespMat.rows() - param.validLength);

    // init _trainResidualMat same as _trainRespMat
    train->_trainResidualMat = train->_trainRespMat;

    // set transaction data
    SLGspan& trainGspan  = train->_gspls->getGraphMining();
    train->_validGspan   = new SLGspan();
    vector<Graph> graphs = trainGspan.getTransaction();
    train->_validGspan->setParameters(gspanParam);

    // set validation transactions
    vector<Graph> validGraphs(graphs.end() - param.validLength, graphs.end());
    train->_validTransaction = validGraphs;
    
    // use left data as train
    vector<Graph> trainGraphs(graphs.begin(), graphs.end() - param.validLength);
    trainGspan.setTransaction(trainGraphs);
    
    // graph changed, rebuild dfs tree;
    trainGspan.rebuildDFSTree();
    
    // outout file prefix
    string prefix = (format("gspls_m%d_L%d_n%d_k%d_t%d_") %
                            param.minsup                  %
                            param.maxpat                  %
                            param.n                       %
                            param.topk                    %
                            param.resultHist.length
                     ).str();
    train->setFilePrefix(prefix);
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
    
    SLMODELRESULTYPE types = _param.mode->getResultType();
    SLModelUtility::printResult(types, result);

    return result;
}

bool SLGsplsTrain::isOverfit(vector<Rule> rules)
{
    _param.resultHist.push_front(make_pair(gspanResult, splsResult));
    
    MatrixXd result(_validTransaction.size(), rules.size());
    _validGspan->buildDarts(rules);
    for (size_t i = 0; i < _validTransaction.size(); ++i) {
        result.row(i) = _validGspan->classify(_validTransaction[i]);
    }
    
    SLModelResult scores = _gspls->classify(result, _validRespMat, _param.mode->getResultType());

    size_t overfitCnt = _param.mode->overfitCnt(scores);

    SLMODELRESULTYPE types = _param.mode->getResultType();
    SLModelUtility::printResult(types, scores);

    _isOverfit = overfitCnt >= _param.resultHist.length;
    return _isOverfit;
}

void SLGsplsTrain::saveResults(size_t index)
{
    SLCrossValidationResults oldResult;
    size_t best = index;
    pair<SLGraphMiningResult, SLModelResult> resultPair;
    
    if (_isOverfit == false)
    {
        cerr << "Info: Can not get best result, please set a bigger value for argument n" << endl;
        best = _param.n;
        resultPair = _param.resultHist.front();
    }
    else
        resultPair = _param.resultHist.back();
    
    // best index
    cout << "Best: n = " << best << endl;
    
    // beta
    ofstream outBeta((_filePrefix+"Beta.txt").c_str(), ios::out);
    outBeta.precision(12);
    outBeta.flags(ios::left);
    outBeta << resultPair.second[SLModelResultTypeBeta] << endl;
    cout << "Beta" << endl << get< MatrixXd >(resultPair.second[SLModelResultTypeBeta]) << endl << endl;

    // dfs
    cout << "DFS" << endl;
    vector<Rule> DFSes = get< vector<Rule> >(resultPair.first[SLGraphMiningResultTypeRules]);
    ofstream outDFS((_filePrefix+"DFS.txt").c_str(), ios::out);
    for ( size_t i = 0; i < DFSes.size(); ++i )
    {
        cout   << DFSes[i].dfs << endl;
        outDFS << DFSes[i].dfs << endl;
    }
    cout << endl;
    
    // time
    cout << "duration: " << timeDuration() << endl;
}
