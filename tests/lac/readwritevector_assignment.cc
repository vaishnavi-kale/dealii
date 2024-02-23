// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2015 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

// Check constructor and operator=

#include <deal.II/base/index_set.h>

#include <deal.II/lac/read_write_vector.h>

#include "../tests.h"



void
test()
{
  unsigned int double_size = 2;
  unsigned int float_size  = 10;
  IndexSet     is(50);
  is.add_range(0, 2);
  is.add_index(46);
  is.add_range(10, 15);
  LinearAlgebra::ReadWriteVector<double> double_vector(is);
  LinearAlgebra::ReadWriteVector<float>  float_vector(float_size);
  deallog << "double_size " << double_vector.locally_owned_size() << std::endl;
  deallog << "float_size " << float_vector.locally_owned_size() << std::endl;

  double_vector = 0.;
  for (unsigned int i = 0; i < double_vector.locally_owned_size(); ++i)
    double_vector.local_element(i) += i;
  for (unsigned int i = 0; i < float_vector.locally_owned_size(); ++i)
    float_vector[i] = i;

  double_vector.print(deallog.get_file_stream());
  float_vector.print(deallog.get_file_stream());

  float_vector = double_vector;
  float_vector.print(deallog.get_file_stream());

  LinearAlgebra::ReadWriteVector<double> double_vector2(double_vector);
  double_vector2.print(deallog.get_file_stream());

  for (unsigned int i = 0; i < double_vector.locally_owned_size(); ++i)
    double_vector2.local_element(i) += i;
  double_vector = double_vector2;
  double_vector.print(deallog.get_file_stream());
}

int
main()
{
  initlog();
  test();

  return 0;
}
