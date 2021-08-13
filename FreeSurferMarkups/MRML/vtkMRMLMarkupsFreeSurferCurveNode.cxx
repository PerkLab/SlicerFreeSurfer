/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#include "vtkMRMLMarkupsFreeSurferCurveNode.h"

// MRML includes
#include <vtkFreeSurferCurveGenerator.h>
#include "vtkCurveMeasurementsCalculator.h"
#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLMeasurementLength.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLUnitNode.h"
#include "vtkSlicerDijkstraGraphGeodesicPath.h"

// VTK includes
#include <vtkArrayCalculator.h>
#include <vtkAssignAttribute.h>
#include <vtkBoundingBox.h>
#include <vtkCallbackCommand.h>
#include <vtkCellLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkCommand.h>
#include <vtkCutter.h>
#include <vtkDoubleArray.h>
#include <vtkFrenetSerretFrame.h>
#include <vtkGeneralTransform.h>
#include <vtkGenericCell.h>
#include <vtkLine.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkOBBTree.h>
#include <vtkObjectFactory.h>
#include <vtkPassThroughFilter.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkStringArray.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>

// STD includes
#include <sstream>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsFreeSurferCurveNode);

//----------------------------------------------------------------------------
vtkMRMLMarkupsFreeSurferCurveNode::vtkMRMLMarkupsFreeSurferCurveNode()
{
  this->FreeSurferCurveGenerator = vtkSmartPointer<vtkFreeSurferCurveGenerator>::New();

  this->CurveGenerator = FreeSurferCurveGenerator;
  this->CurveGenerator->SetInputData(this->CurveInputPoly);
  this->CurveGenerator->SetCurveTypeToLinearSpline();
  this->CurveGenerator->SetNumberOfPointsPerInterpolatingSegment(1);
  this->CurveGenerator->SetSurfaceCostFunctionType(vtkSlicerDijkstraGraphGeodesicPath::COST_FUNCTION_TYPE_DISTANCE);
  this->CurveGenerator->AddObserver(vtkCommand::ModifiedEvent, this->MRMLCallbackCommand);

  this->CurvePolyToWorldTransformer->SetInputConnection(this->CurveGenerator->GetOutputPort());

  // Insert curve measurements calculator between curve generator and world transformer filters
  this->CurveMeasurementsCalculator->SetInputConnection(this->CurveGenerator->GetOutputPort());
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsFreeSurferCurveNode::~vtkMRMLMarkupsFreeSurferCurveNode() = default;

//----------------------------------------------------------------------------
void vtkMRMLMarkupsFreeSurferCurveNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of,nIndent);

  vtkMRMLWriteXMLBeginMacro(of);
  vtkMRMLWriteXMLFloatMacro(distanceWeight, DistanceWeight);
  vtkMRMLWriteXMLFloatMacro(curvatureWeight, CurvatureWeight);
  vtkMRMLWriteXMLFloatMacro(sulcalHeightWeight, SulcalHeightWeight);
  vtkMRMLWriteXMLFloatMacro(distanceCurvatureWeight, DistanceCurvatureWeight);
  vtkMRMLWriteXMLFloatMacro(distanceSulcalHeightWeight, DistanceSulcalHeightWeight);
  vtkMRMLWriteXMLFloatMacro(curvatureSulcalHeightWeight, CurvatureSulcalHeightWeight);
  vtkMRMLWriteXMLFloatMacro(distanceCurvatureSulcalHeightWeight, DistanceCurvatureSulcalHeightWeight);
  vtkMRMLWriteXMLFloatMacro(directionWeight, DirectionWeight);
  vtkMRMLWriteXMLFloatMacro(curvaturePenalty, CurvaturePenalty);
  vtkMRMLWriteXMLFloatMacro(sulcalHeightPenalty, SulcalHeightPenalty);
  vtkMRMLWriteXMLFloatMacro(distanceCurvaturePenalty, DistanceCurvaturePenalty);
  vtkMRMLWriteXMLFloatMacro(distanceSulcalHeightPenalty, DistanceSulcalHeightPenalty);
  vtkMRMLWriteXMLFloatMacro(curvatureSulcalHeightPenalty, CurvatureSulcalHeightPenalty);
  vtkMRMLWriteXMLFloatMacro(distanceCurvatureSulcalHeightPenalty, DistanceCurvatureSulcalHeightPenalty);
  vtkMRMLWriteXMLBooleanMacro(invertScalars, InvertScalars);
  vtkMRMLWriteXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsFreeSurferCurveNode::ReadXMLAttributes(const char** atts)
{
  MRMLNodeModifyBlocker blocker(this);

  this->Superclass::ReadXMLAttributes(atts);

  vtkMRMLReadXMLBeginMacro(atts);
  vtkMRMLReadXMLFloatMacro(distanceWeight, DistanceWeight);
  vtkMRMLReadXMLFloatMacro(curvatureWeight, CurvatureWeight);
  vtkMRMLReadXMLFloatMacro(sulcalHeightWeight, SulcalHeightWeight);
  vtkMRMLReadXMLFloatMacro(distanceCurvatureWeight, DistanceCurvatureWeight);
  vtkMRMLReadXMLFloatMacro(distanceSulcalHeightWeight, DistanceSulcalHeightWeight);
  vtkMRMLReadXMLFloatMacro(curvatureSulcalHeightWeight, CurvatureSulcalHeightWeight);
  vtkMRMLReadXMLFloatMacro(distanceCurvatureSulcalHeightWeight, DistanceCurvatureSulcalHeightWeight);
  vtkMRMLReadXMLFloatMacro(directionWeight, DirectionWeight);
  vtkMRMLReadXMLFloatMacro(curvaturePenalty, CurvaturePenalty);
  vtkMRMLReadXMLFloatMacro(sulcalHeightPenalty, SulcalHeightPenalty);
  vtkMRMLReadXMLFloatMacro(distanceCurvaturePenalty, DistanceCurvaturePenalty);
  vtkMRMLReadXMLFloatMacro(distanceSulcalHeightPenalty, DistanceSulcalHeightPenalty);
  vtkMRMLReadXMLFloatMacro(curvatureSulcalHeightPenalty, CurvatureSulcalHeightPenalty);
  vtkMRMLReadXMLFloatMacro(distanceCurvatureSulcalHeightPenalty, DistanceCurvatureSulcalHeightPenalty);
  vtkMRMLReadXMLBooleanMacro(invertScalars, InvertScalars);
  vtkMRMLReadXMLEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsFreeSurferCurveNode::CopyContent(vtkMRMLNode* anode, bool deepCopy/*=true*/)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::CopyContent(anode, deepCopy);

  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyFloatMacro(DistanceWeight);
  vtkMRMLCopyFloatMacro(CurvatureWeight);
  vtkMRMLCopyFloatMacro(SulcalHeightWeight);
  vtkMRMLCopyFloatMacro(DistanceCurvatureWeight);
  vtkMRMLCopyFloatMacro(DistanceSulcalHeightWeight);
  vtkMRMLCopyFloatMacro(CurvatureSulcalHeightWeight);
  vtkMRMLCopyFloatMacro(DistanceCurvatureSulcalHeightWeight);
  vtkMRMLCopyFloatMacro(DirectionWeight);
  vtkMRMLCopyFloatMacro(CurvaturePenalty);
  vtkMRMLCopyFloatMacro(SulcalHeightPenalty);
  vtkMRMLCopyFloatMacro(DistanceCurvaturePenalty);
  vtkMRMLCopyFloatMacro(DistanceSulcalHeightPenalty);
  vtkMRMLCopyFloatMacro(CurvatureSulcalHeightPenalty);
  vtkMRMLCopyFloatMacro(DistanceCurvatureSulcalHeightPenalty);
  vtkMRMLCopyBooleanMacro(InvertScalars);
  vtkMRMLCopyEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsFreeSurferCurveNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

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
#define FreeSurferCurveGeneratorPropertyMacro(propertyName, type)\
void vtkMRMLMarkupsFreeSurferCurveNode::Set##propertyName(type value)\
{\
  if (value == this->Get##propertyName())\
  {\
    return;\
  }\
  this->FreeSurferCurveGenerator->Set##propertyName(value);\
  this->Modified();\
}\
type vtkMRMLMarkupsFreeSurferCurveNode::Get##propertyName()\
{\
  return this->FreeSurferCurveGenerator->Get##propertyName();\
}

//------------------------------------------------------------------------------
FreeSurferCurveGeneratorPropertyMacro(DistanceWeight, double);
FreeSurferCurveGeneratorPropertyMacro(CurvatureWeight, double);
FreeSurferCurveGeneratorPropertyMacro(SulcalHeightWeight, double);
FreeSurferCurveGeneratorPropertyMacro(DistanceCurvatureWeight, double);
FreeSurferCurveGeneratorPropertyMacro(DistanceSulcalHeightWeight, double);
FreeSurferCurveGeneratorPropertyMacro(CurvatureSulcalHeightWeight, double);
FreeSurferCurveGeneratorPropertyMacro(DistanceCurvatureSulcalHeightWeight, double);
FreeSurferCurveGeneratorPropertyMacro(DirectionWeight, double);

//------------------------------------------------------------------------------
FreeSurferCurveGeneratorPropertyMacro(CurvaturePenalty, double);
FreeSurferCurveGeneratorPropertyMacro(SulcalHeightPenalty, double);
FreeSurferCurveGeneratorPropertyMacro(DistanceCurvaturePenalty, double);
FreeSurferCurveGeneratorPropertyMacro(DistanceSulcalHeightPenalty, double);
FreeSurferCurveGeneratorPropertyMacro(CurvatureSulcalHeightPenalty, double);
FreeSurferCurveGeneratorPropertyMacro(DistanceCurvatureSulcalHeightPenalty, double);

//------------------------------------------------------------------------------
FreeSurferCurveGeneratorPropertyMacro(InvertScalars, bool);
