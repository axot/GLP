//
//  SLGspanSparsePlsFactory.cpp
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
//

#include "SLGspanSparsePlsFactory.h"
#include "SLGlpCombination.h"

SLGlpCombination<SLSparsePls, SLGspan>* SLGspanSparsePlsFactory::create()
{
    return new SLGlpCombination<SLSparsePls, SLGspan>;
}
