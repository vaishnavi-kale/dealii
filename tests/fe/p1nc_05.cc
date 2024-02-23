// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2015 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check the P1NC element's gradient on a square

#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_p1nc.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>

#include <string>

#include "../tests.h"



template <int dim>
void
check()
{
  Triangulation<dim> triangulation;
  GridGenerator::hyper_cube(triangulation, 0, 5);

  FE_P1NC         fe;
  DoFHandler<dim> dof_handler(triangulation);
  dof_handler.distribute_dofs(fe);

  QGauss<dim>   quadrature(3);
  FEValues<dim> fe_values(fe,
                          quadrature,
                          update_gradients | update_quadrature_points);
  fe_values.reinit(dof_handler.begin_active());

  for (unsigned int q = 0; q < quadrature.size(); ++q)
    {
      deallog << "index=" << q << " position=" << fe_values.quadrature_point(q)
              << " values=";
      for (unsigned int i = 0; i < fe.dofs_per_cell; ++i)
        deallog << '[' << fe_values.shape_grad(i, q) << "] ";
      deallog << std::endl;
    }
}

int
main()
{
  initlog();
  deallog << std::setprecision(5) << std::fixed;
  deallog.depth_console(0);

  check<2>();
}
