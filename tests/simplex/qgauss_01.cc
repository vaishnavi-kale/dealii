// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2021 - 2022 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

#include <deal.II/base/function_lib.h>
#include <deal.II/base/quadrature_lib.h>

#include "../tests.h"

template <int dim>
void
print(const unsigned int n_points_1D)
{
  deallog << "n_points_1D = " << n_points_1D << std::endl;
  const QGaussSimplex<dim> quad(n_points_1D);

  deallog << "quad size = " << quad.size() << std::endl;
  for (unsigned int q = 0; q < quad.size(); ++q)
    {
      deallog << quad.point(q) << ' ';
      deallog << quad.weight(q) << ' ';
      deallog << std::endl;
    }
}

template <int dim>
void
check_accuracy_1D(const unsigned int n_points_1D)
{
  const unsigned int accuracy = 2 * n_points_1D - 1;

  Tensor<1, dim> monomial_powers;
  unsigned int   sum = 0;
  for (unsigned int d = 0; d < dim; ++d)
    {
      monomial_powers[d] += accuracy / dim;
      sum += accuracy / dim;
    }

  // if we aren't at the correct degree then add the rest to the final
  // component
  monomial_powers[dim - 1] += accuracy - sum;

  const Functions::Monomial<dim> func(monomial_powers);
  const QGaussSimplex<dim>       quad(n_points_1D);

  deallog << "Monomial powers = " << monomial_powers << std::endl;
  double integrand = 0.0;
  for (unsigned int q = 0; q < quad.size(); ++q)
    integrand += quad.weight(q) * func.value(quad.point(q));
  auto old_precision = deallog.precision(16);
  deallog << "Integrand = " << integrand << std::endl;
  deallog.precision(old_precision);
}

int
main()
{
  initlog();

  deallog << std::endl << std::endl;
  check_accuracy_1D<2>(1);
  check_accuracy_1D<2>(2);
  check_accuracy_1D<2>(3);
  check_accuracy_1D<2>(4);

  check_accuracy_1D<3>(1);
  check_accuracy_1D<3>(2);
  check_accuracy_1D<3>(3);
  check_accuracy_1D<3>(4);
}
