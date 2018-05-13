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

/**
 * \file
 * Tools for streaming and poor man's factorization.
 */

#ifndef NETBUILDER__UTIL_H
#define NETBUILDER__UTIL_H

#include "netbuilder/Types.h"
#include "netbuilder/GeneratingMatrix.h"

#include <vector>
#include <boost/dynamic_bitset.hpp> 
#include <functional>
#include <limits>

//================================================================================

// TO DOCUMENT

namespace NetBuilder {

template <typename T>
T intPow(T base, unsigned long exponent)
{
   T result = (T) (1);
   while (exponent) {
      if (exponent % 2 == 1)
         result *= base;
      base *= base;
      exponent /= 2;
   }
   return result;
}

typedef boost::dynamic_bitset<> Row;

// Returns all possible sequences of positive integers d_i such that sum_{i=1}^s d_i = n
// i.e. the compositions of n in exactly s parts
// Useful for the computation of the t-value of digital nets
// Must have s >= 2, n >= s
std::vector<std::vector<int>> compositions(int n, int nb_parts);

// returns a new row with row permuted by according to the permutation defined by C
Row permutation(Row& row, std::vector<int>& C);

struct nullCombiner
{
    Real operator()(const RealVector& x){ return 0;}

    Real operator()(const std::vector<unsigned int>& x){ return 0;}
};


class Accumulator
{
    public:

        Accumulator(Real initialValue, BinOp acc):
            m_data(initialValue),
            m_op(acc)
            {}

        Accumulator(Accumulator&) = default;

        Accumulator(Accumulator&&) = default;

        void accumulate(Real weight, Real value, Real power){
            m_data = m_op(weight*std::pow(value,power), m_data);
        }

        Real tryAccumulate(Real weight, Real value, Real power) const {
            return m_op(weight*std::pow(value,power), m_data);
        }

        void set(Real value) { m_data = value; }

        Real value()
        {
            return m_data;
        }

    private:
        Real m_data;
        std::function<Real (Real, Real)> m_op;
};

BinOp realToBinOp(Real normType);


Polynomial polynomialParserHelper(const std::string& str);

}

#endif