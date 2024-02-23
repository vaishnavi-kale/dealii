// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2016 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

#include <deal.II/lac/la_parallel_vector.h>
#include <deal.II/lac/la_parallel_vector.templates.h>

DEAL_II_NAMESPACE_OPEN

#include "la_parallel_vector.inst"

// do a few functions that currently don't fit the scheme because they have
// two template arguments that need to be different (the case of same
// arguments is covered by the default copy constructor and copy operator that
// is declared separately)

namespace LinearAlgebra
{
  namespace distributed
  {
#define TEMPL_COPY_CONSTRUCTOR(S1, S2)               \
  template Vector<S1, ::dealii::MemorySpace::Host> & \
  Vector<S1, ::dealii::MemorySpace::Host>::operator= \
    <S2>(const Vector<S2, ::dealii::MemorySpace::Host> &)

    TEMPL_COPY_CONSTRUCTOR(double, float);
    TEMPL_COPY_CONSTRUCTOR(float, double);
#ifdef DEAL_II_WITH_COMPLEX_VALUES
    TEMPL_COPY_CONSTRUCTOR(std::complex<double>, std::complex<float>);
    TEMPL_COPY_CONSTRUCTOR(std::complex<float>, std::complex<double>);
#endif

#undef TEMPL_COPY_CONSTRUCTOR

    template class Vector<float, ::dealii::MemorySpace::Default>;
    template class Vector<double, ::dealii::MemorySpace::Default>;

#ifndef DOXYGEN
    template void
    Vector<float, ::dealii::MemorySpace::Host>::import_elements<
      ::dealii::MemorySpace::Default>(
      const Vector<float, ::dealii::MemorySpace::Default> &,
      VectorOperation::values);
    template void
    Vector<double, ::dealii::MemorySpace::Host>::import_elements<
      ::dealii::MemorySpace::Default>(
      const Vector<double, ::dealii::MemorySpace::Default> &,
      VectorOperation::values);

    template void
    Vector<float, ::dealii::MemorySpace::Default>::import_elements<
      ::dealii::MemorySpace::Host>(
      const Vector<float, ::dealii::MemorySpace::Host> &,
      VectorOperation::values);
    template void
    Vector<double, ::dealii::MemorySpace::Default>::import_elements<
      ::dealii::MemorySpace::Host>(
      const Vector<double, ::dealii::MemorySpace::Host> &,
      VectorOperation::values);

    template void
    Vector<float, ::dealii::MemorySpace::Default>::import_elements<
      ::dealii::MemorySpace::Default>(
      const Vector<float, ::dealii::MemorySpace::Default> &,
      VectorOperation::values);
    template void
    Vector<double, ::dealii::MemorySpace::Default>::import_elements<
      ::dealii::MemorySpace::Default>(
      const Vector<double, ::dealii::MemorySpace::Default> &,
      VectorOperation::values);

    template void
    Vector<float, ::dealii::MemorySpace::Default>::reinit<float>(
      const Vector<float, ::dealii::MemorySpace::Default> &,
      const bool);
    template void
    Vector<double, ::dealii::MemorySpace::Default>::reinit<double>(
      const Vector<double, ::dealii::MemorySpace::Default> &,
      const bool);
#endif
  } // namespace distributed
} // namespace LinearAlgebra


DEAL_II_NAMESPACE_CLOSE
