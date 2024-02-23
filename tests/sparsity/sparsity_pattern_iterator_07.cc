// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2004 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// test that SparsityPattern::iterator yields something
// sensible for empty sparsity patterns

#include <deal.II/lac/sparsity_pattern.h>

#include "../tests.h"


void
test()
{
  // use a non-square sparsity pattern to make sure the object does
  // not automatically store the diagonals
  SparsityPattern sp(5, 6, 3);
  // put nothing into the sparsity pattern
  sp.compress();

  SparsityPattern::const_iterator i = sp.begin();
  for (; i != sp.end(); ++i)
    deallog << i->row() << ' ' << i->column() << std::endl;

  deallog << "OK" << std::endl;
}



int
main()
{
  initlog();

  test();
}
