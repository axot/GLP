//
//  main.cpp
//  GLP
//
//  Created by Zheng Shao on 11/23/15.
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

#include <cstdio>
#include "gspls-train.hpp"

using namespace std;

string usage()
{
    return
    "gspls is a part of GLP v1.0\n\n"
    "   Usage: gspls-train [options] [gsp file]\n\n"
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
    "          [-b] use memory boosting\n"
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
    
    size_t i = 0;
    while (i < param->n) {
        cout << "n: " << ++i << endl;
        
        // column selection
        MatrixXd selectedCol = param->colMode->getSelectedColumn(&train->getTrainResidualMat());

        // gspan
        train->gspanResult = train->gspan(selectedCol);
        
        // spls
        cout << "Train" << endl;
        MatrixXd features = get<MatrixXd>(train->gspanResult[SLGraphMiningResultTypeX]);
        train->splsResult = train->spls(features);
               
        // overfit detection
        cout << "Validation" << endl;
        vector<Rule> rules = get< vector<Rule> >(train->gspanResult[SLGraphMiningResultTypeRules]);
        
        if (train->isOverfit(rules)){
            cout << endl;
            break;
        }

        cout << endl;
    }
    
    // calc time elapsed
    train->timeEnd();
    
    // write result to file
    size_t best = i - param->resultHist.length;
    train->saveResults(best);

    return 0;
}
