//
//  SLUtility.h
//  GLP
//
//  Created by Zheng Shao on 12/28/12.
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

#ifndef GLP_SLUtility_h
#define GLP_SLUtility_h

#define LOG(x)          cout << #x << ":\n" << x << '\n' << endl;
#define SSum(x)         (x).array().pow(2).sum()
#define Std(x)          sqrt( SSum(x.array()-x.mean()) / (x.size()-1) )
#define AutoScale(x)    ( (x).array()-(x).mean() )

#endif
