/*
 * ------------------------------------------------------------------------------------------------------------
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * Copyright (c) 2018-2019 Lawrence Livermore National Security LLC
 * Copyright (c) 2018-2019 The Board of Trustees of the Leland Stanford Junior University
 * Copyright (c) 2018-2019 Total, S.A
 * Copyright (c) 2019-     GEOSX Contributors
 * All right reserved
 *
 * See top level LICENSE, COPYRIGHT, CONTRIBUTORS, NOTICE, and ACKNOWLEDGEMENTS files for details.
 * ------------------------------------------------------------------------------------------------------------
 */

#include "gtest/gtest.h"

#include "constitutive/ConstitutiveManager.hpp"
#include "constitutive/solid/ModifiedCamClay.hpp"

#include "dataRepository/xmlWrapper.hpp"

using namespace geosx;
using namespace ::geosx::constitutive;


// NOTE: using this for debugging, will set up proper unit tests later

TEST( ModifiedCamClayTests, testModel )
{
  ConstitutiveManager constitutiveManager( "constitutive", nullptr );

  string const inputStream =
    "<Constitutive>"
    "   <ModifiedCamClay"
    "      name=\"granite\" "
    "      defaultDensity=\"2700\" "
    "      defaultRefPInvariant=\"-0.5\" "
    "      defaultRefElasticStrainVolumetric=\"0.0\" "
    "      defaultRefShearModulus=\"10.0\" "
    "      defaultShearModulusEvolution=\"0.0\" "
    "      defaultVirginCompressionIndex=\"0.1\" "
    "      defaultRecompressionIndex=\"0.01\" "
    "      defaultCriticalStateSlope=\"1.0\" "
    "      defaultAssociativity=\"1.0\" "
    "      defaultPreconsolidationPressure=\"-1.0\"/>"
    "</Constitutive>";

  xmlWrapper::xmlDocument xmlDocument;
  xmlWrapper::xmlResult xmlResult = xmlDocument.load_buffer( inputStream.c_str(), inputStream.size() );
  if( !xmlResult )
  {
    GEOSX_LOG_RANK_0( "XML parsed with errors!" );
    GEOSX_LOG_RANK_0( "Error description: " << xmlResult.description());
    GEOSX_LOG_RANK_0( "Error offset: " << xmlResult.offset );
  }

  xmlWrapper::xmlNode xmlConstitutiveNode = xmlDocument.child( "Constitutive" );
  constitutiveManager.ProcessInputFileRecursive( xmlConstitutiveNode );
  constitutiveManager.PostProcessInputRecursive();
  
  localIndex constexpr numElem = 2;
  localIndex constexpr numQuad = 4;
  
  dataRepository::Group disc( "discretization", nullptr );
  disc.resize( numElem );
  
  ModifiedCamClay & cm = *(constitutiveManager.GetConstitutiveRelation<ModifiedCamClay>("granite"));
  cm.AllocateConstitutiveData( &disc, numQuad );
  
  EXPECT_EQ( cm.size(), numElem );
  EXPECT_EQ( cm.numQuadraturePoints(), numQuad );
  
  ModifiedCamClay::KernelWrapper cmw = cm.createKernelWrapper();
  
  real64 inc = -1e-4; // compression
  real64 total = 0;
  real64 stress11 = 0;
    
  array2d< real64 > strainIncrement(1,6);
                    strainIncrement = 0;
                    strainIncrement[0][0] = inc;
  
  array2d< real64 > stress(1,6);
  array3d< real64 > stiffness(1,6,6);
  array2d< real64 > deviator(1,6);

  for(localIndex loadstep=0; loadstep < 40; ++loadstep)
  {
    cmw.SmallStrainUpdate(0,0,strainIncrement[0],stress[0],stiffness[0]);
    cmw.SaveConvergedState(0,0);
    total += inc;
    stress11 = stress[0][0];
      
    real64 mean = (stress[0][0]+stress[0][1]+stress[0][2])/3.;
    
    for(localIndex i=0; i<3; ++i)
    {
      deviator[0][i] = stress[0][i] - mean;
      deviator[0][i+3] = stress[0][i+3];
    }
  
    real64 invariantQ = 0;
    for(localIndex i=0; i<3; ++i)
    {
      invariantQ += deviator[0][i]*deviator[0][i];
      invariantQ += deviator[0][i+3]*deviator[0][i+3];
    }
    invariantQ = std::sqrt(invariantQ) + 1e-15; // perturbed to avoid divide by zero when Q=0;
  
    for(localIndex i=0; i<6; ++i)
    {
      deviator[0][i] /= invariantQ; // normalized deviatoric direction, "nhat"
    }
    invariantQ *= std::sqrt(3./2.);
    
    std::cout << mean << " " << invariantQ << " " << total << std::endl;
  }
  
  for(localIndex i=0; i<6; ++i)
  {
    for(localIndex j=0; j<6; ++j)
    {
      std::cout << stiffness[0][i][j] << " ";
    }
    std::cout << "\n";
  }
  
  // finite-difference check of tangent stiffness
  
  array2d< real64 > fd_stiffness(6,6);
  array2d< real64 > pstress(1,6);
  array3d< real64 > pstiffness(1,6,6);
  
  real64 eps = 1e-8;
  
  cmw.SmallStrainUpdate(0,0,strainIncrement[0],stress[0],stiffness[0]);    
  for(localIndex i=0; i<6; ++i)
  {
    strainIncrement[0][i] -= eps;
    
    if(i>0)
    {
      strainIncrement[0][i-1] += eps;
    }
    
    cmw.SmallStrainUpdate(0,0,strainIncrement[0],pstress[0],pstiffness[0]);
   for(localIndex j=0; j<6; ++j)
   {
      std::cout <<pstress[0][j] <<std::endl;
   }
      
    for(localIndex j=0; j<6; ++j)
    {
      fd_stiffness[j][i] = (pstress[0][j]-stress[0][j])/-eps;
    }
  }
  
  for(localIndex i=0; i<6; ++i)
  {
    for(localIndex j=0; j<6; ++j)
    {
      std::cout << fd_stiffness[i][j] << " ";
    }
    std::cout << "\n";
  }
  
}



