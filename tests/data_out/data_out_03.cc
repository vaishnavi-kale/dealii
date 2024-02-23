// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2005 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// write the data in deal.II intermediate form, read it back in, and
// make sure that the result is the same

#include <deal.II/lac/sparsity_pattern.h>

#include <deal.II/numerics/data_out.h>

#include "../tests.h"

#include "data_out_common.h"



// have a class that makes sure we can get at the patches and data set
// names that the base class generates
template <int dim>
class XDataOut : public DataOut<dim>
{
public:
  const std::vector<typename ::DataOutBase::Patch<dim, dim>> &
  get_patches() const
  {
    return DataOut<dim>::get_patches();
  }

  std::vector<std::string>
  get_dataset_names() const
  {
    return DataOut<dim>::get_dataset_names();
  }
};

// have a class that makes sure we can get at the patches and data set
// names that the base class generates
template <int dim>
class XDataOutReader : public DataOutReader<dim>
{
public:
  const std::vector<typename ::DataOutBase::Patch<dim, dim>> &
  get_patches() const
  {
    return DataOutReader<dim>::get_patches();
  }

  std::vector<std::string>
  get_dataset_names() const
  {
    return DataOutReader<dim>::get_dataset_names();
  }
};


template <int dim>
void
check_this(const DoFHandler<dim> &dof_handler,
           const Vector<double>  &v_node,
           const Vector<double>  &v_cell)
{
  XDataOut<dim> data_out;
  data_out.attach_dof_handler(dof_handler);
  data_out.add_data_vector(v_node, "node_data", XDataOut<dim>::type_dof_data);
  data_out.add_data_vector(v_cell, "cell_data", XDataOut<dim>::type_cell_data);
  data_out.build_patches();

  {
    std::ofstream tmp("data_out_03.tmp");
    data_out.write_deal_II_intermediate(tmp);
  }

  XDataOutReader<dim> reader;
  {
    std::ifstream tmp("data_out_03.tmp");
    reader.read(tmp);
  }

  // finally make sure that we have
  // read everything back in
  // correctly
  AssertThrow(data_out.get_dataset_names() == reader.get_dataset_names(),
              ExcInternalError());

  AssertThrow(data_out.get_patches().size() == reader.get_patches().size(),
              ExcInternalError());

  for (unsigned int i = 0; i < reader.get_patches().size(); ++i)
    AssertThrow(data_out.get_patches()[i] == reader.get_patches()[i],
                ExcInternalError());

  // for good measure, delete tmp file
  remove("data_out_03.tmp");

  deallog << "OK" << std::endl;
}
