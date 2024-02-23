// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2007 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



char logname[] = "output";


#include <deal.II/fe/fe_q_iso_q1.h>

#include "injection_common.h"

template <int dim>
void
test()
{
  deallog << std::setprecision(6);
  for (unsigned int i = 1; i < 4; ++i)
    for (unsigned int j = i; j < 4; ++j)
      if (j % i == 0)
        do_check(FE_Q_iso_Q1<dim>(i), FE_Q_iso_Q1<dim>(j));
}
