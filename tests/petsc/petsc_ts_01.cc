// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

#include <deal.II/lac/exceptions.h>
#include <deal.II/lac/petsc_ts.templates.h>

#include "../tests.h"

/**
 * Tests user defined Vector and Matrix types
 * and exceptions handling for PETSCWrappers::TimeStepper.
 */

class VectorType : public Subscriptor
{
public:
  explicit VectorType(Vec v)
    : v(v)
  {}

  Vec &
  petsc_vector()
  {
    return v;
  }

private:
  Vec v;
};

class MatrixType : public Subscriptor
{
public:
  explicit MatrixType(Mat A)
    : A(A)
  {}

  Mat &
  petsc_matrix()
  {
    return A;
  }

private:
  Mat A;
};

using TimeStepper = PETScWrappers::TimeStepper<VectorType, MatrixType>;

int
main(int argc, char **argv)
{
  initlog();

  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);

  VectorType v(nullptr);
  MatrixType A(nullptr);

  TimeStepper myode;

  try
    {
      auto ts = myode.petsc_ts();
      auto t0 = myode.get_time();
      auto dt = myode.get_time_step();
      AssertThrow(ts == static_cast<TS>(myode), ExcInternalError());
      myode.solve(v, A);
    }
  catch (const StandardExceptions::ExcFunctionNotProvided &)
    {
      deallog << "catching expected exception" << std::endl;
    }
  catch (const std::exception &exc)
    {
      deallog << exc.what() << std::endl;
    }
}
