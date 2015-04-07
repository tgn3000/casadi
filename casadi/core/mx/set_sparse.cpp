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
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "set_sparse.hpp"
#include "mx_tools.hpp"
#include <vector>
#include <sstream>
#include "../std_vector_tools.hpp"

using namespace std;

namespace casadi {

  SetSparse::SetSparse(const MX& x, const Sparsity& sp) {
    setDependencies(x);
    setSparsity(Sparsity(sp));
  }

  SetSparse* SetSparse::clone() const {
    return new SetSparse(*this);
  }

  void SetSparse::printPart(std::ostream &stream, int part) const {
    if (part==0) {
      if (sparsity().isDense()) {
        stream << "dense(";
      } else {
        stream << "set_sparse(";
      }
    } else {
      stream << ")";
    }
  }

  template<typename T>
  void SetSparse::evalGen(const T* const* arg, T* const* res,
                          int* itmp, T* rtmp) {
    casadi_project(arg[0], dep().sparsity(), res[0], sparsity(), rtmp);
  }

  void SetSparse::evalD(cp_double* input, p_double* output,
                            int* itmp, double* rtmp) {
    evalGen<double>(input, output, itmp, rtmp);
  }

  void SetSparse::evalSX(cp_SXElement* input, p_SXElement* output,
                             int* itmp, SXElement* rtmp) {
    evalGen<SXElement>(input, output, itmp, rtmp);
  }

  void SetSparse::eval(const cpv_MX& input, const pv_MX& output) {
    *output[0] = input[0]->setSparse(sparsity());
  }

  void SetSparse::evalFwd(const std::vector<std::vector<MX> >& fseed,
                          std::vector<std::vector<MX> >& fsens) {
    int nfwd = fsens.size();
    for (int d=0; d<nfwd; ++d) {
      fsens[d][0] = fseed[d][0].setSparse(sparsity(), true);
    }
  }

  void SetSparse::evalAdj(const std::vector<std::vector<MX> >& aseed,
                          std::vector<std::vector<MX> >& asens) {
    int nadj = aseed.size();
    for (int d=0; d<nadj; ++d) {
      asens[d][0] += aseed[d][0].setSparse(dep().sparsity(), true);
    }
  }

  void SetSparse::spFwd(cp_bvec_t* arg,
                        p_bvec_t* res, int* itmp, bvec_t* rtmp) {
    sparsity().set(res[0], arg[0], dep().sparsity());
  }

  void SetSparse::spAdj(p_bvec_t* arg,
                        p_bvec_t* res, int* itmp, bvec_t* rtmp) {
    dep().sparsity().bor(arg[0], res[0], sparsity());
    fill(res[0], res[0]+nnz(), 0);
  }

  void SetSparse::generate(std::ostream &stream, const std::vector<int>& arg,
                                    const std::vector<int>& res, CodeGenerator& gen) const {
    // Codegen "copy sparse"
    gen.addAuxiliary(CodeGenerator::AUX_PROJECT);

    // Codegen the operation
    int sp_arg = gen.addSparsity(dep(0).sparsity());
    int sp_res = gen.addSparsity(sparsity());
    stream << "  casadi_project(" << gen.work(arg.front()) << ", s" << sp_arg << ", "
           << gen.work(res.front()) << ", s" << sp_res << ", w);" << std::endl;
  }


} // namespace casadi

