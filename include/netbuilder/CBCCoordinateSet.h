// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NETBUILDER__CBC_COORDINATE_SET_H
#define NETBUILDER__CBC_COORDINATE_SET_H

#include "latcommon/CoordinateSets.h"

#include <iostream>
#include <algorithm>

namespace NetBuilder { 

class CBCCoordinateSet
{
    public:

        CBCCoordinateSet(int highestDimension, int maxOrder):
            m_dimension(highestDimension),
            m_maxOrder(maxOrder),
            m_base(0, std::min(maxOrder-1,highestDimension-1), 0, std::max(highestDimension-2,0)),
            m_addCoordinate(m_base, highestDimension-1)
        {};

        LatCommon::CoordinateSets::AddCoordinate<LatCommon::CoordinateSets::FromRanges>::const_iterator begin() const 
        {
            return m_addCoordinate.begin();
        }

        LatCommon::CoordinateSets::AddCoordinate<LatCommon::CoordinateSets::FromRanges>::const_iterator end() const 
        {
            return m_addCoordinate.end();
        }

    private:
        unsigned int m_dimension;
        unsigned int m_maxOrder;
        LatCommon::CoordinateSets::FromRanges m_base;
        LatCommon::CoordinateSets::AddCoordinate<LatCommon::CoordinateSets::FromRanges> m_addCoordinate;
};

}

#endif