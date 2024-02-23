// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2001 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_c1.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>

#include <algorithm>

#include "../tests.h"

#define PRECISION 2

int
main()
{
  initlog();
  deallog << std::fixed;
  deallog << std::setprecision(PRECISION) << std::fixed;

  // create grid of circle, somehow
  // arbitrarily from only one cell,
  // but since we are not interested
  // in the quality of the mesh, this
  // is ok
  SphericalManifold<2> circle;
  Triangulation<2>     tria;
  GridGenerator::hyper_cube(tria, -1, 1);
  tria.set_all_manifold_ids_on_boundary(0);
  tria.set_manifold(0, circle);


  // refine it more or less
  // arbitrarily
  tria.refine_global(1);
  if (true)
    {
      Triangulation<2>::active_cell_iterator cell = tria.begin_active();
      ++cell;
      cell->set_refine_flag();
      tria.execute_coarsening_and_refinement();
    };

  // attach a dof handler to it
  const FE_Q<2> fe(2);
  DoFHandler<2> dof_handler(tria);
  dof_handler.distribute_dofs(fe);

  // and loop over all exterior faces
  // to see whether the normal
  // vectors are indeed continuous
  // and pointing radially outward at
  // the vertices
  const QTrapezoid<1> quadrature;
  const MappingC1<2>  c1_mapping;
  FEFaceValues<2>     c1_values(c1_mapping,
                            fe,
                            quadrature,
                            update_quadrature_points | update_normal_vectors);

  // to compare with, also print the
  // normal vectors as generated by a
  // cubic mapping
  const MappingQ<2> q3_mapping(3);
  FEFaceValues<2>   q3_values(q3_mapping,
                            fe,
                            quadrature,
                            update_quadrature_points | update_normal_vectors);

  for (DoFHandler<2>::active_cell_iterator cell = dof_handler.begin_active();
       cell != dof_handler.end();
       ++cell)
    for (const unsigned int f : GeometryInfo<2>::face_indices())
      if (cell->face(f)->at_boundary())
        {
          c1_values.reinit(cell, f);
          q3_values.reinit(cell, f);

          // there should now be two
          // normal vectors, one for
          // each vertex of the face
          Assert(c1_values.get_normal_vectors().size() == 2,
                 ExcInternalError());

          // check that these two
          // normal vectors have
          // length approximately 1
          // and point radially
          // outward
          for (unsigned int i = 0; i < 2; ++i)
            {
              Point<2> radius = c1_values.quadrature_point(i);
              radius /= std::sqrt(radius.square());
              deallog << "Normalized radius=" << radius << std::endl;

              deallog << "C1 normal vector " << i << ": "
                      << c1_values.normal_vector(i) << std::endl;
              deallog << "Q3 normal vector " << i << ": "
                      << q3_values.normal_vector(i) << std::endl;
            };


          // some numerical checks for correctness
          for (unsigned int i = 0; i < 2; ++i)
            {
              AssertThrow(std::fabs(c1_values.normal_vector(i) *
                                      c1_values.normal_vector(i) -
                                    1) < 1e-14,
                          ExcInternalError());
              Point<2> radius = c1_values.quadrature_point(i);
              radius /= std::sqrt(radius.square());

              AssertThrow((radius - c1_values.normal_vector(i)).norm_square() <
                            1e-14,
                          ExcInternalError());
            };
        };
}
