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

#include "latbuilder/Parser/PointSetType.h"
#include "latbuilder/Parser/Lattice.h"
#include "latbuilder/Parser/CommandLine.h"  
#include "latbuilder/Parser/OutputPoly.h" 
#include "latbuilder/TextStream.h"
#include "latbuilder/Types.h"

#include <fstream>
#include <chrono>

namespace LatBuilder{
using TextStream::operator<<;

static unsigned int merit_digits_displayed = 0; 

template <LatticeType LR, PointSetType PST>
void onLatticeSelected(const Task::Search<LR, PST>& s)
   {
   unsigned int old_precision = (unsigned int) std::cout.precision();
   if (merit_digits_displayed)
      std::cout.precision(merit_digits_displayed);
   std::cout << "==> " << s.bestLattice() << ": " << s.bestMeritValue() << std::endl;
   if (merit_digits_displayed)
      std::cout.precision(old_precision);
   const auto accepted = s.minObserver().acceptedCount();
   const auto rejected = s.minObserver().rejectedCount();
   const auto total = s.minObserver().totalCount();
   std::cout << "    accepted/rejected/total: "
      << accepted << "/" << rejected << "/" << total << std::endl; 
   }

boost::program_options::options_description
makeOptionsDescription()
{
   namespace po = boost::program_options;
   po::options_description desc("allowed options");

   desc.add_options ()
   ("main-construction,C", po::value<std::string>(),
    "(required) main construction type; possible values:\n"
    "  lattice\n"
    "  net\n")
   ("help,h", "produce help message")
   ("version,V", "show version")
   ("quiet,q", "show compact output (single line with number of points, generating vector and merit value)")
   ("lattice-type,l", po::value<std::string>()->default_value("ordinary"),
   "lattice; possible values:\n"
   "  ordinary (default)\n"
   "  polynomial\n")
   ("embedded-lattice,e", po::value<std::string>()->default_value("false"),
    "type of lattice; possible values:\n"
   "  false (default)\n"
   "  true\n")
   ("modulus,s", po::value<std::string>(),
    "(required) modulus of the lattice; possible values:\n"
   "  <modulus>\n"
   "  <base>^<max-power>\n"
   "  input format :\n"
   "  ordinary lattice rules: integer (decimal reprisentation)\n"
   "  polynomial lattice rules: polynomial : list of coeifficients : 1011 stands for 1 + X^2 + X^3\n")
   ("construction,c", po::value<std::string>(),
    "(required) construction type; possible values:\n"
    "  explicit:<a1>,...,<as>\n"
    "  exhaustive\n"
    "  random:<r>\n"
    "  Korobov\n"
    "  random-Korobov:<r>\n"
    "  CBC\n"
    "  random-CBC:<r>\n"
    "  fast-CBC\n"
    "  extend:<num-points>:<a1>,...,<as>\n"
    "where <r> is the number of randomizations, "
    "and <a1>,...,<as> are the components of the generating vector\n")
   ("weights,w", po::value<std::vector<std::string>>()->multitoken(),
    "(required) whitespace-separated list of weights specifications (the actual weights are the sum of these); possible values:\n"
    "  product:<default>:<coord1-weight>[,...]\n"
    "  order-dependent:<default>:<order-1-weight>,...,<order-s-weight>\n"
    "  POD:<default>:<order-1-weight>,...,<order-s-weight>:<default>:<coord1-weight>[,...]\n"
    "  projection-dependent:<proj-1>:<weight-1>[:<proj-2>:<weight-2>[:...]]\n"
    "    where <proj-n> is a comma-separated list of coordinates\n"
    "  file:\"<file>\"\n"
    "    line format in <file>:\n"
    "      <i1>,<i2>,...: <weight>\n"
    "      order <x>: <weight>\n"
    "      default: <weight>\n"
    "    if <file> is `-' data is read from standard input\n")
   ("weights-power,o", po::value<Real>(),
    "(default: same value as for the --norm-type argument) real number specifying that the weights passed as input will be assumed to be already elevated at that power (a value of `inf' is mapped to 1)\n")
   ("norm-type,p", po::value<std::string>()->default_value("2"),
    "(default: 2) norm type used to combine the value of the projection-dependent figure of merit for all projections; possible values:"
    "    <p>: a real number corresponding the l_<p> norm\n"
    "    inf: corresponding to the `max' norm\n")
   ("figure-of-merit,m", po::value<std::string>(),
    "(required) type of figure of merit; format: [CU:]<merit>\n"
    "  where the optional \"CU:\" prefix switches on the coordinate-uniform evaluation algorithm,\n"
    "  and where <merit> is one of:\n"
    "    spectral\n"
    "    P<alpha>\n"
    "    R<alpha>\n")
   ("dimension,d", po::value<std::string>(),
    "(required) lattice dimension\n")
   ("filters,f", po::value<std::vector<std::string>>()->multitoken(),
    "whitespace-separated list of filters for merit values; possible values:\n"
    "  norm:P<alpha>-{SL10|DPW08}\n"
    "  low-pass:<threshold>\n")
   ("multilevel-filters,F", po::value<std::vector<std::string>>()->multitoken(),
    "whitespace-separated list of filters for multilevel merit values; possible values:\n"
    "  norm:P<alpha>-{SL10|DPW08}[:<multilevel-weights>]\n"
    "  low-pass:<threshold>\n"
    "where <multilevel-weights> specifies the per-level weights; possible values:\n"
    "  even[:<min-level>[:<max-level>]] (default)\n")
   ("combiner,b", po::value<std::string>()->default_value("level:max"),
    "combiner for (filtered) multilevel merit values; possible values:\n"
    "  sum\n"
    "  max\n"
    "  level:{<level>|max}\n")
   ("repeat,r", po::value<unsigned int>()->default_value(1),
    "(optional) number of times the construction must be executed\n"
   "(can be useful to obtain different results from random constructions)\n")
   ("output-poly,g", po::value<std::string>()->default_value("file:$HOME/output.out"),
    "(optional) output generator matrices of the resulting polynomial lattice as a digital net, in the indicated format; possible values:\n"
   "  file:\"<file>\":format\n"
   "  available output formats\n"
   "  - ssj ")
   ("merit-digits-displayed", po::value<unsigned int>()->default_value(0),
    "(optional) number of significant figures to use when displaying merit values\n");

   return desc;
}


boost::program_options::variables_map
parse(int argc, const char* argv[])
{
   namespace po = boost::program_options;

   auto desc = makeOptionsDescription();

   po::variables_map opt;
   po::store(po::parse_command_line(argc, argv, desc), opt);
   po::notify(opt);

   if (opt.count("help")) {
      std::cout << desc << std::endl;
      std::exit (0);
   }

  //  if (opt.count("version")) {
  //     std::cout << "Lattice Builder " << LATBUILDER_VERSION << std::endl;
  //     std::exit (0);
  //  }

   if (opt.count("weights") < 1)
      throw std::runtime_error("--weights must be specified (try --help)");
   for (const auto x : {"modulus", "construction", "dimension", "figure-of-merit"}) {
      if (opt.count(x) != 1)
         throw std::runtime_error("--" + std::string(x) + " must be specified exactly once (try --help)");
   }

   return opt;
}


template <PointSetType PST>
void executeOrdinary(const Parser::CommandLine<LatticeType::ORDINARY, PST>& cmd, bool quiet, unsigned int repeat)
{
   const LatticeType LR = LatticeType::ORDINARY ;
   using namespace std::chrono;

   auto search = cmd.parse();

   const std::string separator = "\n--------------------------------------------------------------------------------\n";

   if (not quiet) {
      search->onLatticeSelected().connect(onLatticeSelected<LR,PST>);
      std::cout << *search << std::endl;
   }

   for (unsigned int i = 0; i < repeat; i++) {

      if (not quiet)
         std::cout << separator << std::endl;

      auto t0 = high_resolution_clock::now();
      search->execute();
      auto t1 = high_resolution_clock::now();

      unsigned int old_precision = (unsigned int) std::cout.precision();
      if (merit_digits_displayed)
   std::cout.precision(merit_digits_displayed);
     const auto lat = search->bestLattice();
     
      if (not quiet) {
   auto dt = duration_cast<duration<double>>(t1 - t0);
         std::cout << std::endl;
         std::cout << "BEST LATTICE: " << search->bestLattice() << ": " << search->bestMeritValue() << std::endl;
         std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl;
      }
      else {
         std::cout << lat.sizeParam().modulus();
         for (const auto& a : lat.gen())
            std::cout << "\t" << a;
         std::cout << "\t" << search->bestMeritValue() << std::endl;
      }

      
      if (merit_digits_displayed)
   std::cout.precision(old_precision);

      search->reset();
    }

   if (not quiet)
      std::cout << separator << std::endl;
}


template <PointSetType PST>
void executePolynomial(const Parser::CommandLine<LatticeType::POLYNOMIAL, PST>& cmd, bool quiet, unsigned int repeat, const Parser::OutputPolyParameters& outPoly)
{
   const LatticeType LR = LatticeType::POLYNOMIAL ;
   using namespace std::chrono;

   auto search = cmd.parse();

   const std::string separator = "\n--------------------------------------------------------------------------------\n";

   if (not quiet) {
      search->onLatticeSelected().connect(onLatticeSelected<LR,PST>);
      std::cout << *search << std::endl;
   }
   ofstream outPolyFile;
    if(outPoly.doOutput()){
        
        outPolyFile.open (outPoly.file());
     }

   for (unsigned int i = 0; i < repeat; i++) {

        if (not quiet)
           std::cout << separator << std::endl;

        auto t0 = high_resolution_clock::now();
        search->execute();
        auto t1 = high_resolution_clock::now();

        unsigned int old_precision = (unsigned int) std::cout.precision();
        if (merit_digits_displayed)
            std::cout.precision(merit_digits_displayed);
       const auto lat = search->bestLattice();
      

        if (not quiet) {
        auto dt = duration_cast<duration<double>>(t1 - t0);
           std::cout << std::endl;
           std::cout << "BEST LATTICE: " << search->bestLattice() << ": " << search->bestMeritValue() << std::endl;
           std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl;
        }
        else {
           std::cout << lat.sizeParam().modulus();
           for (const auto& a : lat.gen())
              std::cout << "\t" << a;
           std::cout << "\t" << search->bestMeritValue() << std::endl;
        }
        if(outPoly.doOutput()){
           LatBuilder::DigitalNet::PolynomialLatticeBase2 PolLat (lat.sizeParam().modulus(),lat.gen());
           PolLat.setOutputFormat(outPoly.outputFormat());
           
           outPolyFile << PolLat ;
           std::cout << std::endl;
           std::cout << "BEST MATRICES" << std::endl;
           std::cout << PolLat ;
         }
       outPolyFile.close();

        
        if (merit_digits_displayed)
            std::cout.precision(old_precision);

        search->reset();
    }
    if(outPoly.doOutput())
     outPolyFile.close();

   if (not quiet)
      std::cout << separator << std::endl;
}

   
 




int main(int argc, const char *argv[])
{

   try {
        auto opt = parse(argc, argv);

        bool quiet = opt.count("quiet");

        auto repeat = opt["repeat"].as<unsigned int>();

        // global variable
        merit_digits_displayed = opt["merit-digits-displayed"].as<unsigned int>();

       LatBuilder::LatticeType lattice = Parser::LatticeParser::parse(opt["lattice-type"].as<std::string>());

       if(lattice == LatticeType::ORDINARY){

            Parser::CommandLine<LatticeType::ORDINARY, PointSetType::MULTILEVEL> cmd;

            

            cmd.construction  = opt["construction"].as<std::string>();
            cmd.size          = opt["modulus"].as<std::string>();
            cmd.dimension     = opt["dimension"].as<std::string>();
            cmd.normType      = opt["norm-type"].as<std::string>();
            cmd.figure        = opt["figure-of-merit"].as<std::string>();
            cmd.weights       = opt["weights"].as<std::vector<std::string>>();
            cmd.combiner      = opt["combiner"].as<std::string>();

            cmd.weightsPowerScale = 1.0;
            if (opt.count("weights-power") >= 1) {
               // assume 1.0 if norm-type is `inf' or anything else
               cmd.weightsPowerScale = 1.0;
               try {
                  // start the value of norm-type as a default
                  if (cmd.normType != "inf")
                     cmd.weightsPowerScale = boost::lexical_cast<Real>(cmd.normType);
               }
               catch (boost::bad_lexical_cast&) {}
               // then scale down according to interpretation of input
               cmd.weightsPowerScale /= opt["weights-power"].as<Real>();
            }

            if (opt.count("filters") >= 1)
               cmd.filters = opt["filters"].as<std::vector<std::string>>();

            if (opt.count("multilevel-filters") >= 1)
               cmd.multilevelFilters = opt["multilevel-filters"].as<std::vector<std::string>>();

            PointSetType latType = Parser::PointSetType::parse(opt["embedded-lattice"].as<std::string>());

            if (latType == PointSetType::UNILEVEL){

               executeOrdinary<PointSetType::UNILEVEL> (cmd, quiet, repeat);
               
             }
            else{
               executeOrdinary<PointSetType::MULTILEVEL> (cmd, quiet, repeat);
               
             }
      }

      else if(lattice == LatticeType::POLYNOMIAL){

            Parser::CommandLine<LatticeType::POLYNOMIAL, PointSetType::MULTILEVEL> cmd;

            

            cmd.construction  = opt["construction"].as<std::string>();
            cmd.size          = opt["modulus"].as<std::string>();
            cmd.dimension     = opt["dimension"].as<std::string>();
            cmd.normType      = opt["norm-type"].as<std::string>();
            cmd.figure        = opt["figure-of-merit"].as<std::string>();
            cmd.weights       = opt["weights"].as<std::vector<std::string>>();
            cmd.combiner      = opt["combiner"].as<std::string>();

            cmd.weightsPowerScale = 1.0;
            if (opt.count("weights-power") >= 1) {
               // assume 1.0 if norm-type is `inf' or anything else
               cmd.weightsPowerScale = 1.0;
               try {
                  // start the value of norm-type as a default
                  if (cmd.normType != "inf")
                     cmd.weightsPowerScale = boost::lexical_cast<Real>(cmd.normType);
               }
               catch (boost::bad_lexical_cast&) {}
               // then scale down according to interpretation of input
               cmd.weightsPowerScale /= opt["weights-power"].as<Real>();
            }

            if (opt.count("filters") >= 1)
               cmd.filters = opt["filters"].as<std::vector<std::string>>();

            if (opt.count("multilevel-filters") >= 1)
               cmd.multilevelFilters = opt["multilevel-filters"].as<std::vector<std::string>>();

            PointSetType latType = Parser::PointSetType::parse(opt["embedded-lattice"].as<std::string>());

            Parser::OutputPolyParameters outPoly = Parser::OutputPoly::parse(opt["output-poly"].as<std::string>());

            if (latType == PointSetType::UNILEVEL){
               executePolynomial< PointSetType::UNILEVEL> (cmd, quiet, repeat, outPoly);
               
             }
            else{
               executePolynomial<PointSetType::MULTILEVEL> (cmd, quiet, repeat, outPoly);
               
             }
      }
   }
   catch (Parser::ParserError& e) {
      std::cerr << "COMMAND LINE ERROR: " << e.what() << std::endl;
      std::exit(1);
   }
   catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      std::exit(1);
   }

   return 0;
}

}