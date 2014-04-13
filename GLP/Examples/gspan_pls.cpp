//
//  gspan_pls.cpp
//  GLP
//
//  Created by Zheng Shao on 4/13/14.
//  Copyright (c) 2014 Saigo Laboratoire. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <time.h>
#include <boost/typeof/typeof.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/random.hpp>
#include <cfloat>
#include <GLP/SLGlp.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

void usage()
{
    cerr <<
    "gpsan_pls is a part of GLP v1.0\n\n"
    "   Usage: gspls [-mLnkfytsbv] [gsp file]\n\n"
    " Options: \n"
    "          [-m min frequency of common graphs, default: 2]\n"
    "          [-L max graph size for gspan mining, default: 10]\n"
    "          [-n max iterator number, default: 100]\n"
    "          [-k number of sub graphs abstract by gspan once time, default: 5]\n"
    "          [-f folds of cross validation, default: 10]\n"
    "          [-y distinct response Y matrix file]\n"
    "          [-a use average residual column, defult is using max variance column]\n"
    "          [-r use random residual column, defult is using max variance column]\n"
    "          [-t the threshold value which used to avoid overfiting default: 3(times)]\n"
    "          [-s shuffle data(preprocess)]\n"
    "          [-b use memory boosting]\n"
    "          [-v verbose]\n\n"
    "  Author: Zheng Shao\n"
    " Contact: axot@axot.org\n"
    "Homepage: http://saigo-www.bio.kyutech.ac.jp/~axot"
    << endl;
}

int main(int argc, const char *argv[])
{
    size_t maxpat = 10;
    size_t minsup = 2;
    size_t n = 100;
    size_t topk = 5;
    size_t fold = 10;
    size_t resultHistorySize = 3;
    char *yfile = NULL;
    char *gspfile = NULL;
    bool verbose = false;
    bool boost = false;
    bool useShuffledData = false;
    bool useAverageCol = false;
    bool useRandomCol = false;
    
    if (argc < 2) {
        usage();
        return -1;
    }
    
    int opt;
    while ((opt = getopt(argc, (char **)argv, "L:m:n:k:f:y:t:vbsar")) != -1)
    {
        switch(opt)
        {
            case 'L':
                maxpat = atoi(optarg);
                break;
            case 'm':
                minsup = atoi(optarg);
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'k':
                topk = atoi(optarg);
                break;
            case 'f':
                fold = atoi(optarg);
                break;
            case 'y':
                yfile = strdup(optarg);
                break;
            case 't':
                resultHistorySize = atoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case 'b':
                boost = true;
                break;
            case 's':
                useShuffledData = true;
                break;
            case 'a':
                useAverageCol = true;
                break;
            case 'r':
                useRandomCol = true;
                break;
            default:
                usage();
                return -2;
        }
    }
    
    gspfile = strdup(argv[argc-1]);
    
    ++optind;
    for ( int i = optind; i < argc; i++ )
    {
        printf ("Error: Unknown argument: %s\n", argv[i]);
        usage();
        return -2;
    }
    
    SLSparsePls::SLSparsePlsParameters splsParam;
    
    SLGspan::SLGspanParameters gspanParam;
    gspanParam.minsup = minsup;
    gspanParam.maxpat = maxpat;
    gspanParam.topk   = topk;
    gspanParam.doesUseMemoryBoost = boost;
    gspanParam.gspFilename        = string(gspfile);
    
    BOOST_AUTO(gspls, (*SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam)));
    
    SLCrossValidation<SLSparsePls>::SLCrossValidationParameters cvParam;
    cvParam.useShuffledData     = useShuffledData;
    cvParam.kFold               = fold;
    cvParam.resultHistorySize   = resultHistorySize;
    
    gspls.setCrossValidationParameters(cvParam);
    
    stringstream fileSuffix;
    fileSuffix << format("gspan_pls_m%d_L%d_n%d_k%d_f%d_t%d_")  %
    gspanParam.minsup                 %
    gspanParam.maxpat                 %
    n                                 %
    gspanParam.topk                   %
    cvParam.kFold                     %
    cvParam.resultHistorySize;
    
    MatrixXd X, Y, Res;
    
    if ( yfile != NULL )
        EigenExt::loadMatrixFromFile(Y, yfile);
    else
        Y = get<MatrixXd>(gspls.getInnerValues(SLGraphMiningInnerValueY)[SLGraphMiningInnerValueY]);
    
    Y = Center(Y);
    Res = Y;
    
    double minRSS = DBL_MAX;
    size_t overfitCount = 0;
    
    ptime time_start(microsec_clock::local_time());
    SLGraphMiningResult gspanResult;
    
    mt19937 gen( static_cast<unsigned long>(time(NULL)));
    uniform_int<> dist(0, Y.cols()-1);
    variate_generator< mt19937&, uniform_int<> > rand( gen, dist );
    
    SLMODELRESULTYPE resultTypes =  SLModelResultTypeQ2   |
                                    SLModelResultTypeRSS  |
                                    SLModelResultTypeBeta |
                                    SLModelResultTypeACC  |
                                    SLModelResultTypeAUC  |
                                    SLModelResultTypeAIC  |
                                    SLModelResultTypeBIC  |
                                    SLModelResultTypeCOV;
    
    gspanResult = gspls.search((VectorXd)NULL, SLGraphMiningTasktypeTrain, SLGraphMiningResultTypeX | SLGraphMiningResultTypeRules);

    X = get<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]);
    
    SLCrossValidationResults cvResult = gspls.crossValidation(X, Y, resultTypes);

    int bestBetaIndex;
    cvResult.eachMean(SLCrossValidationResultTypeTrain, SLModelResultTypeRSS).minCoeff(&bestBetaIndex);
    Res = Y - X*get<MatrixXd>(cvResult[SLCrossValidationResultTypeTrain][bestBetaIndex][SLModelResultTypeBeta]);
    
    cvResult.showSummary(resultTypes);
    
    minRSS = cvResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeRSS);
    
    ptime time_end(microsec_clock::local_time());
    time_duration duration(time_end - time_start);
    
    SLCrossValidationResults oldResult;
    int best = 1;
    oldResult = gspls.getResultHistory().back();
    
    cout << "Best: n = " <<  best << endl;
    
    ofstream outX((fileSuffix.str()+"Features.txt").c_str(), ios::out);
    if ( overfitCount < cvParam.resultHistorySize )
        outX << X << endl;
    else
        outX << X.leftCols(X.cols()-(cvParam.resultHistorySize)*topk) << endl;
    outX.close();
    
    ofstream outBeta((fileSuffix.str()+"Beta.txt").c_str(), ios::out);
    outBeta.precision(12);
    outBeta.flags(ios::left);
    VectorXd cvQ2(fold);
    for ( int j = 0; j < cvQ2.size(); ++j )
        cvQ2[j] = get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][j][SLModelResultTypeQ2]).mean();
    cvQ2.maxCoeff(&bestBetaIndex);
    outBeta << get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][bestBetaIndex][SLModelResultTypeBeta]) << endl;
    outBeta.close();

    if ( verbose )
    {
        {
            cout << "Test COV detail, " << "best fold index: " << bestBetaIndex << endl;
            MatrixXd Q2Mat(fold,Y.cols());
            for ( size_t j = 0; j < fold; ++j )
            {
                cout << "fold index: " << j << endl;
                Q2Mat.row(j) << get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][j][SLModelResultTypeCOV]).transpose();
                cout << Q2Mat.row(j) << endl;
            }
            cout << "k-fold average:" << endl;
            cout << Q2Mat.colwise().mean() << endl;
            cout << endl;
        }
        
        {
            cout << "Test RSS detail, " << "best fold index: " << bestBetaIndex << endl;
            MatrixXd RSSMat(fold,Y.cols());
            for ( size_t j = 0; j < fold; ++j )
            {
                cout << "fold index: " << j << endl;
                RSSMat.row(j) << get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][j][SLModelResultTypeRSS]);
                cout << RSSMat.row(j) << endl;
            }
            cout << "k-fold average:" << endl;
            cout << RSSMat.colwise().mean() << endl;
            cout << endl;
        }
        
        cout << "duration: " << duration << endl;
    }

    ofstream outDFS((fileSuffix.str()+"DFS.txt").c_str(), ios::out);
    for ( size_t i = 0; i < topk*best; ++i )
        outDFS << get< vector<Rule> >(gspanResult[SLGraphMiningResultTypeRules])[i].dfs << endl;
    outDFS.close();
    
    return 0;
}