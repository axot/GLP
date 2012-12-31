//
//  SparsePlsTest.cpp
//  GLP
//
//  Created by Zheng Shao on 12/31/12.
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

#include <Eigen/Core>
#include "../Classes/SLGlp.h"

using namespace Eigen;
using namespace std;

int main(int argc, const char *argv[])
{
    srand((unsigned int)time(NULL));

    SLSparsePls spls;
    MatrixXd X(100,3), Y(100,5);
    Y.setRandom();
    
    for (int i = 1; i <= 5; ++i)
    {
        X.setRandom();
        spls.train(X, Y);
        
        auto result = spls.getTrainResult(SLTRAINRESULTYPEQ2 | SLTRAINRESULTYPERSS);
        cout << "Loop: "    << i << endl;
        cout << "Q2:\n"     << result[SLTRAINRESULTYPEQ2]  << endl;
        cout << "\nRSS:\n"  << result[SLTRAINRESULTYPERSS] << '\n' << endl;
    }
    cout << "\nBeta:\n"  << spls.getTrainResult(SLTRAINRESULTYPEBETA)[SLTRAINRESULTYPEBETA] << '\n' << endl;
    
    MatrixXd tX(10,15), tY(10,5);
    tX.setRandom();
    tY.setRandom();
    auto result = spls.classify(tX, tY, SLTRAINRESULTYPEQ2 | SLTRAINRESULTYPERSS);
    cout << "Classify: "<< endl;
    cout << "Q2:\n"     << result[SLTRAINRESULTYPEQ2]  << endl;
    cout << "\nRSS:\n"  << result[SLTRAINRESULTYPERSS] << '\n' << endl;
    return 0;
}
