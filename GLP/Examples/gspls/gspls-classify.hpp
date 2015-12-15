//
//  gspls-classify.hpp
//  GLP
//
//  Created by Zheng Shao on 11/17/15.
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

#ifndef gspls_classify_hpp
#define gspls_classify_hpp

#include <stdio.h>
#include <GLP/SLGlp.h>

class SLGsplsClassify
{
public:
    class ClassifyParameters
    {
    public:
        ClassifyParameters() :
        mode(NULL),
        dfsFile(NULL),
        betaFile(NULL),
        gspFile(NULL),
        respFile(NULL)
        {}
        
        static ClassifyParameters* initWithArgs(int argc, char* argv[]);
        
    public:
        IMode<SLSparsePls>* mode;
        char* dfsFile;
        char* betaFile;
        char* gspFile;
        char* respFile;
    };
    
private:
    ClassifyParameters _param;
    SLGspan* _gspan;
    SLSparsePls* _spls;
    MatrixXd _respMat;
    
public:
    static SLGsplsClassify* initWithParam(ClassifyParameters&);
    void setGspan(SLGspan*);
    void classify();
};

#endif /* gspls_classify_hpp */
