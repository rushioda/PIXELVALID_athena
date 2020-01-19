/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TRTParametersForBarrelHits.h"

// Package headers
#include "TRTProcessingOfBarrelHits.h"

// Athena headers
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "TRT_G4Utilities/TRTParameters.hh"

// Geant4 headers
#include "G4ThreeVector.hh"

// STL headers
#include <fstream>

// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForBarrelHits::TRTParametersForBarrelHits
(TRTProcessingOfBarrelHits* pProcessing): pProcessingOfBarrelHits(pProcessing),
                                          verboseLevel(pProcessing->verboseLevel)
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (verboseLevel>5) { G4cout << "##### Constructor TRTParametersForBarrelHits" << G4endl; }

  DefineParameters();

  if (pParameters->GetInteger("PrintParametersForBarrelHits"))
    PrintParameters();

  if (verboseLevel>5) { G4cout << "##### Constructor TRTParametersForBarrelHits done" << G4endl; }
}


// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTParametersForBarrelHits::~TRTParametersForBarrelHits()
{
  if (verboseLevel>5) { G4cout << "##### Destructor TRTParametersForBarrelHits" << G4endl; }
  if (verboseLevel>5) { G4cout << "##### Destructor TRTParametersForBarrelHits done" << G4endl; }
}


// Called by TRTParametersForBarrelHits

void TRTParametersForBarrelHits::DefineParameters() const
{
  if (verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::DefineParameters" << G4endl; }

  // Number of straw layers A, B, C:
  int numberOfStrawLayersA =
    pParameters->GetInteger("NumberOfStrawLayersA");
  int numberOfStrawLayersB =
    pParameters->GetInteger("NumberOfStrawLayersB");
  int numberOfStrawLayersC =
    pParameters->GetInteger("NumberOfStrawLayersC");

  // Integral distribution of straws A:
  int* numberOfStrawsInLayersA = new int[numberOfStrawLayersA];
  pParameters->GetIntegerArray("NumberOfStrawsInLayersA", numberOfStrawLayersA,
                               numberOfStrawsInLayersA);

  int* integralDistributionOfStrawsA = new int[numberOfStrawLayersA];
  integralDistributionOfStrawsA[0] = numberOfStrawsInLayersA[0];
  int i;
  for (i = 1; i < numberOfStrawLayersA; ++i)
    integralDistributionOfStrawsA[i] = integralDistributionOfStrawsA[i - 1] +
      numberOfStrawsInLayersA[i];

  for (i = 0; i < numberOfStrawLayersA; ++i)
    pProcessingOfBarrelHits->integralDistributionOfStrawsA[i] =
      integralDistributionOfStrawsA[i];

  delete [] numberOfStrawsInLayersA;

  // Integral distribution of straws B:
  int* numberOfStrawsInLayersB = new int[numberOfStrawLayersB];
  pParameters->GetIntegerArray("NumberOfStrawsInLayersB", numberOfStrawLayersB,
                               numberOfStrawsInLayersB);

  int* integralDistributionOfStrawsB = new int[numberOfStrawLayersB];
  integralDistributionOfStrawsB[0] = numberOfStrawsInLayersB[0];
  for (i = 1; i < numberOfStrawLayersB; ++i)
    integralDistributionOfStrawsB[i] = integralDistributionOfStrawsB[i - 1] +
      numberOfStrawsInLayersB[i];

  for (i = 0; i < numberOfStrawLayersB; ++i)
    pProcessingOfBarrelHits->integralDistributionOfStrawsB[i] =
      integralDistributionOfStrawsB[i];

  delete [] numberOfStrawsInLayersB;

  // Integral distribution of straws C:
  int* numberOfStrawsInLayersC = new int[numberOfStrawLayersC];
  pParameters->GetIntegerArray("NumberOfStrawsInLayersC", numberOfStrawLayersC,
                               numberOfStrawsInLayersC);

  int* integralDistributionOfStrawsC = new int[numberOfStrawLayersC];
  integralDistributionOfStrawsC[0] = numberOfStrawsInLayersC[0];
  for (i = 1; i < numberOfStrawLayersC; ++i)
    integralDistributionOfStrawsC[i] = integralDistributionOfStrawsC[i - 1] +
      numberOfStrawsInLayersC[i];

  for (i = 0; i < numberOfStrawLayersC; ++i)
    pProcessingOfBarrelHits->integralDistributionOfStrawsC[i] =
      integralDistributionOfStrawsC[i];

  delete [] numberOfStrawsInLayersC;

  if (pParameters->GetInteger("BarrelIdentifier") == 2)
    {
      // Arrays strawIDToLayerID for modules A, B, C:
      int layerID = 0;
      for (i = 0; i < pProcessingOfBarrelHits->numberOfStrawsA; ++i)
        {
          if (i + 1 > integralDistributionOfStrawsA[layerID])
            ++layerID;
          pProcessingOfBarrelHits->strawIDToLayerIDA[i] = layerID;
        }

      layerID = 0;
      for (i = 0; i < pProcessingOfBarrelHits->numberOfStrawsB; ++i)
        {
          if (i + 1 > integralDistributionOfStrawsB[layerID])
            ++layerID;
          pProcessingOfBarrelHits->strawIDToLayerIDB[i] = layerID;
        }

      layerID = 0;
      for (i = 0; i < pProcessingOfBarrelHits->numberOfStrawsC; ++i)
        {
          if (i + 1 > integralDistributionOfStrawsC[layerID])
            ++layerID;
          pProcessingOfBarrelHits->strawIDToLayerIDC[i] = layerID;
        }
    }

  delete [] integralDistributionOfStrawsA;
  delete [] integralDistributionOfStrawsB;
  delete [] integralDistributionOfStrawsC;


  if (verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::DefineParameters done" << G4endl; }

}


// Called by TRTParametersForBarrelHits

void TRTParametersForBarrelHits::PrintParameters() const
{

  if (verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::PrintParameters" << G4endl; }

  TRTOutputFile* pOutputFile = TRTOutputFile::GetPointer();

  std::ofstream& output = pOutputFile->GetReference();

  output << std::endl;
  output << "***** TRTParametersForBarrelHits::PrintParameters *****"
         << std::endl;

  output << "Parameters of barrel straws:" << std::endl;

  if (pParameters->GetInteger("BarrelIdentifier") == 2)
    {
      int numberOfStrawsA = pProcessingOfBarrelHits->numberOfStrawsA;
      int numberOfStrawsB = pProcessingOfBarrelHits->numberOfStrawsB;
      int numberOfStrawsC = pProcessingOfBarrelHits->numberOfStrawsC;

      output << "  numberOfStrawsA=" << numberOfStrawsA << std::endl;
      output << "  numberOfStrawsB=" << numberOfStrawsB << std::endl;
      output << "  numberOfStrawsC=" << numberOfStrawsC << std::endl;
    }


  int numberOfStrawLayersA = pProcessingOfBarrelHits->numberOfStrawLayersA;
  int numberOfStrawLayersB = pProcessingOfBarrelHits->numberOfStrawLayersB;
  int numberOfStrawLayersC = pProcessingOfBarrelHits->numberOfStrawLayersC;

  output << "  numberOfStrawLayersA=" << numberOfStrawLayersA << std::endl;
  output << "  numberOfStrawLayersB=" << numberOfStrawLayersB << std::endl;
  output << "  numberOfStrawLayersC=" << numberOfStrawLayersC << std::endl;

  output << std::endl << "Integral distributions of straws in layers:"
         << std::endl;

  int i;
  for (i = 0; i < numberOfStrawLayersA; ++i)
    output << "  integralDistributionOfStrawsA[" << i << "]="
           << pProcessingOfBarrelHits->integralDistributionOfStrawsA[i]
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfStrawLayersB; ++i)
    output << "  integralDistributionOfStrawsB[" << i << "]="
           << pProcessingOfBarrelHits->integralDistributionOfStrawsB[i]
           << std::endl;
  output << std::endl;

  for (i = 0; i < numberOfStrawLayersC; ++i)
    output << "  integralDistributionOfStrawsC[" << i << "]="
           << pProcessingOfBarrelHits->integralDistributionOfStrawsC[i]
           << std::endl;

  if ((pParameters->GetInteger("BarrelIdentifier") == 2) &&
      pParameters->GetInteger("PrintParametersForBarrelHits") == 2)
    {
      output << std::endl << "Arrays barrel straw ID to layer ID:" << std::endl;

      int numberOfStrawsA = pProcessingOfBarrelHits->numberOfStrawsA;
      int numberOfStrawsB = pProcessingOfBarrelHits->numberOfStrawsB;
      int numberOfStrawsC = pProcessingOfBarrelHits->numberOfStrawsC;

      int i;
      for (i = 0; i < numberOfStrawsA; ++i)
        output << "  strawIDToLayerIDA[" << i << "]="
               << pProcessingOfBarrelHits->strawIDToLayerIDA[i] << std::endl;
      output << std::endl;

      for (i = 0; i < numberOfStrawsB; ++i)
        output << "  strawIDToLayerIDB[" << i << "]="
               << pProcessingOfBarrelHits->strawIDToLayerIDB[i] << std::endl;
      output << std::endl;

      for (i = 0; i < numberOfStrawsC; ++i)
        output << "  strawIDToLayerIDC[" << i << "]="
               << pProcessingOfBarrelHits->strawIDToLayerIDC[i] << std::endl;
    }

  output << std::endl;

  if (verboseLevel>5) { G4cout << "######### Method TRTParametersForBarrelHits::PrintParameters done" << G4endl; }
}
