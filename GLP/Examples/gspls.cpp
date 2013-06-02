//
//  gspls.cpp
//  GLP
//
//  Created by Zheng Shao on 12/20/12.
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
"gspls is a part of GLP v1.0\n\n"
"Usage: gspls [-mLnkfytsbv] [gsp file]\n\n"
"Options: \n"
"           [-m min frequency of common graphs, default: 2]\n"
"           [-L max graph size for gspan mining, default: 10]\n"
"           [-n max iterator number, default: 100]\n"
"           [-k number of sub graphs abstract by gspan once time, default: 5]\n"
"           [-f folds of cross validation, default: 10]\n"
"           [-y distinct response Y matrix file]\n"
"           [-a use average residual column, defult is using max variance column]\n"
"           [-r use random residual column, defult is using max variance column]\n"
"           [-t the threshold value which used to avoid overfiting default: 3(times)]\n"
"           [-s shuffle data(preprocess)]\n"
"           [-b use memory boosting]\n"
"           [-v verbose]\n\n"
"  Author: Zheng Shao\n"
" Contact: axot@axot.org\n"
"Homepage: http://saigo-www.bio.kyutech.ac.jp/"
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
    
    if ( useAverageCol && useRandomCol )
    {
        cerr << "Error: Can not use both average and random residual column" << endl;
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
    fileSuffix << format("gspls_m%d_L%d_n%d_k%d_f%d_t%d_")%
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
    unsigned int i = 0;
    SLGraphMiningResult gspanResult;
    
    mt19937 gen( static_cast<unsigned long>(time(NULL)));
    uniform_int<> dist(0, Y.cols()-1);
    variate_generator< mt19937&, uniform_int<> > rand( gen, dist );
    
    while ( i < n )
    {
        cout << "n: " << ++i << endl;
        
        if ( useAverageCol )
        {
            VectorXd ResMean(Res.rows());
            ResMean.setZero();

            for (ssize_t i=0; i<Res.cols(); ++i)
                ResMean += Res.col(i);
            
            gspanResult = gspls.search(ResMean/Res.cols(),
                                       SLGraphMiningTasktypeTrain,
                                       SLGraphMiningResultTypeX | SLGraphMiningResultTypeDFS);
        }
        else if ( useRandomCol )
        {
            long randomColumnIndex;
            
            randomColumnIndex = rand();            
            gspanResult = gspls.search(Res.col(randomColumnIndex),
                                       SLGraphMiningTasktypeTrain,
                                       SLGraphMiningResultTypeX | SLGraphMiningResultTypeDFS);
        }
        else
        {
            long maxSquaredNormColumn;
            ColSSum(Res).maxCoeff(&maxSquaredNormColumn);
            
            gspanResult = gspls.search(Res.col(maxSquaredNormColumn),
                                       SLGraphMiningTasktypeTrain,
                                       SLGraphMiningResultTypeX | SLGraphMiningResultTypeDFS);
        }
        MatrixXd x = get<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]);
                
        SLCrossValidationResults cvResult =
            gspls.crossValidation(x, Y,
                                  SLModelResultTypeQ2   |
                                  SLModelResultTypeRSS  |
                                  SLModelResultTypeBeta |
                                  SLModelResultTypeACC  |
                                  SLModelResultTypeAUC  |
                                  SLModelResultTypeAIC  |
                                  SLModelResultTypeBIC);

        long appendedXRows = x.rows();
        long appendedXCols = x.cols();
        long oldXCols      = X.cols();
        X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
        X.rightCols(appendedXCols).setZero();
        X << X.leftCols(oldXCols), x;
                
        int bestBetaIndex;
        cvResult.eachMean(SLCrossValidationResultTypeTrain, SLModelResultTypeRSS).minCoeff(&bestBetaIndex);
        Res = Y - X*get<MatrixXd>(cvResult[SLCrossValidationResultTypeTrain][bestBetaIndex][SLModelResultTypeBeta]);
                
        cvResult.showSummary( SLModelResultTypeQ2  |
                              SLModelResultTypeRSS |
                              SLModelResultTypeACC |
                              SLModelResultTypeAUC |
                              SLModelResultTypeAIC |
                              SLModelResultTypeBIC);
        
        double RSS = cvResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeRSS);
                
        if ( RSS < minRSS )
        {
            minRSS = RSS;
            overfitCount = 0;
        }
        else
            ++overfitCount;
        
        if ( overfitCount >= cvParam.resultHistorySize ) break;
        
    }
    
    ptime time_end(microsec_clock::local_time());
    time_duration duration(time_end - time_start);
    
    if ( overfitCount < cvParam.resultHistorySize )
    {
        cerr << "Info: Can not get best result, please set a bigger value for argument n" << endl;
        return -3;
    }
    
    SLCrossValidationResults oldResult = gspls.getResultHistory().back();
    int best = i - cvParam.resultHistorySize;
    cout << "Best: n = " <<  best << endl;
    
    oldResult.showSummary( SLModelResultTypeQ2  |
                           SLModelResultTypeRSS |
                           SLModelResultTypeACC |
                           SLModelResultTypeAUC |
                           SLModelResultTypeAIC |
                           SLModelResultTypeBIC);

    ofstream outX((fileSuffix.str()+"Features.txt").c_str(), ios::out);
    outX << X.leftCols(X.cols()-(cvParam.resultHistorySize)*topk) << endl;
    outX.close();
    
    ofstream outBeta((fileSuffix.str()+"Beta.txt").c_str(), ios::out);
    outBeta.precision(12);
    outBeta.flags(ios::left);
    long bestBetaIndex;
    VectorXd cvQ2(fold);
    for ( int j = 0; j < cvQ2.size(); ++j )
        cvQ2[j] = get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][j][SLModelResultTypeQ2]).mean();
    cvQ2.maxCoeff(&bestBetaIndex);
    outBeta << get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][bestBetaIndex][SLModelResultTypeBeta]) << endl;
    outBeta.close();
    
    if ( verbose )
    {
        {
            cout << "Test Q2 detail, " << "best fold index: " << bestBetaIndex << endl;
            MatrixXd Q2Mat(fold,Y.cols());
            for ( size_t j = 0; j < fold; ++j )
            {
                cout << "fold index: " << j << endl;
                Q2Mat.row(j) << get<MatrixXd>(oldResult[SLCrossValidationResultTypeTest][j][SLModelResultTypeQ2]);
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
        outDFS << get< vector<string> >(gspanResult[SLGraphMiningResultTypeDFS])[i] << endl;
    outDFS.close();
    
    return 0;
}
