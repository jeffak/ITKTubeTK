/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#include "itktubeAnisotropicCoherenceEnhancingDiffusionImageFilter.h"
#include "tubeCLIFilterWatcher.h"
#include "tubeCLIProgressReporter.h"
#include "tubeMessage.h"

#include <itkImageFileReader.h>
#include <itkOrientImageFilter.h>
#include <itkTimeProbesCollectorBase.h>

#include "EnhanceCoherenceUsingDiffusionCLP.h"

template< class TPixel, unsigned int VDimension >
int DoIt( int argc, char * argv[] );

// Must follow include of "...CLP.h" and forward declaration of int DoIt( ... ).
#include "tubeCLIHelperFunctions.h"

template< class TPixel, unsigned int VDimension >
int DoIt( int argc, char * argv[] )
{
  PARSE_ARGS;

  // The timeCollector is used to perform basic profiling of the components
  //   of your algorithm.
  itk::TimeProbesCollectorBase timeCollector;

  // CLIProgressReporter is used to communicate progress with the Slicer GUI
  tube::CLIProgressReporter    progressReporter(
    "CoherenceEnhancingAnisotropicDiffusion", CLPProcessInformation );
  progressReporter.Start();

  // Define the types and dimension of the images
  // Use the input image to dictate the type of the image reader/writer,
  // but use double for the filter to avoid rounding off errors in the
  // filter's floating point operations
  enum { Dimension = 3 };
  typedef TPixel                                   ImagePixelType;
  typedef itk::Image< ImagePixelType, Dimension >  InputImageType;
  typedef itk::Image< ImagePixelType, Dimension >  OutputImageType;
  typedef double                                   FilterPixelType;
  typedef itk::Image< FilterPixelType, Dimension > FilterInputImageType;
  typedef itk::Image< FilterPixelType, Dimension > FilterOutputImageType;

  // Read the input volume
  timeCollector.Start("Load data");
  typedef itk::ImageFileReader< InputImageType  >  ImageReaderType;
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( inputVolume.c_str() );
  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    tube::ErrorMessage( "Reading volume: Exception caught: "
                        + std::string(err.GetDescription()) );
    timeCollector.Report();
    return EXIT_FAILURE;
    }
  timeCollector.Stop("Load data");
  double progress = 0.1;
  progressReporter.Report( progress );

  // C-style cast from input image type to type 'double'
  typedef itk::CastImageFilter< InputImageType, FilterInputImageType >
    CastInputImageFilterType;
  typename CastInputImageFilterType::Pointer castInputImageFilter =
    CastInputImageFilterType::New();
  castInputImageFilter->SetInput( reader->GetOutput() );

  // Reorient to axial because the anisotropic diffusion tensor function does
  // not handle direction
  typedef itk::OrientImageFilter< FilterInputImageType, FilterInputImageType >
      OrientInputFilterType;
  typename OrientInputFilterType::Pointer orientInputFilter
      = OrientInputFilterType::New();
  orientInputFilter->UseImageDirectionOn();
  orientInputFilter->SetDesiredCoordinateOrientationToAxial();
  orientInputFilter->SetInput( castInputImageFilter->GetOutput() );

  // Perform the coherence enhancing anisotropic diffusion
  timeCollector.Start("Coherence enhancing anisotropic diffusion");

  // Declare the anisotropic diffusion coherence enhancing filter
  typedef itk::tube::AnisotropicCoherenceEnhancingDiffusionImageFilter<
    FilterInputImageType, FilterOutputImageType>
    CoherenceEnhancingFilterType;

  // Create a coherence enhancing filter
  typename CoherenceEnhancingFilterType::Pointer CoherenceEnhancingFilter =
    CoherenceEnhancingFilterType::New();

  CoherenceEnhancingFilter->SetInput( orientInputFilter->GetOutput() );

  //Set/Get CED parameters
  CoherenceEnhancingFilter->SetSigma( scaleParameter );
  CoherenceEnhancingFilter->SetAlpha( alpha );
  CoherenceEnhancingFilter->SetContrastParameterLambdaC(
    cedContrastParameter );
  CoherenceEnhancingFilter->SetTimeStep( timeStep );
  CoherenceEnhancingFilter->SetNumberOfIterations( numberOfIterations );

  double progressFraction = 0.8;
  tube::CLIFilterWatcher watcher( CoherenceEnhancingFilter,
    "Coherence enhancing anisotropic diffusion", CLPProcessInformation,
    progressFraction, progress, true );

  try
    {
    CoherenceEnhancingFilter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    tube::ErrorMessage(
      "Coherence enhancing anisotropic diffusion: Exception caught: "
      + std::string(err.GetDescription()) );
    timeCollector.Report();
    return EXIT_FAILURE;
    }

  timeCollector.Stop("Coherence enhancing anisotropic diffusion");
  progress = 0.9;
  progressReporter.Report( progress );

  // C-style cast from double to output image type
  typedef itk::CastImageFilter< FilterOutputImageType, OutputImageType >
    CastOutputImageFilterType;
  typename CastOutputImageFilterType::Pointer castOutputImageFilter =
    CastOutputImageFilterType::New();
  castOutputImageFilter->SetInput( CoherenceEnhancingFilter->GetOutput() );

  // Reorient back from axial to whatever direction we had before
  typedef itk::OrientImageFilter< OutputImageType, OutputImageType >
      OrientOutputFilterType;
  typename OrientOutputFilterType::Pointer orientOutputFilter
      = OrientOutputFilterType::New();
  orientOutputFilter->UseImageDirectionOn();
  orientOutputFilter->SetDesiredCoordinateDirection(
      reader->GetOutput()->GetDirection() );
  orientOutputFilter->SetInput( castOutputImageFilter->GetOutput() );

  // Save output data
  timeCollector.Start("Save data");
  typedef itk::ImageFileWriter< OutputImageType  >      ImageWriterType;
  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName( outputVolume.c_str() );
  writer->SetUseCompression( true );
  writer->SetInput( orientOutputFilter->GetOutput() );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    tube::ErrorMessage( "Writing volume: Exception caught: "
                        + std::string(err.GetDescription()) );
    timeCollector.Report();
    return EXIT_FAILURE;
    }

  timeCollector.Stop("Save data");

  progress = 1.0;
  progressReporter.Report( progress );
  progressReporter.End();
  timeCollector.Report();

  // All objects should be automatically destroyed at this point
  return EXIT_SUCCESS;
}

// Main
int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  // You may need to update this line if, in the project's .xml CLI file,
  //   you change the variable name for the inputVolume.
  return tube::ParseArgsAndCallDoIt( inputVolume, argc, argv );
}
