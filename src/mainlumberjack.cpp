#include <cryptominisat5/cryptominisat.h>
#include <cuttingplane.h>
#include <matrix.h>
#include <assert.h>
#include <cstring>
#include <fstream>
#include <vector>
using std::vector;
using namespace CMSat;


static void show_usage(std::string name){
    std::cerr << "Usage: " << name << std::endl;
    std::cerr << "\t-i/--input INPUT FILE (required)." << std::endl;
    std::cerr << "\t-o/--output OUTPUT FILE (required) where results can be stored." << std::endl;
    std::cerr << "\t-t/--threads Number of threads (Default: 1)." << std::endl;
    return;
}

int main(int argc, char** argv){
    
  // Print usage message if the proper number of arguments have not been provided
  if (argc < 2) {
    show_usage(argv[0]);
    return 1;
  }
  
  // Fix parameters that could be set by user in future
  int k = 1;
  
  // Parse input parameters
  std::string filename;
  std::string outputFolder;
  int nrThreads = 1;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
        show_usage(argv[0]);
        return 0;
    }
    else if ((arg == "-i") || (arg == "--input")) {
      // Make sure argument was supplied for this flag
      if (i + 1 < argc) {filename = argv[++i];}
        else { // Otherwise no argument was provided with this option
          std::cerr << "Check that arguments were provided for all flag options." << std::endl;
          return 1;
        }
    }
    else if ((arg == "-o") || (arg == "--output")) {
      if (i + 1 < argc) {outputFolder = argv[++i];}
      else {std::cerr << "Check that arguments were provided for all flag options." << std::endl;
        return 1;
      }
    }
    else if ((arg == "-t") || (arg == "--threads")) {
      if (i + 1 < argc) {nrThreads = std::stoul(argv[++i]);}
      else {std::cerr << "Check that arguments were provided for all flag options." << std::endl;
          return 1;
        }
    }
    else {
      std::cerr << "Flag provided is not recognized. Please type -h or --help for details on input format." << std::endl;
        return 1;
    }
  }
  
  // Read in input matrix from file
  Matrix D;
  std::ifstream inD(filename.c_str());
  if (!inD.good())
  {
    std::cerr << "Error: failed to open '" << filename << "' for reading." << std::endl;
    return 1;
  }
  inD >> D;
  inD.close();

  // Create output paths
  std::string outputFilename = outputFolder;
  
  // Construct and run solver
  CuttingPlane solver(D, k);
  solver.init();
  if (solver.solve(nrThreads))
  {
    std::ofstream outE(outputFilename.c_str());
    outE << solver.getSolA();
    outE.close();
  }
  else
  {
    return 1;
  }
  return 0;
}
