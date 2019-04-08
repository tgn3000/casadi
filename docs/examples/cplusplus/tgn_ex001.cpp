/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 */

#include <casadi/casadi.hpp>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace casadi;
using namespace std;
/**
 *  Example program demonstrating parametric NLPs in CasADi
 *  Note that there is currently no support for parametric sensitivities via
 * this feature (although it would make a lot of sense).
 *  For parametric sensitivities, see the parametric_sensitivities.cpp example
 * which calculates sensitivitied via the sIPOPT extension
 *  to IPOPT.
 *
 *  Joel Andersson, K.U. Leuven 2012
 */

int main() {
  /** Test problem (Ganesh & Biegler, A reduced Hessian strategy for sensitivity
   * analysis of optimal flowsheets, AIChE 33, 1987, pp. 282-296)
   *
   *    min     -x0-x1 ( max x0+x1 )
   *    s.t.    x0^2+x1^2 <= 1
   *
   */

  // Optimization variables
  SX x = SX::sym("x", 2);

  // Objective
  SX f = -x(0) - x(1);

  // Constraints
  SX g = x(0) * x(0) + x(1) * x(1);

  // Initial guess and bounds for the optimization variables
  vector<double> x0  = {0.0, 0.0};
  vector<double> lbx = {-inf, -inf};
  vector<double> ubx = {inf, inf};

  // Nonlinear bounds
  vector<double> lbg = {-inf};
  vector<double> ubg = {1.0};

  // NLP
  SXDict nlp = {{"x", x}, {"f", f}, {"g", g}};

  // Create NLP solver and buffers
  Function solver = nlpsol("solver", "ipopt", nlp);
  std::map<std::string, DM> arg, res;

  // Solve the NLP
  arg["lbx"] = lbx;
  arg["ubx"] = ubx;
  arg["lbg"] = lbg;
  arg["ubg"] = ubg;
  arg["x0"]  = x0;
  res        = solver(arg);

  // Print the solution
  cout << "-----" << endl;
  cout << "Optimal solution: " << endl;
  cout << setw(30) << "Objective: " << res.at("f") << endl;
  cout << setw(30) << "Primal solution: " << res.at("x") << endl;
  cout << setw(30) << "Dual solution (x): " << res.at("lam_x") << endl;
  cout << setw(30) << "Dual solution (g): " << res.at("lam_g") << endl;

  return 0;
}
