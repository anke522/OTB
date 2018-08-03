/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef otbWrapperExpNumericalParameterInterface_h
#define otbWrapperExpNumericalParameterInterface_h

#include <cassert>
#include <memory>
#include "otbWrapperExpAnyNumeric.h"
#include "OTBApplicationEngineExport.h"

namespace otb
{
namespace WrapperExp
{

class NumericalParameterInterface
{
public:
  typedef NumericalParameterInterface  Self;
  typedef std::shared_ptr<Self>        Pointer;
  typedef std::shared_ptr<const Self>  ConstPointer;

  // itkTypeMacroNoParent( NumericalParameterInterface );

  // static Pointer New()
  //   {
  //   return std::shared_ptr<Self>( new Self() );
  //   }

  /**
  * Set default value of the numerical parameter 
  */
  virtual void SetDefaultValue( any_numeric val );

  /**
  * Get default value of the numerical parameter as an any_numeric 
  */
  virtual const any_numeric & GetDefaultValue() const
    {
    return m_Boundaries[1];
    }

  /**
  * Set minimum value of the numerical parameter 
  */
  virtual void SetMinimumValue( any_numeric val );

  /**
  * Get minimum value of the numerical parameter as an any_numeric
  */
  virtual const any_numeric & GetMinimumValue() const
    {
    return m_Boundaries[0];
    }

  /**
  * Set maximum value of the numerical parameter 
  */
  virtual void SetMaximumValue( any_numeric val );

  /**
  * Get maximum value of the numerical parameter as an any_numeric
  */
  virtual const any_numeric & GetMaximumValue() const
    {
    return m_Boundaries[2];
    }

  virtual double GetFloattingPointValue() const = 0 ;

  virtual int GetIntegerValue() const = 0 ;

  virtual any_numeric GetAnyNumericValue() const = 0 ;

  virtual ~NumericalParameterInterface() = default ;

protected:
  NumericalParameterInterface() ;

  void CheckInvariant( any_numeric val , int n ){}

  std::array< any_numeric , 3 > m_Boundaries;

private:



};

} // end namespace WrapperExp
} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbWrapperExpNumericalParameterInterface.txx"
#endif

#endif