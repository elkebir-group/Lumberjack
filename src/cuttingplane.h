/*
 * cuttingplane.h
 *
 */

#ifndef CUTTINGPLANE_H
#define CUTTINGPLANE_H

#include <cryptominisat5/cryptominisat.h>
#include "matrix.h"
#include "utils.h"

/// This class provides a cutting plane wrapper for CryptoMiniSAT
/// This can be used to solve the the k-DP problem .
class CuttingPlane
{
public:
    
  /// Constructor
  ///
  /// @param B Input matrix
  /// @param k Maximum number of losses per character
  CuttingPlane(const Matrix& B, int k);
  
  /// Initialize solver
  virtual void init();
  
  /// Return solution matrix
  const Matrix& getSolA() const
  {
    return _solA;
  }
  
  /// Solve
  bool solve(int nrThreads);
  
protected:
  
  /// Get current assignment from solver and input
  int getAssignment(int p, int c);
  
  /// Identify violated constraints
  int separate();
  
  /// Extract solution from SAT solver
  void processSolution();
  
  /// Triple (p,c,i)
  struct Triple
  {
  public:
    Triple(int p, int c, int i)
      : _p(p)
      , _c(c)
      , _i(i)
    {
    }
    
    Triple()
      : _p(-1)
      , _c(-1)
      , _i(-1)
    {
    }
    
    int _p;
    int _c;
    int _i;
  };
      
  /// Forbidden submatrix
  typedef std::array<Triple, 6> ViolatedConstraint;
  
  /// List of forbidden submatrices
  typedef std::list<ViolatedConstraint> ViolatedConstraintList;
  
protected:
  /// Input matrix
  const Matrix& _B;
  /// Number of taxa
  const int _m;
  /// Number of characters
  const int _n;
  /// Maximum number of losses
  const int _k;
  /// _B2Var[p][c] maps matrix entries to active variable index
  StlIntMatrix _B2Var;
  /// _var2B maps active variable index to triple indexing marix B entry
  std::vector<Triple> _var2B;
  /// Indicates which matrix entries are active
  StlBoolMatrix _activeEntries;
  /// Number of active variables
  int _nrActiveVariables;
  /// Number of constraints
  int _nrConstraints;
  /// Solver
  CMSat::SATSolver _solver;
  /// Solution matrix
  Matrix _solA;
};

#endif // COLUMNGEN_H
