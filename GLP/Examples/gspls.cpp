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
#include <Eigen/Core>
#include <cfloat>
#include <GLP/SLGlp.h>

using namespace Eigen;
using namespace std;
using namespace boost;

void usage()
{
    cerr <<
"Usage: gspls [-mLnkyv] [-t train data] [-o output model]\n\n"
"Options: \n"
"           [-m minsup, default:1]\n"
"           [-L maxpat, default:10]\n"
"           [-n components, default:10]\n"
"           [-k topk, default:10]\n"
"           [-f folds of cross validation, default:4]\n"
"           [-y Y matrix file, if not use Y values which included in gsp file]\n"
"           [-v verbose: default:0 range:0-1]\n"
"           [-t train data]\n"
"           [-o model output]\n\n"
"GLP v2.0 2012.12.20\n"
"  Author: Zheng Shao\n"
" Contact: axot@axot.org\n"
"Homepage: http://saigo-www.bio.kyutech.ac.jp/\n"
    << endl;
}

int main(int argc, const char *argv[])
{
    size_t maxpat = 10;
    size_t minsup = 2;
    size_t n = 10;
    size_t topk = 10;
    size_t fold = 4;
    char *yfile = NULL;
    char *gspfile = NULL;
    int verbose = 0;

    if (argc < 2) {
        usage();
        return -1;
    }
    
    int opt;
    while ((opt = getopt(argc, (char **)argv, "L:m:n:k:f:y:v:")) != -1)
    {
        switch(opt)
        {
            case 'L':
                maxpat = atoi (optarg);
                break;
            case 'm':
                minsup = atoi (optarg);
                break;
            case 'n':
                n = atoi (optarg);
                break;
            case 'k':
                topk = atoi (optarg);
                break;
            case 'f':
                fold = atoi (optarg);
                break;
            case 'y':
                yfile = strdup(optarg);
                break;
            case 'v':
                verbose = atoi (optarg);
                break;
            default:
                usage();
                return -2;
        }
    }
    
    gspfile = strdup(argv[argc-1]);
    
    ++optind;
    for (int i = optind; i < argc; i++)
    {
        printf ("Unknown argument: %s\n", argv[i]);
        usage();
        return -2;
    }
    
    SLSparsePls::SLSparsePlsParameters splsParam;
    
    SLGspan::SLGspanParameters gspanParam;
    gspanParam.minsup = minsup;
    gspanParam.maxpat = maxpat;
    gspanParam.topk   = topk;
    gspanParam.doesUseMemoryBoost = true;
    gspanParam.gspFilename = string(gspfile);
    
    BOOST_AUTO(gspls, (*SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam)));
        
    SLCrossValidation<SLSparsePls>::SLCrossValidationParameters cvParam;
    cvParam.doesUseShuffleData = false;
    cvParam.kFold = fold;
    cvParam.resultHistorySize = 4;
    
    gspls.setCrossValidationParameters(cvParam);
    
    MatrixXd X, Y, Res;
    
    if ( yfile != NULL )
    {
        EigenExt::loadMatrixFromFile(Y, yfile);
    }
    else
    {
        Y = any_cast<MatrixXd>(gspls.getInnerValues(SLGraphMiningInnerValueY)[SLGraphMiningInnerValueY]);
    }
    
    Y = Y.array() - Y.mean();
    Res = Y;
    
    double lastRSS = -DBL_MAX;
    size_t overfitCount = 0;
    
    unsigned int i = 0;
    
    while ( i < n )
    {
        cout << "n: " << ++i << endl;
        
        long maxSquaredNormColumn;
        SSum(Res).maxCoeff(&maxSquaredNormColumn);
        VectorXd largestResCol = Res.col(maxSquaredNormColumn);
        
        SLGraphMiningResult gspanResult = gspls.search(largestResCol, SLGraphMiningTasktypeTrain, SLGraphMiningResultTypeX);
        
        MatrixXd x = any_cast<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]);
                
        SLCrossValidationResults cvResult =
            gspls.crossValidation(x, Y, SLModelResultTypeQ2 | SLModelResultTypeRSS | SLModelResultTypeBeta | SLModelResultTypeACC);

        long appendedXRows = x.rows();
        long appendedXCols = x.cols();
        long oldXCols      = X.cols();
        X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
        X.rightCols(appendedXCols).setZero();
        X << X.leftCols(oldXCols), x;
                
        int bestBetaIndex;
        cvResult.eachMean(SLCrossValidationResultTypeTrain, SLModelResultTypeRSS).minCoeff(&bestBetaIndex);
        Res = Y - X*any_cast<MatrixXd>(cvResult[SLCrossValidationResultTypeTrain][bestBetaIndex][SLModelResultTypeBeta]);
        
        double RSS = cvResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeRSS);
        
        cout << "Train:"    << endl;
        cout << "Q2:\t"     << cvResult.mean(SLCrossValidationResultTypeTrain, SLModelResultTypeQ2);
        cout << "\tRSS:\t"    << cvResult.mean(SLCrossValidationResultTypeTrain, SLModelResultTypeRSS);
        cout << "\tACC:\t"    << cvResult.mean(SLCrossValidationResultTypeTrain, SLModelResultTypeACC) << endl;
        
        cout << "Validation:" << endl;
        cout << "Q2:\t"     << cvResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeQ2);
        cout << "\tRSS:\t"    << cvResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeRSS);
        cout << "\tACC:\t"    << cvResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeACC) << endl;

        cout << "Test:"   << endl;
        cout << "Q2:\t"     << cvResult.mean(SLCrossValidationResultTypeTest, SLModelResultTypeQ2);
        cout << "\tRSS:\t"    << cvResult.mean(SLCrossValidationResultTypeTest, SLModelResultTypeRSS);
        cout << "\tACC:\t"    << cvResult.mean(SLCrossValidationResultTypeTest, SLModelResultTypeACC) << endl;

        cout << endl << endl;
        
        if ( RSS > lastRSS )
        {
            ++overfitCount;
        }
        else
        {
            overfitCount = 0;
        }
        if ( overfitCount >= cvParam.resultHistorySize-1 )
        {
            break;
        }
        
        lastRSS = RSS;
    }
    
    if ( overfitCount < cvParam.resultHistorySize-1 )
    {
        cerr << "can not get best result, please set a bigger value for argument n" << endl;
        return -1;
    }
    
    SLCrossValidationResults oldResult = gspls.getResultHistory().back();
    cout << "\nBest: n = "       <<  i - cvParam.resultHistorySize + 1 << endl;
    cout << "Train:"            << endl;
    cout << "Q2:\t"             << oldResult.mean(SLCrossValidationResultTypeTrain, SLModelResultTypeQ2);
    cout << "\tRSS:\t"            << oldResult.mean(SLCrossValidationResultTypeTrain, SLModelResultTypeRSS);
    cout << "\tACC:\t"            << oldResult.mean(SLCrossValidationResultTypeTrain, SLModelResultTypeACC) << endl;
    
    cout << "Validation:"     << endl;
    cout << "Q2:\t"             << oldResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeQ2);
    cout << "\tRSS:\n"            << oldResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeRSS);
    cout << "\tACC:\t"            << oldResult.mean(SLCrossValidationResultTypeValidation, SLModelResultTypeACC) << endl;
    
    cout << "Test:"           << endl;
    cout << "Q2:\t"             << oldResult.mean(SLCrossValidationResultTypeTest, SLModelResultTypeQ2);
    cout << "\tRSS:\t"            << oldResult.mean(SLCrossValidationResultTypeTest, SLModelResultTypeRSS);
    cout << "\tACC:\t"            << oldResult.mean(SLCrossValidationResultTypeTest, SLModelResultTypeACC) << endl;
    
    ofstream outX("X.txt", ios::out);
    outX << X.leftCols(X.cols()-(cvParam.resultHistorySize-1)*topk) << endl;
    outX.close();
    
    ofstream outBeta("Beta.txt", ios::out);
    outBeta << oldResult.print(SLCrossValidationResultTypeValidation, SLModelResultTypeBeta) << endl;
//    cout << "\nBeta:"         << oldResult.print(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEBETA) << endl;
    outBeta.close();
    return 0;
}
