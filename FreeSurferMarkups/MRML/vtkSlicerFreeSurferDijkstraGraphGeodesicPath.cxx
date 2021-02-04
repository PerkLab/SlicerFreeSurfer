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
  Care Ontario, OpenAnatomy, and Brigham and Women’s Hospital through NIH grant R01MH112748.

==============================================================================*/

// Markups MRML includes
#include "vtkSlicerFreeSurferDijkstraGraphGeodesicPath.h"

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFreeSurferDijkstraGraphGeodesicPath);

//------------------------------------------------------------------------------
vtkSlicerFreeSurferDijkstraGraphGeodesicPath::vtkSlicerFreeSurferDijkstraGraphGeodesicPath()
{
  this->DistanceWeight = 1.0;
  this->CurvatureWeight = 1.0;
  this->SulcalHeightWeight = 1.0;
  this->DistanceCurvatureWeight = 1.0;
  this->DistanceSulcalHeightWeight = 1.0;
  this->CurvatureSulcalHeightWeight = 1.0;
  this->DistanceCurvatureSulcalHeightWeight = 1.0;
  this->DirectionWeight = 1.0;

  this->CurvaturePenalty = 10.0;
  this->SulcalHeightPenalty = 10.0;
  this->DistanceCurvaturePenalty = 10.0;
  this->DistanceSulcalHeightPenalty = 10.0;
  this->CurvatureSulcalHeightPenalty = 10.0;
  this->DistanceCurvatureSulcalHeightPenalty = 1.0;
}

//------------------------------------------------------------------------------
vtkSlicerFreeSurferDijkstraGraphGeodesicPath::~vtkSlicerFreeSurferDijkstraGraphGeodesicPath() = default;

//------------------------------------------------------------------------------
void vtkSlicerFreeSurferDijkstraGraphGeodesicPath::PrintSelf(std::ostream & os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
double vtkSlicerFreeSurferDijkstraGraphGeodesicPath::CalculateStaticEdgeCost(
  vtkDataSet * vtkNotUsed(inData), vtkIdType vtkNotUsed(u), vtkIdType vtkNotUsed(v))
{
  return 0.0;
}

//------------------------------------------------------------------------------
double vtkSlicerFreeSurferDijkstraGraphGeodesicPath::CalculateDynamicEdgeCost(
  vtkDataSet * inData, vtkIdType u, vtkIdType v)
{
  /// Based on the FreeSurfer implementation found here:
  /// https://github.com/freesurfer/freesurfer/blob/4db941ef298c0ac5fb78c29fd0e95571ac363e16/mris_pmake/env.cpp#L2017

  if (!inData)
  {
    return 0.0;
  }

  double currentPoint[3];
  inData->GetPoint(u, currentPoint);
  double neighbourPoint[3];
  inData->GetPoint(v, neighbourPoint);

  double distance = sqrt(vtkMath::Distance2BetweenPoints(currentPoint, neighbourPoint)); //wd

  double curvature = 0.0;
  float curvatureRange[2] = { 0.0, 0.0 };

  double sulcalHeight = 0.0;
  float sulcalHeightRange[2] = { 0.0, 0.0 };

  vtkPointData* pointData = inData->GetPointData();
  if (pointData)
  {
    vtkFloatArray* curvArray = vtkFloatArray::SafeDownCast(pointData->GetArray("curv"));
    if (curvArray)
    {
      curvArray->GetValueRange(curvatureRange);
      curvature = curvArray->GetValue(v);
    }

    vtkFloatArray* sulcArray = vtkFloatArray::SafeDownCast(pointData->GetArray("sulc"));
    if (sulcArray)
    {
      sulcArray->GetValueRange(sulcalHeightRange);
      sulcalHeight = sulcArray->GetValue(v);
    }
  }

  double edgeDirection[3] = { 0.0 };
  vtkMath::Subtract(neighbourPoint, currentPoint, edgeDirection);
  vtkMath::Normalize(edgeDirection);

  double endVertex[3] = { 0.0 };
  inData->GetPoint(this->EndVertex, endVertex);

  double directionToEnd[3] = { 0.0 };
  vtkMath::Subtract(endVertex, currentPoint, directionToEnd);
  vtkMath::Normalize(directionToEnd);

  double direction = 1.0 - vtkMath::Dot(edgeDirection, directionToEnd);

  double distanceWeight = this->DistanceWeight;
  double curvatureWeight = this->CurvatureWeight;
  double sulcalHeightWeight = this->SulcalHeightWeight;
  double distanceCurvatureWeight = this->DistanceCurvatureWeight;
  double distanceSulcalHeightWeight = this->DistanceSulcalHeightWeight;
  double curvatureSulcalHeightWeight = this->CurvatureSulcalHeightWeight;
  double distanceCurvatureSulcalHeightWeight = this->DistanceCurvatureSulcalHeightWeight;
  double directionWeight = this->DirectionWeight;

  if (curvature < 0)
  {
    curvatureWeight *= this ->CurvaturePenalty;
    distanceCurvatureWeight *= this->DistanceCurvaturePenalty;
    curvatureSulcalHeightWeight *= this->CurvatureSulcalHeightPenalty;
    distanceCurvatureSulcalHeightWeight *= this->DistanceCurvatureSulcalHeightPenalty;
  }

  if (sulcalHeight < 0)
  {
    sulcalHeightWeight *= this->SulcalHeightPenalty;
    distanceSulcalHeightWeight *= this->DistanceSulcalHeightPenalty;
    curvatureSulcalHeightWeight *= this->CurvatureSulcalHeightPenalty;
    distanceCurvatureSulcalHeightWeight *= this->DistanceCurvatureSulcalHeightPenalty;
  }

  // Set curvature and sulcal height to strictly positive ranges
  curvature = curvatureRange[1] - curvature;
  sulcalHeight = sulcalHeightRange[1] - sulcalHeight;

  double cost = 0.0;
  cost += distanceWeight * distance;
  cost += curvatureWeight * curvature;
  cost += sulcalHeightWeight * sulcalHeight;
  cost += distanceSulcalHeightWeight * distance * sulcalHeight;
  cost += distanceCurvatureWeight * distance * curvature;
  cost += curvatureSulcalHeightWeight * curvature * sulcalHeight;
  cost += distanceCurvatureSulcalHeightWeight * distance * curvature * sulcalHeight;
  cost += directionWeight * direction;

  return cost;
}



