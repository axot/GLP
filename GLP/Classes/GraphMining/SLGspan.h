//
//  SLGspan.h
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
//

#ifndef __GLP__SLGspan__
#define __GLP__SLGspan__

#include <iostream>

#include "SLGraphMiningStrategy.h"

class SLGspan : public SLGraphMiningStrategy
{
public:    
    MatrixXd& search();
};

#endif /* defined(__GLP__SLGspan__) */
