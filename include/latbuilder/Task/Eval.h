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

#ifndef LATBUILDER__TASK__EVAL_H
#define LATBUILDER__TASK__EVAL_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/MeritSeq/CBC.h"
#include "latbuilder/GenSeq/GeneratingValues.h"

#include <vector>

namespace LatBuilder { namespace Task {

template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct EvalTag {};


/// Explicit construction (evaluates a figure of merit for a single lattice).
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE> using Eval =
   CBCBasedSearch<EvalTag<LR, PST, COMPRESS, PLO, FIGURE>>;


/// Explicit construction (evaluates a figure of merit for a single lattice).
template <class FIGURE, LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
Eval<LR, PST, COMPRESS, PLO, FIGURE> eval(
      Storage<LR, PST, COMPRESS, PLO> storage,
      Dimension dimension,
      FIGURE figure,
      typename LatticeTraits<LR>::GeneratingVector genVec
      )
{ return Eval<LR, PST, COMPRESS, PLO, FIGURE>(std::move(storage), dimension, std::move(figure), std::move(genVec)); }


template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct CBCBasedSearchTraits<EvalTag<LR, PST, COMPRESS, PLO, FIGURE>> {
   typedef LatBuilder::Task::Search<LR, PST> Search;
   typedef LatBuilder::Storage<LR, PST, COMPRESS, PLO> Storage;
   typedef typename LatBuilder::Storage<LR, PST, COMPRESS, PLO>::SizeParam SizeParam;
   typedef typename CBCSelector<LR, PST, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef typename LatticeTraits<LR>::GeneratingVector GeneratingVector;
   typedef std::vector< typename LatticeTraits<LR>::GenValue> GenSeqType;

   CBCBasedSearchTraits(GeneratingVector genVec): genVec(std::move(genVec)) {}

   virtual ~CBCBasedSearchTraits() {}

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      if (dimension > genVec.size())
         throw std::runtime_error("dimension > generating vector size");
      std::vector<GenSeqType> out(dimension);
      for (size_t j = 0; j < dimension; j++)
         out[j] = GenSeqType{genVec[j]};
      return out;
   }

   std::string name() const
   { return FIGURE::evaluationName() + " evaluation"; }

   void init(LatBuilder::Task::Eval<LR, PST, COMPRESS, PLO, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }

   GeneratingVector genVec;
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, Eval);

}}

#endif
