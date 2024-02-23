// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2018 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// tests matrix-free face evaluation, matrix-vector products as compared to
// the same implementation with MeshWorker. Same as matrix_vector_faces_01 but
// using FE_Q instead of FE_DGQ (i.e., we do not actually have any
// contributions from interior faces but the algorithm should have get them to
// zero)

#include <deal.II/base/function.h>

#include <deal.II/fe/fe_q.h>

#include "../tests.h"

#include "matrix_vector_faces_common.h"

template <int dim, int fe_degree>
void
test()
{
  Triangulation<dim> tria;
  GridGenerator::hyper_cube(tria);
  tria.refine_global(5 - dim);

  FE_Q<dim>       fe(fe_degree);
  DoFHandler<dim> dof(tria);
  dof.distribute_dofs(fe);
  AffineConstraints<double> constraints;
  constraints.close();

  // test with threads enabled as well
  do_test<dim, fe_degree, fe_degree + 1, double>(dof, constraints, false);
}
