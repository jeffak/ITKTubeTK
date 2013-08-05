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

#ifndef __tubeCropROI_hxx
#define __tubeCropROI_hxx

#include "tubeCropROI.h"

#include "tubeCLIFilterWatcher.h"
#include "tubeMessage.h"

namespace tube
{

template< class TPixel, unsigned int TDimension >
CropROI< TPixel, TDimension >
::CropROI( void )
{
  m_InputImage = NULL;
  m_OutputImage = NULL;
  m_ROIMin.Fill( 0 );
  m_UseROIMin = false;
  m_ROIMax.Fill( 0 );
  m_UseROIMax = false;
  m_ROISize.Fill( 0 );
  m_UseROISize = false;
  m_ROICenter.Fill( 0 );
  m_UseROICenter = false;
  m_ROIBoundary.Fill( 0 );
  m_UseROIBoundary = false;

  m_TimeCollector = NULL;
  m_ProgressReporter = NULL;
  m_ProgressStart = 0;
  m_ProgressRange = 1;
}


template< class TPixel, unsigned int TDimension >
CropROI< TPixel, TDimension >
::~CropROI( void )
{
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetInput( typename ImageType::Pointer inputImage )
{
  m_InputImage = inputImage;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetMin( typename ImageType::IndexType roiMin )
{
  m_ROIMin = roiMin;
  m_UseROIMin = true;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetUseMin( bool useMin )
{
  m_UseROIMin = useMin;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetMax( typename ImageType::IndexType roiMax )
{
  m_ROIMax = roiMax;
  m_UseROIMax = true;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetUseMax( bool useMax )
{
  m_UseROIMax = useMax;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetSize( typename ImageType::SizeType roiSize )
{
  m_ROISize = roiSize;
  m_UseROISize = true;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetUseSize( bool useSize )
{
  m_UseROISize = useSize;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetCenter( typename ImageType::IndexType roiCenter )
{
  m_ROICenter = roiCenter;
  m_UseROICenter = true;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetUseCenter( bool useCenter )
{
  m_UseROICenter = useCenter;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetBoundary( typename ImageType::IndexType roiBoundary )
{
  m_ROIBoundary = roiBoundary;
  m_UseROIBoundary = true;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension >
::SetUseBoundary( bool useBoundary )
{
  m_UseROIBoundary = useBoundary;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension>
::SetTimeCollector( itk::TimeProbesCollectorBase * timeCollector )
{
  m_TimeCollector = timeCollector;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension>
::SetProgressReporter( CLIProgressReporter * progressReporter,
  float progressStart,
  float progressRange )
{
  m_ProgressReporter = progressReporter;
  m_ProgressStart = progressStart;
  m_ProgressRange = progressRange;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension>
::Update( void )
{
  if( m_ProgressReporter )
    {
    m_ProgressReporter->Start();
    }
  if( m_TimeCollector )
    {
    m_TimeCollector->Start("CropROI");
    }

  if( m_UseROISize || m_UseROIMin || m_UseROIMax )
    {
    if( m_UseROISize && m_UseROIMax )
      {
      ErrorMessage(
        "You must specify either --size or --max options.  Not both." );
      throw( "CropROI: specify only one of UseSize or UseMax.  Not Both.");
      }

    typename CropFilterType::Pointer cropFilter;

    cropFilter = CropFilterType::New();
    cropFilter->SetInput( m_InputImage );

    if( !m_UseROIMin && !m_UseROICenter )
      {
      for( unsigned int i=0; i<TDimension; i++ )
        {
        m_ROIMin[i] = 0;
        }
      }

    if( m_UseROICenter )
      {
      for( unsigned int i=0; i<TDimension; i++ )
        {
        m_ROIMin[i] = m_ROICenter[i] - m_ROISize[i]/2;
        }
      }

    typename ImageType::SizeType imageSize;
    imageSize = m_InputImage->GetLargestPossibleRegion().GetSize();
    for( unsigned int i=0; i<TDimension; i++ )
      {
      if( m_ROIMin[i] < 0 )
        {
        ErrorMessage( "Min is less than 0." );
        throw( "Min is less than 0." );
        }
      if( m_ROIMin[i] >= (int)(imageSize[i]) )
        {
        ErrorMessage( "Min is larger than image size." );
        throw( "Min is larger than image size." );
        }
      }

    typename ImageType::SizeType outputSize;
    outputSize = m_InputImage->GetLargestPossibleRegion().GetSize();
    if( m_UseROISize )
      {
      for( unsigned int i=0; i<TDimension; i++ )
        {
        outputSize[i] = m_ROISize[i];
        if( outputSize[i] < 1 )
          {
          outputSize[i] = 1;
          }
        }
      }
    else
      {
      for( unsigned int i=0; i<TDimension; i++ )
        {
        if( m_ROIMin[i] > m_ROIMax[i] )
          {
          int tf = m_ROIMin[i];
          m_ROIMin[i] = m_ROIMax[i];
          m_ROIMax[i] = tf;
          }
        outputSize[i] = m_ROIMax[i]-m_ROIMin[i]+1;
        }
      }

    if( m_UseROIBoundary )
      {
      for( unsigned int i=0; i<TDimension; i++ )
        {
        m_ROIMin[i] -= m_ROIBoundary[i];
        outputSize[i] += 2*m_ROIBoundary[i];
        }
      }

    for( unsigned int i=0; i<TDimension; i++ )
      {
      if( m_ROIMin[i] + outputSize[i] > imageSize[i] )
        {
        outputSize[i] = imageSize[i] - m_ROIMin[i];
        }
      if( m_ROIMin[i] < 0 )
        {
        outputSize[i] += m_ROIMin[i];
        m_ROIMin[i] = 0;
        }
      }

    typename ImageType::SizeType lowerCropSize;
    typename ImageType::SizeType upperCropSize;
    for( unsigned int i=0; i<TDimension; i++ )
      {
      lowerCropSize[i] = m_ROIMin[i];
      upperCropSize[i] = imageSize[i] - (m_ROIMin[i] + outputSize[i]);
      }

    cropFilter->SetLowerBoundaryCropSize( lowerCropSize );
    cropFilter->SetUpperBoundaryCropSize( upperCropSize );

    if( m_ProgressReporter )
      {
      CLIFilterWatcher watcher( cropFilter,
                               "cropFilterWatcher",
                               m_ProgressReporter->GetProcessInformation(),
                               m_ProgressRange,
                               m_ProgressStart,
                               true );
      }

    try
      {
      cropFilter->Update();
      }
    catch( itk::ExceptionObject & e )
      {
      std::stringstream out;
      out << "itk exception: ";
      out << e;
      ErrorMessage( out.str() );
      throw( out.str() );
      }
    catch( ... )
      {
      ErrorMessage( "Crop filter: generic exception" );
      throw( "Crop filter: generic exception" );
      }

    m_OutputImage = cropFilter->GetOutput();

    if( m_TimeCollector )
      {
      m_TimeCollector->Stop("CropROI");
      }

    if( m_ProgressReporter )
      {
      m_ProgressReporter->Report( m_ProgressStart + 1.0*m_ProgressRange );
      }
    }
}


template< class TPixel, unsigned int TDimension >
typename itk::Image< TPixel, TDimension >::Pointer
CropROI< TPixel, TDimension>
::GetOutput( void )
{
  return m_OutputImage;
}


template< class TPixel, unsigned int TDimension >
void
CropROI< TPixel, TDimension>
::PrintSelf( void )
{
  if( m_InputImage.IsNotNull() )
    {
    std::cout << "Input Image: " << m_InputImage << std::endl;
    }
  else
    {
    std::cout << "Input Image: NULL" << std::endl;
    }

  if( m_OutputImage.IsNotNull() )
    {
    std::cout << "Output Image: " << m_OutputImage << std::endl;
    }
  else
    {
    std::cout << "Output Image: NULL" << std::endl;
    }

  std::cout << "ROIMin: " << m_ROIMin << std::endl;
  if( m_UseROIMin )
    {
    std::cout << "Use ROIMin: true" << std::endl;
    }
  else
    {
    std::cout << "Use ROIMin: false" << std::endl;
    }

  std::cout << "ROIMax: " << m_ROIMax << std::endl;
  if( m_UseROIMax )
    {
    std::cout << "Use ROIMax: true" << std::endl;
    }
  else
    {
    std::cout << "Use ROIMax: false" << std::endl;
    }

  std::cout << "ROISize: " << m_ROISize << std::endl;
  if( m_UseROISize )
    {
    std::cout << "Use ROISize: true" << std::endl;
    }
  else
    {
    std::cout << "Use ROISize: false" << std::endl;
    }

  std::cout << "ROICenter: " << m_ROICenter << std::endl;
  if( m_UseROICenter )
    {
    std::cout << "Use ROICenter: true" << std::endl;
    }
  else
    {
    std::cout << "Use ROICenter: false" << std::endl;
    }

  std::cout << "ROIBoundary: " << m_ROIBoundary << std::endl;
  if( m_UseROIBoundary )
    {
    std::cout << "Use ROIBoundary: true" << std::endl;
    }
  else
    {
    std::cout << "Use ROIBoundary: false" << std::endl;
    }

  if( m_TimeCollector == NULL )
    {
    std::cout << "Time Collector: NULL" << std::endl;
    }
  else
    {
    std::cout << "Time Collector: Not NULL" << std::endl;
    }
}

} // End namespace tube

#endif // End !defined(__tubeCropROI_hxx)
