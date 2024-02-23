// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 1998 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

#ifndef dealii_matrix_creator_h
#define dealii_matrix_creator_h


#include <deal.II/base/config.h>

#include <deal.II/base/exceptions.h>
#include <deal.II/base/function.h>

#include <deal.II/lac/affine_constraints.h>

#include <map>

#ifdef DEAL_II_WITH_PETSC
#  include <petscsys.h>
#endif

DEAL_II_NAMESPACE_OPEN


// forward declarations
#ifndef DOXYGEN
template <int dim>
class Quadrature;


template <typename number>
class Vector;
template <typename number>
class SparseMatrix;

template <int dim, int spacedim>
class Mapping;
template <int dim, int spacedim>
DEAL_II_CXX20_REQUIRES((concepts::is_valid_dim_spacedim<dim, spacedim>))
class DoFHandler;

namespace hp
{
  template <int>
  class QCollection;
  template <int, int>
  class MappingCollection;
} // namespace hp

#endif


/**
 * This namespace provides functions that assemble certain standard matrices
 * for a given triangulation, using a given finite element, a given mapping
 * and a quadrature formula.
 *
 *
 * <h3>Conventions for all functions</h3>
 *
 * There exist two versions of almost all functions, one that takes an
 * explicit Mapping argument and one that does not. The second one generally
 * calls the first with an implicit $Q_1$ argument (i.e., with an argument of
 * kind MappingQ(1)). If your intend your code to use a different
 * mapping than a (bi-/tri-)linear one, then you need to call the functions
 * <b>with</b> mapping argument should be used.
 *
 * All functions take a sparse matrix object to hold the matrix to be created.
 * The functions assume that the matrix is initialized with a sparsity pattern
 * (SparsityPattern) corresponding to the given degree of freedom handler,
 * i.e. the sparsity structure is already as needed. You can do this by
 * calling the DoFTools::make_sparsity_pattern() function.
 *
 * Furthermore it is assumed that no relevant data is in the matrix. Some
 * entries will be overwritten and some others will contain invalid data if
 * the matrix wasn't empty before. Therefore you may want to clear the matrix
 * before assemblage.
 *
 * By default, all created matrices are `raw': they are not condensed, i.e.
 * hanging nodes are not eliminated. The reason is that you may want to add
 * several matrices and could then condense afterwards only once, instead of
 * for every matrix. To actually do computations with these matrices, you have
 * to condense the matrix using the AffineConstraints::condense function; you
 * also have to condense the right hand side accordingly and distribute the
 * solution afterwards. Alternatively, you can give an optional argument
 * AffineConstraints that writes cell matrix (and vector) entries with
 * distribute_local_to_global into the global matrix and vector. This way,
 * adding several matrices from different sources is more complicated and
 * you should make sure that you do not mix different ways of applying
 * constraints. Particular caution is necessary when the given
 * AffineConstraints object contains inhomogeneous constraints: In that case,
 * the matrix assembled this way must be the only matrix (or you need to
 * assemble the <b>same</b> right hand side for <b>every</b> matrix you
 * generate and add together).
 *
 * If you want to use boundary conditions with the matrices generated by the
 * functions of this namespace in addition to the ones in a possible
 * AffineConstraints object, you have to use a function like
 * <tt>apply_boundary_values</tt> with the matrix, solution, and right hand
 * side.
 *
 *
 * <h3>Supported matrices</h3>
 *
 * At present there are functions to create the following matrices:
 * <ul>
 * <li> @p create_mass_matrix: create the matrix with entries $m_{ij} =
 * \int_\Omega \phi_i(x) \phi_j(x) dx$ by numerical quadrature. Here, the
 * $\phi_i$ are the basis functions of the finite element space given.
 *
 * A coefficient may be given to evaluate $m_{ij} = \int_\Omega a(x) \phi_i(x)
 * \phi_j(x) dx$ instead.
 *
 * <li> @p create_laplace_matrix: create the matrix with entries $a_{ij} =
 * \int_\Omega \nabla\phi_i(x) \nabla\phi_j(x) dx$ by numerical quadrature.
 *
 * Again, a coefficient may be given to evaluate $a_{ij} = \int_\Omega a(x)
 * \nabla\phi_i(x) \nabla\phi_j(x) dx$ instead.
 * </ul>
 *
 * Make sure that the order of the Quadrature formula given to these functions
 * is sufficiently high to compute the matrices with the required accuracy.
 * For the choice of this quadrature rule you need to take into account the
 * polynomial degree of the FiniteElement basis functions, the roughness of
 * the coefficient @p a, as well as the degree of the given @p Mapping (if
 * any).
 *
 * Note, that for vector-valued elements the @ref GlossMassMatrix "mass matrix" and the laplace
 * matrix is implemented in such a way that each component couples only with
 * itself, i.e. there is no coupling of shape functions belonging to different
 * components. If the degrees of freedom have been sorted according to their
 * vector component (e.g., using DoFRenumbering::component_wise()), then the
 * resulting matrices will be block diagonal.
 *
 * If the finite element for which the mass matrix or the Laplace matrix is to
 * be built has more than one component, the functions accept a single
 * coefficient as well as a vector valued coefficient function. For the latter
 * case, the number of components must coincide with the number of components
 * of the system finite element.
 *
 *
 * <h3>Matrices on the boundary</h3>
 *
 * The create_boundary_mass_matrix() creates the matrix with entries $m_{ij} =
 * \int_{\Gamma} \phi_i \phi_j dx$, where $\Gamma$ is the union of boundary
 * parts with indicators contained in a std::map<types::boundary_id, const
 * Function<spacedim,number>*> passed to the function (i.e. if you want to set
 * up the mass matrix for the parts of the boundary with indicators zero and 2,
 * you pass the function a map with key type types::boundary_id
 * as the parameter @p boundary_functions containing the keys zero and
 * 2). The size of the matrix is equal to the number of degrees of freedom
 * that have support on the boundary, i.e. it is <em>not</em> a matrix on all
 * degrees of freedom, but only a subset. (The $\phi_i$ in the formula are the
 * subset of basis functions which have at least part of their support on
 * $\Gamma$.) In order to determine which shape functions are to be
 * considered, and in order to determine in which order, the function takes a
 * @p dof_to_boundary_mapping; this object maps global DoF numbers to a
 * numbering of the degrees of freedom located on the boundary, and can be
 * obtained using the function DoFTools::map_dof_to_boundary_indices().
 *
 * In order to work, the function needs a matrix of the correct size, built on
 * top of a corresponding sparsity pattern. Since we only work on a subset of
 * the degrees of freedom, we can't use the matrices and sparsity patterns
 * that are created for the entire set of degrees of freedom. Rather, you
 * should use the DoFHandler::make_boundary_sparsity_pattern() function to
 * create the correct sparsity pattern, and build a matrix on top of it.
 *
 * Note that at present there is no function that computes the mass matrix for
 * <em>all</em> shape functions, though such a function would be trivial to
 * implement.
 *
 *
 * <h3>Right hand sides</h3>
 *
 * In many cases, you will not only want to build the matrix, but also a right
 * hand side, which will give a vector with $f_i = \int_\Omega f(x) \phi_i(x)
 * dx$. For this purpose, each function exists in two versions, one only
 * building the matrix and one also building the right hand side vector. If
 * you want to create a right hand side vector without creating a matrix, you
 * can use the VectorTools::create_right_hand_side() function. The use of the
 * latter may be useful if you want to create many right hand side vectors.
 *
 * @ingroup numerics
 */
namespace MatrixCreator
{
  /**
   * Assemble the @ref GlossMassMatrix "mass matrix". If no coefficient is given (i.e., if the
   * pointer to a function object is zero as it is by default), the
   * coefficient is taken as being constant and equal to one.
   * In case you want to specify @p constraints and use the default argument
   * for the coefficient you have to specify the (unused) coefficient argument
   * as <code>(const Function<spacedim,number> *const)nullptr</code>.
   *
   * If the library is configured to use multithreading, this function works
   * in parallel.
   *
   * The optional argument @p constraints allows to apply constraints on the
   * resulting matrix directly. Note, however, that this becomes difficult
   * when you have inhomogeneous constraints and later want to add several
   * such matrices, for example in time dependent settings such as the main
   * loop of step-26.
   *
   * See the general documentation of this namespace for more information.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const Mapping<dim, spacedim>    &mapping,
    const DoFHandler<dim, spacedim> &dof,
    const Quadrature<dim>           &q,
    SparseMatrixType                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Call the create_mass_matrix() function, see above, with
   * <tt>mapping=MappingQ@<dim@>(1)</tt>.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const DoFHandler<dim, spacedim> &dof,
    const Quadrature<dim>           &q,
    SparseMatrixType                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Assemble the @ref GlossMassMatrix "mass matrix" and a right hand side vector. If no coefficient
   * is given (i.e., if the pointer to a function object is zero as it is by
   * default), the coefficient is taken as being constant and equal to one.
   * In case you want to specify @p constraints and use the default argument
   * for the coefficient you have to specify the (unused) coefficient argument
   * as <code>(const Function <spacedim,number> *const)nullptr</code>.
   *
   * If the library is configured to use multithreading, this function works
   * in parallel.
   *
   * The optional argument @p constraints allows to apply constraints on the
   * resulting matrix directly. Note, however, that this becomes difficult
   * when you have inhomogeneous constraints and later want to add several
   * such matrices, for example in time dependent settings such as the main
   * loop of step-26.
   *
   * See the general documentation of this namespace for more information.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const Mapping<dim, spacedim>                                    &mapping,
    const DoFHandler<dim, spacedim>                                 &dof,
    const Quadrature<dim>                                           &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Call the create_mass_matrix() function, see above, with
   * <tt>mapping=MappingQ@<dim@>(1)</tt>.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const DoFHandler<dim, spacedim>                                 &dof,
    const Quadrature<dim>                                           &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Same function as above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const hp::MappingCollection<dim, spacedim> &mapping,
    const DoFHandler<dim, spacedim>            &dof,
    const hp::QCollection<dim>                 &q,
    SparseMatrixType                           &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Same function as above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const DoFHandler<dim, spacedim> &dof,
    const hp::QCollection<dim>      &q,
    SparseMatrixType                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Same function as above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const hp::MappingCollection<dim, spacedim>                      &mapping,
    const DoFHandler<dim, spacedim>                                 &dof,
    const hp::QCollection<dim>                                      &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Same function as above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_mass_matrix(
    const DoFHandler<dim, spacedim>                                 &dof,
    const hp::QCollection<dim>                                      &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());


  /**
   * Assemble the @ref GlossMassMatrix "mass matrix" and a right hand side vector along the boundary.
   *
   * The matrix is assumed to already be initialized with a suiting sparsity
   * pattern (the DoFHandler provides an appropriate function).
   *
   * If the library is configured to use multithreading, this function works
   * in parallel.
   *
   * @arg @p weight: an optional weight for the computation of the mass
   * matrix. If no weight is given, it is set to one.
   * In case you want to specify @p component_mapping and use the default argument
   * for the coefficient you have to specify the (unused) coefficient argument
   * as <code>(const Function <spacedim,number> *const)nullptr</code>.
   *
   * @arg @p component_mapping: if the components in @p boundary_functions and
   * @p dof do not coincide, this vector allows them to be remapped. If the
   * vector is not empty, it has to have one entry for each component in @p
   * dof. This entry is the component number in @p boundary_functions that
   * should be used for this component in @p dof. By default, no remapping is
   * applied.
   *
   * @todo This function does not work for finite elements with cell-dependent
   * shape functions.
   */
  template <int dim, int spacedim, typename number>
  void
  create_boundary_mass_matrix(
    const Mapping<dim, spacedim>    &mapping,
    const DoFHandler<dim, spacedim> &dof,
    const Quadrature<dim - 1>       &q,
    SparseMatrix<number>            &matrix,
    const std::map<types::boundary_id, const Function<spacedim, number> *>
                                           &boundary_functions,
    Vector<number>                         &rhs_vector,
    std::vector<types::global_dof_index>   &dof_to_boundary_mapping,
    const Function<spacedim, number> *const weight            = 0,
    std::vector<unsigned int>               component_mapping = {});


  /**
   * Call the create_boundary_mass_matrix() function, see above, with
   * <tt>mapping=MappingQ@<dim@>(1)</tt>.
   */
  template <int dim, int spacedim, typename number>
  void
  create_boundary_mass_matrix(
    const DoFHandler<dim, spacedim> &dof,
    const Quadrature<dim - 1>       &q,
    SparseMatrix<number>            &matrix,
    const std::map<types::boundary_id, const Function<spacedim, number> *>
                                           &boundary_functions,
    Vector<number>                         &rhs_vector,
    std::vector<types::global_dof_index>   &dof_to_boundary_mapping,
    const Function<spacedim, number> *const a                 = nullptr,
    std::vector<unsigned int>               component_mapping = {});

  /**
   * Same function as above, but for hp-objects.
   */
  template <int dim, int spacedim, typename number>
  void
  create_boundary_mass_matrix(
    const hp::MappingCollection<dim, spacedim> &mapping,
    const DoFHandler<dim, spacedim>            &dof,
    const hp::QCollection<dim - 1>             &q,
    SparseMatrix<number>                       &matrix,
    const std::map<types::boundary_id, const Function<spacedim, number> *>
                                           &boundary_functions,
    Vector<number>                         &rhs_vector,
    std::vector<types::global_dof_index>   &dof_to_boundary_mapping,
    const Function<spacedim, number> *const a                 = nullptr,
    std::vector<unsigned int>               component_mapping = {});

  /**
   * Same function as above, but for hp-objects.
   */
  template <int dim, int spacedim, typename number>
  void
  create_boundary_mass_matrix(
    const DoFHandler<dim, spacedim> &dof,
    const hp::QCollection<dim - 1>  &q,
    SparseMatrix<number>            &matrix,
    const std::map<types::boundary_id, const Function<spacedim, number> *>
                                           &boundary_functions,
    Vector<number>                         &rhs_vector,
    std::vector<types::global_dof_index>   &dof_to_boundary_mapping,
    const Function<spacedim, number> *const a                 = nullptr,
    std::vector<unsigned int>               component_mapping = {});

  /**
   * Assemble the Laplace matrix. If no coefficient is given (i.e., if the
   * pointer to a function object is zero as it is by default), the
   * coefficient is taken as being constant and equal to one.
   * In case you want to specify @p constraints and use the default argument
   * for the coefficient you have to specify the (unused) coefficient argument
   * as <code>(const Function<spacedim> *const)nullptr</code>.
   *
   * If the library is configured to use multithreading, this function works
   * in parallel.
   *
   * The optional argument @p constraints allows to apply constraints on the
   * resulting matrix directly. Note, however, that this becomes difficult
   * when you have inhomogeneous constraints and later want to add several
   * such matrices, for example in time dependent settings such as the main
   * loop of step-26.
   *
   * See the general documentation of this namespace for more information.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const Mapping<dim, spacedim>    &mapping,
    const DoFHandler<dim, spacedim> &dof,
    const Quadrature<dim>           &q,
    SparseMatrixType                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Call the create_laplace_matrix() function, see above, with
   * <tt>mapping=MappingQ@<dim@>(1)</tt>.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const DoFHandler<dim, spacedim> &dof,
    const Quadrature<dim>           &q,
    SparseMatrixType                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Assemble the Laplace matrix and a right hand side vector. If no
   * coefficient is given, it is assumed to be constant one.
   * In case you want to specify @p constraints and use the default argument
   * for the coefficient you have to specify the (unused) coefficient argument
   * as <code>(const Function<spacedim> *const)nullptr</code>.
   *
   * If the library is configured to use multithreading, this function works
   * in parallel.
   *
   * The optional argument @p constraints allows to apply constraints on the
   * resulting matrix directly. Note, however, that this becomes difficult
   * when you have inhomogeneous constraints and later want to add several
   * such matrices, for example in time dependent settings such as the main
   * loop of step-26.
   *
   * See the general documentation of this namespace for more information.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const Mapping<dim, spacedim>                                    &mapping,
    const DoFHandler<dim, spacedim>                                 &dof,
    const Quadrature<dim>                                           &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Call the create_laplace_matrix() function, see above, with
   * <tt>mapping=MappingQ@<dim@>(1)</tt>.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const DoFHandler<dim, spacedim>                                 &dof,
    const Quadrature<dim>                                           &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Like the functions above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const hp::MappingCollection<dim, spacedim> &mapping,
    const DoFHandler<dim, spacedim>            &dof,
    const hp::QCollection<dim>                 &q,
    SparseMatrixType                           &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Like the functions above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const DoFHandler<dim, spacedim> &dof,
    const hp::QCollection<dim>      &q,
    SparseMatrixType                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Like the functions above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const hp::MappingCollection<dim, spacedim>                      &mapping,
    const DoFHandler<dim, spacedim>                                 &dof,
    const hp::QCollection<dim>                                      &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Like the functions above, but for hp-objects.
   */
  template <int dim, int spacedim, typename SparseMatrixType>
  void
  create_laplace_matrix(
    const DoFHandler<dim, spacedim>                                 &dof,
    const hp::QCollection<dim>                                      &q,
    SparseMatrixType                                                &matrix,
    const Function<spacedim, typename SparseMatrixType::value_type> &rhs,
    Vector<typename SparseMatrixType::value_type>                   &rhs_vector,
    const Function<spacedim, typename SparseMatrixType::value_type> *const a =
      nullptr,
    const AffineConstraints<typename SparseMatrixType::value_type> &
      constraints = AffineConstraints<typename SparseMatrixType::value_type>());

  /**
   * Exception
   */
  DeclExceptionMsg(ExcComponentMismatch,
                   "You are providing either a right hand side function or a "
                   "coefficient with a number of vector components that is "
                   "inconsistent with the rest of the arguments. If you do "
                   "provide a coefficient or right hand side function, then "
                   "it either needs to have as many components as the finite "
                   "element in use, or only a single vector component. In "
                   "the latter case, the same value will be taken for "
                   "each vector component of the finite element.");
} // namespace MatrixCreator


DEAL_II_NAMESPACE_CLOSE

#endif
