//
//  gspls-classify.cpp
//  GLP
//
//  Created by Zheng Shao on 11/29/15.
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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include "gspls-classify.hpp"

SLGsplsClassify::ClassifyParameters* SLGsplsClassify::ClassifyParameters::initWithArgs(int argc, char* argv[])
{
    SLGsplsClassify::ClassifyParameters* param = new SLGsplsClassify::ClassifyParameters();
    
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
                              "d:b:y:",
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
            case 'd':
                param->dfsFile = strdup(optarg);
                break;
            case 'b':
                param->betaFile = strdup(optarg);
                break;
            case 'y':
                param->respFile = strdup(optarg);
                break;
            default:
                return NULL;
        }
    }

    param->gspFile = strdup(argv[argc-1]);

    if (param->mode == NULL)
        param->mode = (SLPlsModeRegression*) new SLPlsModeRegression();
    
    return param;
}

SLGsplsClassify* SLGsplsClassify::initWithParam(ClassifyParameters& param)
{
    SLGsplsClassify* classify = new SLGsplsClassify();
    classify->_param = param;
    
    SLGspan::SLGspanParameters gspanParam;
    gspanParam.doesUseMemoryBoost = true;
    gspanParam.gspFilename = param.gspFile;
    
    SLGspan* gspan = new SLGspan();
    gspan->setParameters(gspanParam);
    classify->setGspan(gspan);
    
    EigenExt::loadMatrixFromFile(classify->_respMat, param.respFile);
    
    return classify;
}

void SLGsplsClassify::setGspan(SLGspan* gspan)
{
    _gspan = gspan;
}

void SLGsplsClassify::classify()
{
    string dfsLine;
    vector<std::string> dfsLines;
    ifstream dfsFile(_param.dfsFile);
    
    while (getline(dfsFile, dfsLine)) dfsLines.push_back(dfsLine);
    
    _gspan->buildDarts(dfsLines);
    
    vector<Graph> trans = _gspan->getTransaction();
    MatrixXd result(trans.size(), dfsLines.size());
    for (size_t i = 0; i < trans.size(); ++i) {
        result.row(i) = _gspan->classify(trans[i]);
    }
    
    MatrixXd beta;
    EigenExt::loadMatrixFromFile(beta, _param.betaFile);

    _spls = new SLSparsePls();
    SLMODELRESULTYPE types = _param.mode->getResultType();
    SLModelResult scores = _spls->classify(beta, result, _respMat, types);
    
    SLModelUtility::printResult(types, scores);
}