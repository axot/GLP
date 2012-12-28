//
//  SLGspanSparsePlsFactory.h
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
//

#ifndef __GLP__SLGspanSparsePlsFactory__
#define __GLP__SLGspanSparsePlsFactory__

#include <iostream>

#include "SLGlpFactory.h"
#include "SLGlpCombination.h"
#include "../Models/SLSparsePls.h"
#include "../GraphMining/SLGspan.h"

class SLGspanSparsePlsFactory : public SLGlpFactory<SLSparsePls, SLGspan>
{
public:
    SLGlpCombination<SLSparsePls, SLGspan>* create();
};

#endif /* defined(__GLP__SLGspanSparsePlsFactory__) */
