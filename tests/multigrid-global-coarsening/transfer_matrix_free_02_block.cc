// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2016 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


// Check MGTransferBlockMatrixFree by comparison with MGTransferMatrixFree on a
// series of adaptive meshes for FE_Q. This is, essentially, a
// copy-paste of transfer_matrix_free_02.cc

#include <deal.II/base/logstream.h>

#include <deal.II/distributed/tria.h>

#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>

#include <deal.II/grid/grid_generator.h>

#include <deal.II/lac/la_parallel_block_vector.h>
#include <deal.II/lac/la_parallel_vector.h>

#include <deal.II/multigrid/mg_transfer.h>
#include <deal.II/multigrid/mg_transfer_global_coarsening.h>

#include "../tests.h"


template <int dim, typename Number>
void
check(const unsigned int fe_degree)
{
  FE_Q<dim> fe(fe_degree);
  deallog << "FE: " << fe.get_name() << std::endl;

  // run a few different sizes...
  unsigned int sizes[] = {1, 2, 3};
  for (unsigned int cycle = 0; cycle < sizeof(sizes) / sizeof(unsigned int);
       ++cycle)
    {
      unsigned int n_refinements = 0;
      unsigned int n_subdiv      = sizes[cycle];
      if (n_subdiv > 1)
        while (n_subdiv % 2 == 0)
          {
            n_refinements += 1;
            n_subdiv /= 2;
          }
      n_refinements += 3 - dim;
      if (fe_degree < 3)
        n_refinements += 1;

      parallel::distributed::Triangulation<dim> tr(
        MPI_COMM_WORLD,
        Triangulation<dim>::limit_level_difference_at_vertices,
        parallel::distributed::Triangulation<
          dim>::construct_multigrid_hierarchy);
      GridGenerator::subdivided_hyper_cube(tr, n_subdiv);
      tr.refine_global(n_refinements);

      // adaptive refinement into a circle
      for (typename Triangulation<dim>::active_cell_iterator cell =
             tr.begin_active();
           cell != tr.end();
           ++cell)
        if (cell->is_locally_owned() && cell->center().norm() < 0.5)
          cell->set_refine_flag();
      tr.execute_coarsening_and_refinement();
      for (typename Triangulation<dim>::active_cell_iterator cell =
             tr.begin_active();
           cell != tr.end();
           ++cell)
        if (cell->is_locally_owned() && cell->center().norm() > 0.3 &&
            cell->center().norm() < 0.4)
          cell->set_refine_flag();
      tr.execute_coarsening_and_refinement();
      for (typename Triangulation<dim>::active_cell_iterator cell =
             tr.begin_active();
           cell != tr.end();
           ++cell)
        if (cell->is_locally_owned() && cell->center().norm() > 0.33 &&
            cell->center().norm() < 0.37)
          cell->set_refine_flag();
      tr.execute_coarsening_and_refinement();

      deallog << "no. cells: " << tr.n_global_active_cells() << std::endl;

      DoFHandler<dim> mgdof(tr);
      mgdof.distribute_dofs(fe);
      mgdof.distribute_mg_dofs();

      MGConstrainedDoFs mg_constrained_dofs;
      mg_constrained_dofs.initialize(mgdof);
      mg_constrained_dofs.make_zero_boundary_constraints(mgdof, {0});

      // build reference
      MGTransferMF<dim, Number> transfer_ref(mg_constrained_dofs);
      transfer_ref.build(mgdof);

      // build matrix-free transfer
      MGTransferBlockMF<dim, Number> transfer(mg_constrained_dofs);
      transfer.build(mgdof);

      const unsigned int nb = 3;
      // check prolongation for all levels using random vector
      for (unsigned int level = 1;
           level < mgdof.get_triangulation().n_global_levels();
           ++level)
        {
          LinearAlgebra::distributed::BlockVector<Number> v1(nb), v2(nb),
            v3(nb);
          for (unsigned int b = 0; b < nb; ++b)
            {
              v1.block(b).reinit(mgdof.locally_owned_mg_dofs(level - 1),
                                 MPI_COMM_WORLD);
              v2.block(b).reinit(mgdof.locally_owned_mg_dofs(level),
                                 MPI_COMM_WORLD);
              v3.block(b).reinit(mgdof.locally_owned_mg_dofs(level),
                                 MPI_COMM_WORLD);

              for (unsigned int i = 0; i < v1.block(b).locally_owned_size();
                   ++i)
                v1.block(b).local_element(i) = random_value<double>();

              transfer_ref.prolongate(level, v2.block(b), v1.block(b));
            }

          transfer.prolongate(level, v3, v1);
          v3 -= v2;
          deallog << "Diff prolongate   l" << level << ": " << v3.l2_norm()
                  << std::endl;
        }

      // check restriction for all levels using random vector
      for (unsigned int level = 1;
           level < mgdof.get_triangulation().n_global_levels();
           ++level)
        {
          LinearAlgebra::distributed::BlockVector<Number> v1(nb), v2(nb),
            v3(nb);
          for (unsigned int b = 0; b < nb; ++b)
            {
              v1.block(b).reinit(mgdof.locally_owned_mg_dofs(level),
                                 MPI_COMM_WORLD);
              v2.block(b).reinit(mgdof.locally_owned_mg_dofs(level - 1),
                                 MPI_COMM_WORLD);
              v3.block(b).reinit(mgdof.locally_owned_mg_dofs(level - 1),
                                 MPI_COMM_WORLD);

              for (unsigned int i = 0; i < v1.block(b).locally_owned_size();
                   ++i)
                v1.block(b).local_element(i) = random_value<double>();

              transfer_ref.restrict_and_add(level, v2.block(b), v1.block(b));
            }

          transfer.restrict_and_add(level, v3, v1);
          v3 -= v2;
          deallog << "Diff restrict     l" << level << ": " << v3.l2_norm()
                  << std::endl;

          v2 = 1.;
          v3 = 1.;
          transfer.restrict_and_add(level, v2, v1);
          for (unsigned int b = 0; b < nb; ++b)
            transfer_ref.restrict_and_add(level, v3.block(b), v1.block(b));
          v3 -= v2;
          deallog << "Diff restrict add l" << level << ": " << v3.l2_norm()
                  << std::endl;
        }
      deallog << std::endl;
    }
}


int
main(int argc, char **argv)
{
  // no threading in this test...
  Utilities::MPI::MPI_InitFinalize mpi(argc, argv, 1);
  mpi_initlog();

  check<2, double>(1);
  check<2, double>(3);
  check<3, double>(1);
  check<3, double>(3);
  check<2, float>(2);
  check<3, float>(2);
}
