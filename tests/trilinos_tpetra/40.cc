// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2004 - 2024 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check LinearAlgebra::TpetraWrappers::Vector<double>::add(s,V,s,V)

#include <deal.II/base/utilities.h>

#include <deal.II/lac/trilinos_tpetra_vector.h>

#include <iostream>
#include <vector>

#include "../tests.h"


void
test(LinearAlgebra::TpetraWrappers::Vector<double> &v,
     LinearAlgebra::TpetraWrappers::Vector<double> &w,
     LinearAlgebra::TpetraWrappers::Vector<double> &x)
{
  for (unsigned int i = 0; i < v.size(); ++i)
    {
      v(i) = i;
      w(i) = i + 1.;
      x(i) = i + 2.;
    }

  v.compress(VectorOperation::insert);
  w.compress(VectorOperation::insert);
  x.compress(VectorOperation::insert);

  v.add(2, w, 3, x);

  // make sure we get the expected result
  for (unsigned int i = 0; i < v.size(); ++i)
    {
      AssertThrow(w(i) == i + 1., ExcInternalError());
      AssertThrow(x(i) == i + 2., ExcInternalError());
      AssertThrow(v(i) == i + 2 * (i + 1.) + 3 * (i + 2.), ExcInternalError());
    }

  deallog << "OK" << std::endl;
}



int
main(int argc, char **argv)
{
  initlog();

  Utilities::MPI::MPI_InitFinalize mpi_initialization(
    argc, argv, testing_max_num_threads());


  try
    {
      {
        LinearAlgebra::TpetraWrappers::Vector<double> v;
        v.reinit(complete_index_set(100), MPI_COMM_WORLD);
        LinearAlgebra::TpetraWrappers::Vector<double> w;
        w.reinit(complete_index_set(100), MPI_COMM_WORLD);
        LinearAlgebra::TpetraWrappers::Vector<double> x;
        x.reinit(complete_index_set(100), MPI_COMM_WORLD);
        test(v, w, x);
      }
    }
  catch (const std::exception &exc)
    {
      std::cerr << std::endl
                << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Exception on processing: " << std::endl
                << exc.what() << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;

      return 1;
    }
  catch (...)
    {
      std::cerr << std::endl
                << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Unknown exception!" << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;
      return 1;
    };
}
