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

#ifndef LATBUILDER__TASK__CBC_H
#define LATBUILDER__TASK__CBC_H

#include "latbuilder/Task/CBCBasedSearch.h"
#include "latbuilder/Task/macros.h"

#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/GenSeq/GeneratingValues.h"
#include "latbuilder/GenSeq/VectorCreator.h"

namespace LatBuilder { namespace Task {

template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct CBCTag {};


/// CBC construction.
template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE> using CBC =
   CBCBasedSearch<CBCTag<LR, PST, COMPRESS, PLO, FIGURE>>;


/// CBC construction.
template <class FIGURE, LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO>
CBC<LR, PST, COMPRESS, PLO, FIGURE> cbc(
      Storage<LR, PST, COMPRESS, PLO> storage,
      Dimension dimension,
      FIGURE figure
      )
{ return CBC<LR, PST, COMPRESS, PLO, FIGURE>(std::move(storage), dimension, std::move(figure)); }


template <LatticeType LR, PointSetType PST, Compress COMPRESS, PerLevelOrder PLO, class FIGURE>
struct CBCBasedSearchTraits<CBCTag<LR, PST, COMPRESS, PLO, FIGURE>> {
   typedef LatBuilder::Task::Search<LR, PST> Search;
   typedef LatBuilder::Storage<LR, PST, COMPRESS, PLO> Storage;
   typedef typename LatBuilder::Storage<LR, PST, COMPRESS, PLO>::SizeParam SizeParam;
   typedef typename CBCSelector<LR, PST, COMPRESS, PLO, FIGURE>::CBC CBC;
   typedef GenSeq::GeneratingValues<LR, COMPRESS> GenSeqType;

   std::vector<GenSeqType> genSeqs(const SizeParam& sizeParam, Dimension dimension) const
   {
      auto vec = GenSeq::VectorCreator<GenSeqType>::create(sizeParam, dimension);
      vec[0] = GenSeq::Creator<GenSeqType>::create(SizeParam(LatticeTraits<LR>::TrivialModulus));
      return vec;
   }

   std::string name() const
   { return FIGURE::evaluationName() + " CBC"; }

   void init(LatBuilder::Task::CBC<LR, PST, COMPRESS, PLO, FIGURE>& search) const
   { connectCBCProgress(search.cbc(), search.minObserver(), search.filters().empty()); }
};

TASK_FOR_ALL(TASK_EXTERN_TEMPLATE, CBCBasedSearch, CBC);

}}

#endif
