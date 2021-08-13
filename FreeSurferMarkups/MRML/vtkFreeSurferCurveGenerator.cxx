/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kyle Sunderland, PerkLab, Queen's University
  and was supported through CANARIE's Research Software Program, Cancer
  Care Ontario, OpenAnatomy, and Brigham and Women's Hospital through NIH grant R01MH112748.

==============================================================================*/

// FreeSurfer includes
#include "vtkFreeSurferCurveGenerator.h"

#include <vtkSlicerFreeSurferDijkstraGraphGeodesicPath.h>

#include <vtkMRMLNode.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkFreeSurferCurveGenerator);

//------------------------------------------------------------------------------
vtkFreeSurferCurveGenerator::vtkFreeSurferCurveGenerator()
{
  this->FreeSurferSurfacePathFilter = vtkSmartPointer<vtkSlicerFreeSurferDijkstraGraphGeodesicPath>::New();
  this->SurfacePathFilter = this->FreeSurferSurfacePathFilter;
  this->SurfacePathFilter->StopWhenEndReachedOn();
}

//------------------------------------------------------------------------------
vtkFreeSurferCurveGenerator::~vtkFreeSurferCurveGenerator() = default;

//------------------------------------------------------------------------------
void vtkFreeSurferCurveGenerator::PrintSelf(std::ostream & os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintFloatMacro(DistanceWeight);
  vtkMRMLPrintFloatMacro(CurvatureWeight);
  vtkMRMLPrintFloatMacro(SulcalHeightWeight);
  vtkMRMLPrintFloatMacro(DistanceCurvatureWeight);
  vtkMRMLPrintFloatMacro(DistanceSulcalHeightWeight);
  vtkMRMLPrintFloatMacro(CurvatureSulcalHeightWeight);
  vtkMRMLPrintFloatMacro(DistanceCurvatureSulcalHeightWeight);
  vtkMRMLPrintFloatMacro(DirectionWeight);
  vtkMRMLPrintFloatMacro(CurvaturePenalty);
  vtkMRMLPrintFloatMacro(SulcalHeightPenalty);
  vtkMRMLPrintFloatMacro(DistanceCurvaturePenalty);
  vtkMRMLPrintFloatMacro(DistanceSulcalHeightPenalty);
  vtkMRMLPrintFloatMacro(CurvatureSulcalHeightPenalty);
  vtkMRMLPrintFloatMacro(DistanceCurvatureSulcalHeightPenalty);
  vtkMRMLPrintBooleanMacro(InvertScalars);
  vtkMRMLPrintEndMacro();
}

//------------------------------------------------------------------------------
#define FreeSurferPathFilterPropertyMacro(propertyName, type)\
void vtkFreeSurferCurveGenerator::Set##propertyName(type value)\
{\
  if (value == this->Get##propertyName())\
  {\
    return;\
  }\
  this->FreeSurferSurfacePathFilter->Set##propertyName(value);\
  this->Modified();\
}\
type vtkFreeSurferCurveGenerator::Get##propertyName()\
{\
  return this->FreeSurferSurfacePathFilter->Get##propertyName();\
}

//------------------------------------------------------------------------------
FreeSurferPathFilterPropertyMacro(DistanceWeight, double);
FreeSurferPathFilterPropertyMacro(CurvatureWeight, double);
FreeSurferPathFilterPropertyMacro(SulcalHeightWeight, double);
FreeSurferPathFilterPropertyMacro(DistanceCurvatureWeight, double);
FreeSurferPathFilterPropertyMacro(DistanceSulcalHeightWeight, double);
FreeSurferPathFilterPropertyMacro(CurvatureSulcalHeightWeight, double);
FreeSurferPathFilterPropertyMacro(DistanceCurvatureSulcalHeightWeight, double);
FreeSurferPathFilterPropertyMacro(DirectionWeight, double);

//------------------------------------------------------------------------------
FreeSurferPathFilterPropertyMacro(CurvaturePenalty, double);
FreeSurferPathFilterPropertyMacro(SulcalHeightPenalty, double);
FreeSurferPathFilterPropertyMacro(DistanceCurvaturePenalty, double);
FreeSurferPathFilterPropertyMacro(DistanceSulcalHeightPenalty, double);
FreeSurferPathFilterPropertyMacro(CurvatureSulcalHeightPenalty, double);
FreeSurferPathFilterPropertyMacro(DistanceCurvatureSulcalHeightPenalty, double);

//------------------------------------------------------------------------------
FreeSurferPathFilterPropertyMacro(InvertScalars, bool);