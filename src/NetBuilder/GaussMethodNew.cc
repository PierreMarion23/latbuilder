// // This file is part of Lattice Builder.
// //
// // Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

// #include <map>
// #include <fstream>
// #include "netbuilder/TValueComputation.h"
// #include "netbuilder/Util.h"
// #include "netbuilder/ProgressiveRowReducer.h"

// namespace NetBuilder {

// int iteration_on_k(std::vector<GeneratingMatrix>& Origin_Mats, int k, ProgressiveRowReducer rowReducer, bool verbose=false){
//     int s = (int) Origin_Mats.size();
//     int m = (int) Origin_Mats[0].nCols();

//     std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rows_order = compositionsChanges(k, s);
    
//     // Initialization of row map from original matrices to computation matrix
//     std::map<std::pair<int, int>, int> Origin_to_M;

//     for (int i=0; i<k-s+1; i++){
//         Origin_to_M[{1, i+1}] = i;
//         rowReducer.addRow(Origin_Mats[0].subMatrix(i, 1, m));
//     }
//     for (int i=1; i<s; i++){
//         Origin_to_M[{i+1, 1}] = k-s+i;
//         rowReducer.addRow(Origin_Mats[i].subMatrix(0, 1, m));
//     }

//     if (rowReducer.computeRank() != m){
//         return 1;
//     }

//     int step = 0;
//     for (auto it = begin (rows_order); it != end (rows_order); ++it) {
//         if (verbose){
//             std::cout << "STEP " << step << std::endl;
//         }
//         step++;
//         std::pair<std::pair<int, int>, std::pair<int, int>> rowChange = *it;

//         int ind_exchange = Origin_to_M[rowChange.first];
//         Origin_to_M[rowChange.second] = ind_exchange;
//         Origin_to_M.erase(rowChange.first);
        
//         GeneratingMatrix newRow = Origin_Mats[rowChange.second.first-1].subMatrix(rowChange.second.second-1, 1, m);

//         if (verbose){
//             std::cout << "new row " << newRow << std::endl;
//         }

//         rowReducer.exchangeRow(ind_exchange, newRow);
//         if (rowReducer.computeRank() != m){
//             return 1;
//         }
//     }
//     return 0;
// }

// int GaussMethod::computeTValue(std::vector<GeneratingMatrix> Origin_Mats, int maxSubProj, int verbose=0)
// {
//     int m = (int) Origin_Mats[0].nRows();
//     int s = (int) Origin_Mats.size();
//     ProgressiveRowReducer rowReducer = ProgressiveRowReducer();
    
//     if (s == 1){    // to be modified!
//         return 0;
//     }
//     for (int k=m-maxSubProj; k >= s; k--){
//         rowReducer.reset(m);
//         int status = iteration_on_k(Origin_Mats, k, rowReducer, verbose);
//         if (verbose){
//             std::cout << "after iteration " << k << ", status : " << status << std::endl;
//         }     
//         if (status == 0){
//             return std::max(m-k, maxSubProj);
//         }
//     }
//     return std::max(m-s+1, maxSubProj);
// }

// }

