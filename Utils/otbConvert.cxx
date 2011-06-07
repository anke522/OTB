/*=========================================================================

 Program:   ORFEO Toolbox
 Language:  C++
 Date:      $Date$
 Version:   $Revision$


 Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
 See OTBCopyright.txt for details.


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/

#include "otbConvert.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbStreamingImageFileWriter.h"
#include "otbVectorRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "otbStandardFilterWatcher.h"
#include "otbStandardWriterWatcher.h"
#include "otbUnaryImageFunctorWithVectorImageFilter.h"
#include "otbStreamingShrinkImageFilter.h"
#include "itkListSample.h"
#include "otbListSampleToHistogramListGenerator.h"
#include "itkImageRegionConstIterator.h"

namespace otb
{

namespace Functor
{
template< class TScalar >
class ITK_EXPORT LogFunctor
{
public:
  LogFunctor(){};
  ~LogFunctor(){};
  TScalar operator() (const TScalar& v) const
  {
    return vcl_log(v);
  }
};
}

template<typename OutputPixelType>
int generic_main_convert(otb::ApplicationOptionsResult* parseResult)
{
  unsigned int ram = 256;

  if (parseResult->IsOptionPresent("AvailableMemory"))
    {
    ram = parseResult->GetParameterUInt("AvailableMemory");
    }

  typedef otb::VectorImage<double, 2> InputImageType;
  typedef otb::VectorImage<OutputPixelType, 2> OutputImageType;

  typedef otb::StreamingImageFileWriter<OutputImageType> WriterType;

  typename WriterType::Pointer writer=WriterType::New();

  writer->SetFileName(parseResult->GetOutputImage().c_str());

  if (parseResult->IsOptionPresent("UseRescale"))
    {
    unsigned int transferNum = 1;
    if (parseResult->IsOptionPresent("RescaleType"))
      {
      transferNum = parseResult->GetParameterUInt("RescaleType");
      }
    typedef otb::ImageFileReader<InputImageType> ReaderType;
    typename ReaderType::Pointer reader=ReaderType::New();
    reader->SetFileName(parseResult->GetInputImage().c_str());
    reader->UpdateOutputInformation();

    unsigned int nbComp(reader->GetOutput()->GetNumberOfComponentsPerPixel());

    //define the transfer log
    typedef otb::Functor::LogFunctor<InputImageType::InternalPixelType> TransferLogFunctor;
    typedef otb::UnaryImageFunctorWithVectorImageFilter<InputImageType, InputImageType, TransferLogFunctor> TransferLogType;
    TransferLogType::Pointer transferLog = TransferLogType::New();
    transferLog->SetInput(reader->GetOutput());
    transferLog->UpdateOutputInformation();

    typedef otb::VectorRescaleIntensityImageFilter<InputImageType, OutputImageType> RescalerType;
    typename OutputImageType::PixelType minimum;
    typename OutputImageType::PixelType maximum;
    minimum.SetSize(nbComp);
    maximum.SetSize(nbComp);
    minimum.Fill(itk::NumericTraits<OutputPixelType>::min());
    maximum.Fill(itk::NumericTraits<OutputPixelType>::max());

    typename RescalerType::Pointer rescaler=RescalerType::New();

    rescaler->SetOutputMinimum(minimum);
    rescaler->SetOutputMaximum(maximum);

    // We need to subsample the input image in order to estimate its
    // histogram
    typedef otb::StreamingShrinkImageFilter<InputImageType,InputImageType> ShrinkFilterType;
    typename ShrinkFilterType::Pointer shrinkFilter = ShrinkFilterType::New();
    
    // Shrink factor is computed so as to load a quicklook of 1000
    // pixels square at most
    typename InputImageType::SizeType imageSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    unsigned int shrinkFactor = std::max(imageSize[0],imageSize[1])/1000;

    std::cout<<"Shrink factor: "<<shrinkFactor<<std::endl;

    shrinkFilter->SetShrinkFactor(shrinkFactor);

    if (transferNum == 2)
      {
      shrinkFilter->SetInput(transferLog->GetOutput());
      rescaler->SetInput(transferLog->GetOutput());
      }
    else
      {
      shrinkFilter->SetInput(reader->GetOutput());
      rescaler->SetInput(reader->GetOutput());
      }

    otb::StandardWriterWatcher shrinkWatcher(shrinkFilter->GetStreamer(), shrinkFilter->GetFilter(),"Estimating image histogram for rescaling");

    shrinkFilter->GetStreamer()->SetAutomaticTiledStreaming(ram);

    shrinkFilter->Update();

    typedef itk::Statistics::ListSample<typename InputImageType::PixelType> ListSampleType;
    typedef itk::Statistics::DenseFrequencyContainer DFContainerType;

    typedef otb::ListSampleToHistogramListGenerator<ListSampleType,typename InputImageType::InternalPixelType,DFContainerType> HistogramsGeneratorType;
    itk::ImageRegionConstIterator<InputImageType> it(shrinkFilter->GetOutput(),shrinkFilter->GetOutput()->GetLargestPossibleRegion());

    typename ListSampleType::Pointer listSample = ListSampleType::New();

    // Now we generate the list of samples
    for(it.GoToBegin();!it.IsAtEnd();++it)
      {
      listSample->PushBack(it.Get());
      }

    // And then the histogram
    typename HistogramsGeneratorType::Pointer histogramsGenerator = HistogramsGeneratorType::New();
    histogramsGenerator->SetListSample(listSample);
    histogramsGenerator->SetNumberOfBins(255);
    histogramsGenerator->NoDataFlagOn();
    histogramsGenerator->Update();

    // And extract the 2% lower and upper quantile
    typename InputImageType::PixelType inputMin(nbComp),inputMax(nbComp);
    
    for(unsigned int i = 0; i < nbComp; ++i)
      {
      inputMin[i] = histogramsGenerator->GetOutput()->GetNthElement(i)->Quantile(0,0.02);
      inputMax[i] = histogramsGenerator->GetOutput()->GetNthElement(i)->Quantile(0,0.98);
      }
    
    
    rescaler->AutomaticInputMinMaxComputationOff();
    rescaler->SetInputMinimum(inputMin);
    rescaler->SetInputMaximum(inputMax);

    writer->SetInput(rescaler->GetOutput());
   
    writer->SetAutomaticTiledStreaming(ram);

    otb::StandardWriterWatcher watcher(writer, rescaler,"Conversion");

    writer->Update();
    }
  else
    {
    typedef otb::ImageFileReader<OutputImageType> ReaderType;
    typename ReaderType::Pointer reader=ReaderType::New();
    reader->SetFileName(parseResult->GetInputImage().c_str());
    otb::StandardFilterWatcher watcher(writer,"Conversion");
    writer->SetInput(reader->GetOutput());

    if (parseResult->IsOptionPresent("AvailableMemory"))
      {
      ram = parseResult->GetParameterUInt("AvailableMemory");
      }
    writer->SetAutomaticTiledStreaming(ram);

    writer->Update();
    }

  return EXIT_SUCCESS;
}

int Convert::Describe(ApplicationDescriptor* descriptor)
{
  descriptor->SetName("ConvertApplication");
  descriptor->SetDescription("Convert an image to a different format, eventually rescaling the data"
                             " and/or changing the pixel type");
  descriptor->AddInputImage();
  descriptor->AddOutputImage();
  descriptor->AddOption("OutputPixelType","OutputPixelType: unsigned char (1), short int (2), int (3), float (4),"
                        " double (5), unsigned short int (12), unsigned int (13); default 1","t", 1, false, ApplicationDescriptor::Integer);
  descriptor->AddOption("UseRescale", "Rescale value between output type min and max","r", 0, false, ApplicationDescriptor::Boolean);
  descriptor->AddOption("RescaleType", "Transfer function for the rescaling: linear (1), log (2); default 1","rt", 1, false, ApplicationDescriptor::Integer);
  descriptor->AddOption("AvailableMemory","Set the maximum of available memory for the pipeline execution in mega bytes (optional, 256 by default)","ram", 1, false, otb::ApplicationDescriptor::Integer);

  return EXIT_SUCCESS;
}

int Convert::Execute(otb::ApplicationOptionsResult* parseResult)
{

  try
    {
    unsigned int type=1;
    if (parseResult->IsOptionPresent("OutputPixelType"))
      {
      type=parseResult->GetParameterUInt("OutputPixelType");
      }
    
    switch (type)
      {
      case 1:
        generic_main_convert<unsigned char>(parseResult);
        break;
      case 2:
        generic_main_convert<short int>(parseResult);
        break;
      case 3:
        generic_main_convert<int>(parseResult);
        break;
      case 4:
        generic_main_convert<float>(parseResult);
        break;
      case 5:
        generic_main_convert<double>(parseResult);
        break;
      case 12:
        generic_main_convert<unsigned short int>(parseResult);
        break;
      case 13:
        generic_main_convert<unsigned int>(parseResult);
        break;
      default:
        generic_main_convert<unsigned char>(parseResult);
        break;
      }
    }
  catch ( itk::ExceptionObject & err )
    {
    std::cout << "Exception itk::ExceptionObject raised !" << std::endl;
    std::cout << err << std::endl;
    return EXIT_FAILURE;
    }
  catch ( std::bad_alloc & err )
    {
    std::cout << "Exception bad_alloc : "<<(char*)err.what()<< std::endl;
  return EXIT_FAILURE;
    }
  catch ( ... )
    {
    std::cout << "Unknow exception raised !" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
}
