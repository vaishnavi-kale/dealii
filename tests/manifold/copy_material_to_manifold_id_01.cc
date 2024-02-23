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


// Copy from boundary ids to manifold ids

#include "../tests.h"


// all include files you need here
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

template <int dim, int spacedim>
void
print_info(Triangulation<dim, spacedim> &tria)
{
  typename Triangulation<dim, spacedim>::active_cell_iterator cell;

  for (cell = tria.begin_active(); cell != tria.end(); ++cell)
    {
      deallog << "cell: " << cell << ", material_id: " << cell->material_id()
              << ", manifold_id: " << cell->manifold_id() << std::endl;

      for (const unsigned int f : GeometryInfo<dim>::face_indices())
        deallog << "face: " << cell->face(f)
                << ", boundary_id: " << cell->face(f)->boundary_id()
                << ", manifold_id: " << cell->face(f)->manifold_id()
                << std::endl;
    }
}


// Helper function
template <int dim, int spacedim>
void
test()
{
  deallog << "Testing dim=" << dim << ", spacedim=" << spacedim << std::endl;

  Triangulation<dim, spacedim> tria;
  GridGenerator::hyper_cube(tria, 0., 1.);
  tria.refine_global(1);
  tria.begin_active()->set_material_id(1);

  deallog << "Original mesh ==============================" << std::endl;
  print_info(tria);
  GridTools::copy_material_to_manifold_id(tria);
  deallog << "Copied mesh ================================" << std::endl;
  print_info(tria);
  GridTools::copy_material_to_manifold_id(tria, true);
  deallog << "Copied mesh with boundary  =================" << std::endl;
  print_info(tria);
}

int
main()
{
  initlog(true);

  test<1, 1>();
  test<1, 2>();
  test<2, 2>();
  test<2, 3>();
  test<3, 3>();

  return 0;
}
