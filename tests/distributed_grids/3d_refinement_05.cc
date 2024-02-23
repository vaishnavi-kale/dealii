// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2010 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// Pretty much exactly like refinement_02, except that we go to around 50,000
// cells. this is a similar case to refinement_03 (where we start with a
// coarse grid of 30,000 cells, however) and that takes a ton of time at the
// time of writing this

#include <deal.II/base/tensor.h>

#include <deal.II/distributed/tria.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/intergrid_map.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"

#include "coarse_grid_common.h"



template <int dim>
void
test(std::ostream & /*out*/)
{
  parallel::distributed::Triangulation<dim> tr(MPI_COMM_WORLD);
  Triangulation<dim>                        tr2(
    Triangulation<dim>::limit_level_difference_at_vertices);

  GridGenerator::hyper_cube(tr);
  tr.refine_global(1);

  GridGenerator::hyper_cube(tr2);
  tr2.refine_global(1);

  Assert(tr.n_active_cells() == tr2.n_active_cells(), ExcInternalError());


  while (tr.n_active_cells() < 50000)
    {
      std::vector<bool> flags(tr.n_active_cells(), false);

      // refine one fifth of all cells each
      // time (but at least one)
      for (unsigned int i = 0; i < tr.n_active_cells() / 5 + 1; ++i)
        {
          const unsigned int x = Testing::rand() % flags.size();
          // deallog << "Refining cell " << x << std::endl;
          flags[x] = true;
        }

      InterGridMap<Triangulation<dim>> intergrid_map;
      intergrid_map.make_mapping(tr, tr2);

      // refine tr and tr2
      unsigned int index = 0;
      for (typename Triangulation<dim>::active_cell_iterator cell =
             tr.begin_active();
           cell != tr.end();
           ++cell, ++index)
        if (flags[index])
          {
            cell->set_refine_flag();
            intergrid_map[cell]->set_refine_flag();
          }
      Assert(index == tr.n_active_cells(), ExcInternalError());
      tr.execute_coarsening_and_refinement();
      tr2.execute_coarsening_and_refinement();

      deallog << " Number of cells: " << tr.n_active_cells() << ' '
              << tr2.n_active_cells() << std::endl;
      deallog << "Checksum: " << tr.get_checksum() << std::endl;

      assert_tria_equal(tr, tr2);
    }
}


int
main(int argc, char *argv[])
{
  initlog();
#ifdef DEAL_II_WITH_MPI
  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
#else
  (void)argc;
  (void)argv;
#endif

  deallog.push("3d");
  test<3>(deallog.get_file_stream());
  deallog.pop();
}
