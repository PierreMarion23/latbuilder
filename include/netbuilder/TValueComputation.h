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

#ifndef NETBUILDER__TVALUE_COMPUTATION_H
#define NETBUILDER__TVALUE_COMPUTATION_H

#include "netbuilder/GeneratingMatrix.h"

namespace NetBuilder {

    struct GaussMethod
    {
    static int computeTValue(std::vector<GeneratingMatrix> Origin_Mats, int maxSubProj, int verbose);

    };

    struct SchmidMethod
    {
        static int computeTValue(std::vector<GeneratingMatrix> matrices, unsigned int maxTValuesSubProj, int verbose);
    };

    struct ReversedSchmidMethod
    {
        static int computeTValue(std::vector<GeneratingMatrix> matrices, unsigned int maxTValuesSubProj, int verbose);   
    };
}

#endif


