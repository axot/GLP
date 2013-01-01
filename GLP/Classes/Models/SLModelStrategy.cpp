//
//  SLModelStrategy.cpp
//  GLP
//
//  Created by Zheng Shao on 1/2/13.
//  Copyright (c) 2013 Saigo Laboratoire. All rights reserved.
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

#include "SLModelStrategy.h"

SLGlpCrossValidationResults SLModelStrategy::crossValidation(const MatrixXd& X, const MatrixXd& Y, SLGLPRESULTYPE type) const
{
    ASSERT(kFold <= X.rows(), "K-Fold was too big than X rows.");
    ASSERT(kFold >= 4, "K-Fold was too small, it must bigger than 4.");
    
    srand((unsigned int)time(NULL));
    
    if (randomIndexs.size() == 0)
    {
        randomIndexs.setLinSpaced(X.rows(), 0, (int)X.rows());
        random_shuffle(randomIndexs.derived().data(), randomIndexs.derived().data()+randomIndexs.derived().size());
    }
    
    MatrixXd shuffledX = X;
    MatrixXd shuffledY = Y;
    
    for (int i=0; i<X.rows(); ++i)
    {
        shuffledX.row(i) = X.row(randomIndexs[i]);
        shuffledY.row(i) = Y.row(randomIndexs[i]);
    }
    
    SLGlpCrossValidationResults result;
    for (int i=0; i < kFold; ++i)
    {
        //            MatrixXd trainX AssignCVBlocks(i, i*cvX.cols()/kFold, 2*cvX.cols()/kFold, cvX);
        //            MatrixXd trainY AssignCVBlocks(i, i*cvY.cols()/kFold, 2*cvX.cols()/kFold, cvY);
        //
        //            MatrixXd validateX AssignCVBlocks(i, i*cvX.cols()/kFold, cvX);
        //            MatrixXd validateY AssignCVBlocks(i, i*cvY.cols()/kFold, cvY);
        //
        //            MatrixXd testX AssignCVBlocks(i, i*cvX.cols()/kFold, cvX);
        //            MatrixXd testY AssignCVBlocks(i, i*cvY.cols()/kFold, cvY);
        
        //            result[SLGLPCROSSVALIDATIONRESULTYPETRAIN]    = train(trainX, trainY, type);
        //            result[SLGLPCROSSVALIDATIONRESULTYPEVALIDATE] = classify(validateX, validateY, type);
        //            result[SLGLPCROSSVALIDATIONRESULTYPETEST]     = classify(testX, testY, type);
    }
    return result;
}
