// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2007 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check FullMatrix::Tmmult


#include "../tests.h"

#include "full_matrix_common.h"



template <typename number>
void
check()
{
  FullMatrix<number> m, n, o;
  make_square_matrix(m);
  make_square_matrix(n);
  make_square_matrix(o);

  m.Tmmult(n, o, true);
  print_matrix(m);
  print_matrix(n);
  print_matrix(o);

  m.Tmmult(n, o, false);
  print_matrix(m);
  print_matrix(n);
  print_matrix(o);
}
