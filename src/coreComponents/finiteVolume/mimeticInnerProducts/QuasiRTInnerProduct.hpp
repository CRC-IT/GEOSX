/*
 * ------------------------------------------------------------------------------------------------------------
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * Copyright (c) 2018-2020 Lawrence Livermore National Security LLC
 * Copyright (c) 2018-2020 The Board of Trustees of the Leland Stanford Junior University
 * Copyright (c) 2018-2020 Total, S.A
 * Copyright (c) 2019-     GEOSX Contributors
 * All rights reserved
 *
 * See top level LICENSE, COPYRIGHT, CONTRIBUTORS, NOTICE, and ACKNOWLEDGEMENTS files for details.
 * ------------------------------------------------------------------------------------------------------------
 */

/**
 * @file QuasiRTInnerProduct.hpp
 */

#ifndef GEOSX_FINITEVOLUME_MIMETICINNERPRODUCTS_QUASIRTINNERPRODUCT_HPP_
#define GEOSX_FINITEVOLUME_MIMETICINNERPRODUCTS_QUASIRTINNERPRODUCT_HPP_

#include "finiteVolume/mimeticInnerProducts/MimeticInnerProductBase.hpp"

namespace geosx
{
namespace mimeticInnerProduct
{

/**
 * @class QuasiRTInnerProduct
 *
 * Provides an implementation of a quasi Raviart-Thomas inner product in the hybrid FVM solvers
 */
class QuasiRTInnerProduct : public MimeticInnerProductBase
{
public:

  /**
   * @brief In a given element, recompute the transmissibility matrix using the quasi Raviart-Thomas inner product.
   * @param[in] nodePosition the position of the nodes
   * @param[in] faceToNodes the map from the face to their nodes
   * @param[in] elemToFaces the maps from the one-sided face to the corresponding face
   * @param[in] elemCenter the center of the element
   * @param[in] elemVolume the volume of the element
   * @param[in] elemPerm the permeability in the element
   * @param[in] lengthTolerance the tolerance used in the trans calculations
   * @param[inout] transMatrix the output
   *
   * @details Reference: K-A Lie, An Introduction to Reservoir Simulation Using MATLAB/GNU Octave (2019)
   */
  template< localIndex NF >
  GEOSX_HOST_DEVICE
  static void
  Compute( arrayView2d< real64 const, nodes::REFERENCE_POSITION_USD > const & nodePosition,
           ArrayOfArraysView< localIndex const > const & faceToNodes,
           arraySlice1d< localIndex const > const & elemToFaces,
           arraySlice1d< real64 const > const & elemCenter,
           real64 const & elemVolume,
           real64 const (&elemPerm)[ 3 ],
           real64 const & lengthTolerance,
           arraySlice2d< real64 > const & transMatrix );

};

template< localIndex NF >
GEOSX_HOST_DEVICE
void
QuasiRTInnerProduct::Compute( arrayView2d< real64 const, nodes::REFERENCE_POSITION_USD > const & nodePosition,
                              ArrayOfArraysView< localIndex const > const & faceToNodes,
                              arraySlice1d< localIndex const > const & elemToFaces,
                              arraySlice1d< real64 const > const & elemCenter,
                              real64 const & elemVolume,
                              real64 const (&elemPerm)[ 3 ],
                              real64 const & lengthTolerance,
                              arraySlice2d< real64 > const & transMatrix )
{
  MimeticInnerProductBase::ComputeParametricInnerProduct< NF >( nodePosition,
                                                                faceToNodes,
                                                                elemToFaces,
                                                                elemCenter,
                                                                elemVolume,
                                                                elemPerm,
                                                                6.0,
                                                                lengthTolerance,
                                                                transMatrix );
}

} // end namespace mimeticInnerProduct

} // end namespace geosx

#endif //GEOSX_FINITEVOLUME_MIMETICINNERPRODUCTS_QUASIRTINNERPRODUCT_HPP_
