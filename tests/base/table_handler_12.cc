// ---------------------------------------------------------------------
//
// Copyright (C) 2010 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------


// verify that the flags we set for precision when printing stuff from
// a table do not affect the precision flags set for the stream to
// which we print


#include <deal.II/base/data_out_base.h>
#include <deal.II/base/table_handler.h>

#include <string>
#include <vector>

#include "../tests.h"


int
main()
{
  initlog();

  // set precision on the output
  // stream to 4 digits
  deallog.get_file_stream() << std::setprecision(4);

  // but then set precision on the
  // table output to 2
  TableHandler table;
  table.add_value("key", 0.123456789);
  table.set_precision("key", 2);

  // now output the table...
  table.write_text(deallog.get_file_stream());
  // ...and then output some other
  // number, hopefully with 4 digits
  // of precision
  deallog.get_file_stream() << 0.123456789 << std::endl;
}
