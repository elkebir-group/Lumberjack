/*
 * cuttingplane.cpp
 *
 */

#include <cuttingplane.h>
using namespace CMSat;

CuttingPlane::CuttingPlane(const Matrix& B, int k)
  : _B(B)
  , _m(B.getNrClones())
  , _n(B.getNrMutations())
  , _k(k)
  , _B2Var()
  , _var2B()
  , _activeEntries()
  , _nrActiveVariables(0)
  , _nrConstraints(0)
  , _solver()
  , _solA(_B.getNrClones(), _B.getNrMutations())
{
}

void CuttingPlane::init(){
  
  /// Resize containers for each clone
  _activeEntries.resize(_m);
  _B2Var.resize(_m);
  for (int p = 0; p < _m; p++)
  {
    /// Resize container for each mutation
    _activeEntries[p].resize(_n);
    _B2Var[p].resize(_n);
    for (int c = 0; c < _n; ++c)
    {
      _activeEntries[p][c] = false;
      
      /// Check if this entry is elligible for being active
      if (_B.getEntry(p, c)==0){
        
        /// Note that is it active and generate variables
        _activeEntries[p][c] = true;
        _B2Var[p][c] = _nrActiveVariables;
        _var2B.push_back(Triple(p, c, 2));
        _nrActiveVariables = _nrActiveVariables+1;
        assert(_var2B.size() == _nrActiveVariables);
      }
    }
  }
}

bool CuttingPlane::solve(int nrThreads){
  
  /// Set number of threads
  _solver.set_num_threads(nrThreads);
  
  /// Create variables
  _solver.new_vars(_nrActiveVariables);
  
  /// Loop over SAT solver
  /// After each iteration, add voilated constraints with cutting planes
  int iteration = 1;
  bool res = false;
  while (true)
  {
    std::cerr << "Step " << iteration << " -- number of constraints: " << _nrConstraints << std::endl;
    
    lbool ret = _solver.solve();
    
    /// No solution found for SAT
    if (ret != l_True) {
      assert(ret == l_False);
      std::cerr << "No solution found." << std::endl;
      break;
    }
        
    /// See if current solution violated any contraints
    int separatedConstraints = separate();
    _nrConstraints += separatedConstraints;
    std::cerr << "Step " << iteration << " -- introduced " << separatedConstraints << " constraints" << std::endl;
    
    /// If assignment did not violate constraints, then we stop
    if (separatedConstraints == 0){
      res = true;
      break;
    }

    ++iteration;
  }
  
  /// If a satisfying assignment is found, process it
  if (res){
    processSolution();
  }
  
  return res;
}


int CuttingPlane::getAssignment(int p, int c){
  
  if (!_activeEntries[p][c]){
    return _B.getEntry(p, c);
  }
  
  int var = _B2Var[p][c];
  if (_solver.get_model()[var] == l_True){
    return 2;
  }
  
  if (_solver.get_model()[var] == l_False){
    return 0;
  }
  
  throw std::runtime_error("Error: Solver did not assign truth value to variable.");
}

int CuttingPlane::separate()
{

  int nrNewClauses = 0;
  std::vector<Lit> clause;
  
  for (int c = 0; c < _n; ++c)
  {
    for (int d = c + 1; d < _n; ++d)
    {
      // condition 1
      for (int i = 1; i <= _k + 1; ++i)
      {
        for (int i_prime = 1; i_prime <= _k + 1; ++i_prime)
        {
          for (int j = 1; j <= _k + 1; ++j)
          {
            for (int j_prime = 1; j_prime <= _k + 1; ++j_prime)
            {
              StlIntSet pSet;
              for (int p = 0; p < _m; p++)
              {
                if ((getAssignment(p,c) == i) && (getAssignment(p,d) == 0))
                {
                  pSet.insert(p);
                }
              }
              
              StlIntSet qSet;
              for (int q = 0; q < _m; q++)
              {
                if ((getAssignment(q,c) == 0) && (getAssignment(q,d) == j))
                {
                  qSet.insert(q);
                }
              }
              
              StlIntSet rSet;
              for (int r = 0; r < _m; r++)
              {
                if ((getAssignment(r,c) == i_prime) && (getAssignment(r,d) == j_prime))
                {
                  rSet.insert(r);
                }
              }
              
              for (int p : pSet)
              {
                for (int q : qSet)
                {
                  for (int r : rSet)
                  {
                    clause.clear();
                                        
                    if (_activeEntries[p][c]){
                      /// constraint[0] = Triple(p, c, i);
                      /// This entry is currently set to TRUE
                      /// This constraint will not be violated if it is FALSE next time
                      clause.push_back(Lit(_B2Var[p][c], true));
                    }
                    
                    if (_activeEntries[q][c]){
                      /// constraint[1] = Triple(p, d, 0);
                      /// This entry is currently set to FALSE
                      /// This constraint will not be violated if it is TRUE next time
                      clause.push_back(Lit(_B2Var[p][d], false));
                    }
                    
                    if (_activeEntries[p][c]){
                      /// constraint[2] = Triple(q, c, 0);
                      /// This entry is currently set to FALSE
                      /// This constraint will not be violated if it is TRUE next time
                      clause.push_back(Lit(_B2Var[q][c], false));
                    }
                    
                    if (_activeEntries[p][c]){
                      /// constraint[3] = Triple(q, d, j);
                      /// This entry is currently set to TRUE
                      /// This constraint will not be violated if it is FALSE next time
                      clause.push_back(Lit(_B2Var[q][d], true));
                    }
                    
                    if (_activeEntries[p][c]){
                      /// constraint[4] = Triple(r, c, i_prime);
                      /// This entry is currently set to TRUE
                      /// This constraint will not be violated if it is FALSE next time
                      clause.push_back(Lit(_B2Var[r][c], true));
                    }
                    
                    if (_activeEntries[p][c]){
                      /// constraint[5] = Triple(r, d, j_prime);
                      /// This entry is currently set to TRUE
                      /// This constraint will not be violated if it is FALSE next time
                      clause.push_back(Lit(_B2Var[r][d], true));
                    }
                    _solver.add_clause(clause);
                    nrNewClauses = nrNewClauses+1;
                  }
                }
              }
            }
          }
        }
      }
      
      // condition 2
      for (int i = 1; i <= _k + 1; ++i)
      {
        for (int i_prime = 1; i_prime <= _k + 1; ++i_prime)
        {
          for (int j = 2; j <= _k + 1; ++j)
          {
            for (int j_prime = 1; j_prime <= _k + 1; ++j_prime)
            {
              if (j_prime == j) continue;
          
              StlIntSet pSet;
              for (int p = 0; p < _m; p++)
              {
                if ((getAssignment(p,c) == i) && (getAssignment(p,d) == j_prime))
                {
                  pSet.insert(p);
                }
              }
              
              StlIntSet qSet;
              for (int q = 0; q < _m; q++)
              {
                if ((getAssignment(q,c) == 0) && (getAssignment(q,d) == j))
                {
                  qSet.insert(q);
                }
              }
              
              StlIntSet rSet;
              for (int r = 0; r < _m; r++)
              {
                if ((getAssignment(r,c) == i_prime) && (getAssignment(r,d) == j))
                {
                  rSet.insert(r);
                }
              }
              
              for (int p : pSet)
              {
                for (int q : qSet)
                {
                  for (int r : rSet)
                  {
                    clause.clear();
                    
                    if (_activeEntries[p][c]){
                      /// constraint[0] = Triple(p, c, i);
                      clause.push_back(Lit(_B2Var[p][c], true));
                    }
                    
                    if (_activeEntries[p][d]){
                      /// constraint[1] = Triple(p, d, j_prime);
                      clause.push_back(Lit(_B2Var[p][d], true));
                    }
                    
                    if (_activeEntries[q][c]){
                      /// constraint[2] = Triple(q, c, 0);
                      clause.push_back(Lit(_B2Var[q][c], false));
                    }
                    
                    if (_activeEntries[q][d]){
                      /// constraint[3] = Triple(q, d, j);
                      clause.push_back(Lit(_B2Var[q][d], true));
                    }
                    
                    if (_activeEntries[r][c]){
                      /// constraint[4] = Triple(r, c, i_prime);
                      clause.push_back(Lit(_B2Var[r][c], true));
                    }
                    
                    if (_activeEntries[r][d]){
                      /// constraint[5] = Triple(r, d, j);
                      clause.push_back(Lit(_B2Var[r][d], true));
                    }
                    _solver.add_clause(clause);
                    nrNewClauses = nrNewClauses+1;
                  }
                }
              }
            }
          }
        }
      }
      
      // condition 3
      for (int i = 2; i <= _k + 1; ++i)
      {
        for (int i_prime = 1; i_prime <= _k + 1; ++i_prime)
        {
          if (i_prime == i) continue;
          for (int j = 1; j <= _k + 1; ++j)
          {
            for (int j_prime = 1; j_prime <= _k + 1; ++j_prime)
            {
              StlIntSet pSet;
              for (int p = 0; p < _m; p++)
              {
                if ((getAssignment(p,c) == i) && (getAssignment(p,d) == 0))
                {
                  pSet.insert(p);
                }
              }
              
              StlIntSet qSet;
              for (int q = 0; q < _m; q++)
              {
                if ((getAssignment(q,c) == i_prime) && (getAssignment(q,d) == j))
                {
                  qSet.insert(q);
                }
              }
              
              StlIntSet rSet;
              for (int r = 0; r < _m; r++)
              {
                if ((getAssignment(r,c) == i) && (getAssignment(r,d) == j_prime))
                {
                  rSet.insert(r);
                }
              }
              
              for (int p : pSet)
              {
                for (int q : qSet)
                {
                  for (int r : rSet)
                  {
                    clause.clear();
                    
                    if (_activeEntries[p][c]){
                      /// constraint[0] = Triple(p, c, i);
                      clause.push_back(Lit(_B2Var[p][c], true));
                    }
                    
                    if (_activeEntries[p][d]){
                      /// constraint[1] = Triple(p, d, 0);
                      clause.push_back(Lit(_B2Var[p][d], false));
                    }
                    
                    if (_activeEntries[q][c]){
                      /// constraint[2] = Triple(q, c, i_prime);
                      clause.push_back(Lit(_B2Var[q][c], true));
                    }
                    
                    if (_activeEntries[q][d]){
                      /// constraint[3] = Triple(q, d, j);
                      clause.push_back(Lit(_B2Var[q][d], true));
                    }
                    
                    if (_activeEntries[r][c]){
                      /// constraint[4] = Triple(r, c, i);
                      clause.push_back(Lit(_B2Var[r][c], true));
                    }
                    
                    if (_activeEntries[r][d]){
                      /// constraint[5] = Triple(r, d, j_prime);
                      clause.push_back(Lit(_B2Var[r][d], true));
                    }
                    _solver.add_clause(clause);
                    nrNewClauses = nrNewClauses+1;
                  }
                }
              }
            }
          }
        }
      }
      
      // condition 4
      for (int i = 2; i <= _k + 1; ++i)
      {
        for (int i_prime = 1; i_prime <= _k + 1; ++i_prime)
        {
          if (i_prime == i) continue;
          for (int j = 2; j <= _k + 1; ++j)
          {
            for (int j_prime = 1; j_prime <= _k + 1; ++j_prime)
            {
              if (j_prime == j) continue;
              
              StlIntSet pSet;
              for (int p = 0; p < _m; p++)
              {
                if ((getAssignment(p,c) == i) && (getAssignment(p,d) == j_prime))
                {
                  pSet.insert(p);
                }
              }
              
              StlIntSet qSet;
              for (int q = 0; q < _m; q++)
              {
                if ((getAssignment(q,c) == i_prime) && (getAssignment(q,d) == j))
                {
                  qSet.insert(q);
                }
              }
              
              StlIntSet rSet;
              for (int r = 0; r < _m; r++)
              {
                if ((getAssignment(r,c) == i) && (getAssignment(r,d) == j))
                {
                  rSet.insert(r);
                }
              }
              
              for (int p : pSet)
              {
                for (int q : qSet)
                {
                  for (int r : rSet)
                  {
                    clause.clear();
                    
                    if (_activeEntries[p][c]){
                      /// constraint[0] = Triple(p, c, i);
                      clause.push_back(Lit(_B2Var[p][c], true));
                    }
                    
                    if (_activeEntries[p][d]){
                      /// constraint[1] = Triple(p, d, j_prime);
                      clause.push_back(Lit(_B2Var[p][d], true));
                    }
                    
                    if (_activeEntries[q][c]){
                      /// constraint[2] = Triple(q, c, i_prime);
                      clause.push_back(Lit(_B2Var[q][c], true));
                    }
                    
                    if (_activeEntries[q][d]){
                      /// constraint[3] = Triple(q, d, j);
                      clause.push_back(Lit(_B2Var[q][d], true));
                    }
                    
                    if (_activeEntries[r][c]){
                      /// constraint[4] = Triple(r, c, i);
                      clause.push_back(Lit(_B2Var[r][c], true));
                    }
                    
                    if (_activeEntries[r][d]){
                      /// constraint[5] = Triple(r, d, j);
                      clause.push_back(Lit(_B2Var[r][d], true));
                    }
                    _solver.add_clause(clause);
                    nrNewClauses = nrNewClauses+1;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  
  return nrNewClauses;
}

void CuttingPlane::processSolution(){
  for (int p = 0; p < _m; p++){
    for (int c = 0; c < _n; ++c){
      _solA.setEntry(p, c, getAssignment(p, c));
    }
  }
}
