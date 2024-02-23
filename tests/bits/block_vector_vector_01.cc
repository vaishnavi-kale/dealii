// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2004 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check existence of
// BlockVector<double>::BlockVector(BlockVector<float>). this conversion
// constructor was disabled previously altogether because of a compiler defect
// that did not honor the 'explicit' keyword on template constructors. this is
// now autoconf'ed.

#include <deal.II/lac/block_vector.h>

#include "../tests.h"


void
test(BlockVector<double> &v)
{
  for (unsigned int i = 0; i < v.size(); ++i)
    v(i) = i + 1.;
  BlockVector<float> w(v);

  AssertThrow(w == v, ExcInternalError());

  deallog << "OK" << std::endl;
}



int
main()
{
  initlog();

  try
    {
      std::vector<types::global_dof_index> block_sizes(2, 50);
      BlockVector<double>                  v(block_sizes);
      test(v);
    }
  catch (const std::exception &exc)
    {
      deallog << std::endl
              << std::endl
              << "----------------------------------------------------"
              << std::endl;
      deallog << "Exception on processing: " << std::endl
              << exc.what() << std::endl
              << "Aborting!" << std::endl
              << "----------------------------------------------------"
              << std::endl;

      return 1;
    }
  catch (...)
    {
      deallog << std::endl
              << std::endl
              << "----------------------------------------------------"
              << std::endl;
      deallog << "Unknown exception!" << std::endl
              << "Aborting!" << std::endl
              << "----------------------------------------------------"
              << std::endl;
      return 1;
    };
}
