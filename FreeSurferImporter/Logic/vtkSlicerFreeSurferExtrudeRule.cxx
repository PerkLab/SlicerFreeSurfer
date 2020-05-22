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

#include "vtkSlicerFreeSurferExtrudeRule.h"

#include "vtkMRMLDynamicModelerNode.h"

// MRML includes
#include <vtkMRMLMarkupsPlaneNode.h>
#include <vtkMRMLMarkupsClosedCurveNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipClosedSurface.h>
#include <vtkClipPolyData.h>
#include <vtkCollection.h>
#include <vtkCommand.h>
#include <vtkConnectivityFilter.h>
#include <vtkCutter.h>
#include <vtkDataSetAttributes.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkFeatureEdges.h>
#include <vtkGeneralTransform.h>
#include <vtkImplicitBoolean.h>
#include <vtkIntArray.h>
#include <vtkMergeCells.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkReverseSense.h>
#include <vtkSelectPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkStripper.h>
#include <vtkThreshold.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkPointData.h>
#include <vtkCellData.h>


#include <vtkPolyDataNormals.h>

//----------------------------------------------------------------------------
vtkRuleNewMacro(vtkSlicerFreeSurferExtrudeRule);

//----------------------------------------------------------------------------
vtkSlicerFreeSurferExtrudeRule::vtkSlicerFreeSurferExtrudeRule()
{
  /////////
  // Inputs
  vtkNew<vtkIntArray> inputModelEvents;
  inputModelEvents->InsertNextTuple1(vtkCommand::ModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLModelNode::MeshModifiedEvent);
  inputModelEvents->InsertNextTuple1(vtkMRMLTransformableNode::TransformModifiedEvent);
  vtkNew<vtkStringArray> inputModelClassNames;
  inputModelClassNames->InsertNextValue("vtkMRMLModelNode");

  NodeInfo inputPatch(
    "Patch model node",
    "Surface patch to be extruded.",
    inputModelClassNames,
    "FreeSurferExtrude.InputPatch",
    false,
    false,
    inputModelEvents
  );
  this->InputNodeInfo.push_back(inputPatch);

  /////////
  // Inputs
  NodeInfo inputModel0(
    "Orig model node",
    "",
    inputModelClassNames,
    "FreeSurferExtrude.InputOrigModel",
    false,
    false,
    inputModelEvents
    );
  this->InputNodeInfo.push_back(inputModel0);

  /////////
  // Inputs
  NodeInfo inputModel1(
    "Pial model node",
    "",
    inputModelClassNames,
    "FreeSurferExtrude.InputPialModel",
    false,
    false,
    inputModelEvents
  );
  this->InputNodeInfo.push_back(inputModel1);

  /////////
  // Outputs
  NodeInfo outputModel(
    "Model node",
    "Output model containing the cut region.",
    inputModelClassNames,
    "FreeSurferExtrude.OutputModel",
    false,
    false
    );
  this->OutputNodeInfo.push_back(outputModel);
}

//----------------------------------------------------------------------------
vtkSlicerFreeSurferExtrudeRule::~vtkSlicerFreeSurferExtrudeRule()
= default;

//----------------------------------------------------------------------------
const char* vtkSlicerFreeSurferExtrudeRule::GetName()
{
  return "FreeSurfer Extrude";
}

//----------------------------------------------------------------------------
bool vtkSlicerFreeSurferExtrudeRule::RunInternal(vtkMRMLDynamicModelerNode* surfaceEditorNode)
{
  if (!this->HasRequiredInputs(surfaceEditorNode))
    {
    vtkErrorMacro("Invalid number of inputs");
    return false;
    }

  vtkMRMLModelNode* outputModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthOutputNode(0, surfaceEditorNode));
  if (!outputModelNode)
    {
    // Nothing to output
    return true;
    }

  vtkMRMLModelNode* inputPatchModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthInputNode(0, surfaceEditorNode));
  if (!inputPatchModelNode || !inputPatchModelNode->GetMesh())
    {
    // Nothing to output
    return true;
    }

  vtkMRMLModelNode* inputOrigModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthInputNode(1, surfaceEditorNode));
  if (!inputOrigModelNode || !inputOrigModelNode->GetMesh())
    {
    // Nothing to output
    return true;
    }

  vtkMRMLModelNode* inputPialModelNode = vtkMRMLModelNode::SafeDownCast(this->GetNthInputNode(2, surfaceEditorNode));
  if (!inputPialModelNode || !inputPialModelNode->GetMesh())
    {
    // Nothing to output
    return true;
    }
  
  vtkNew<vtkGeneralTransform> inputPatchTransform;
  if (inputPatchModelNode->GetParentTransformNode())
    {
    inputPatchModelNode->GetParentTransformNode()->GetTransformToWorld(inputPatchTransform);
    }
  vtkNew<vtkTransformPolyDataFilter> inputPatchTransformFilter;
  inputPatchTransformFilter->SetInputData(inputPatchModelNode->GetMesh());
  inputPatchTransformFilter->SetTransform(inputPatchTransform);

  vtkNew<vtkFeatureEdges> featureEdgesFilter;
  featureEdgesFilter->SetInputConnection(inputPatchTransformFilter->GetOutputPort());
  featureEdgesFilter->BoundaryEdgesOn();
  featureEdgesFilter->FeatureEdgesOff();

  vtkNew<vtkStripper> stripperFilter;
  stripperFilter->SetInputConnection(featureEdgesFilter->GetOutputPort());
  stripperFilter->Update();  

  vtkNew<vtkGeneralTransform> inputOrigTransform;
  if (inputOrigModelNode->GetParentTransformNode())
    {
    inputOrigModelNode->GetParentTransformNode()->GetTransformToWorld(inputOrigTransform);
    }
  vtkNew<vtkTransformPolyDataFilter> inputOrigTransformFilter;
  inputOrigTransformFilter->SetInputData(inputOrigModelNode->GetMesh());
  inputOrigTransformFilter->SetTransform(inputOrigTransform);
  inputOrigTransformFilter->Update();

  vtkNew<vtkGeneralTransform> inputPialTransform;
  if (inputPialModelNode->GetParentTransformNode())
    {
    inputPialModelNode->GetParentTransformNode()->GetTransformToWorld(inputPialTransform);
    }
  vtkNew<vtkTransformPolyDataFilter> inputPialTransformFilter;
  inputPialTransformFilter->SetInputData(inputPialModelNode->GetMesh());
  inputPialTransformFilter->SetTransform(inputOrigTransform);
  inputPialTransformFilter->Update();

  vtkNew<vtkPointLocator> locator;
  locator->SetDataSet(inputOrigTransformFilter->GetOutput());
  locator->BuildLocator();

  vtkNew<vtkIdList> loopIds;
  vtkNew<vtkPoints> loopPoints;
  vtkCellArray* lines = stripperFilter->GetOutput()->GetLines();
  for (int i = 0; i < lines->GetNumberOfCells(); ++i)
    {
    vtkNew<vtkIdList> inputIds;
    lines->GetCell(i, inputIds);
    vtkNew<vtkIdList> outputIds;
    for (int j = 0; j < inputIds->GetNumberOfIds(); ++j)
      {
      vtkIdType id = inputIds->GetId(j);
      double point[3] = { 0.0 };
      stripperFilter->GetOutput()->GetPoint(id, point);

      vtkIdType outputId = locator->FindClosestPoint(point);
      inputPialTransformFilter->GetOutput()->GetPoint(outputId, point);
      loopPoints->InsertNextPoint(point);
      loopIds->InsertNextId(outputId);
      }
    }

  vtkNew<vtkPoints> spanPoints;
  vtkNew<vtkCellArray> spanPolys;
  for (int i = 0; i < loopIds->GetNumberOfIds() - 1; ++i)
    {
    vtkIdType id0 = loopIds->GetId(i);
    vtkIdType id1 = loopIds->GetId(i+1);

    double origPoint0[3] = { 0.0 };
    inputOrigTransformFilter->GetOutput()->GetPoint(id0, origPoint0);

    double origPoint1[3] = { 0.0 };
    inputOrigTransformFilter->GetOutput()->GetPoint(id1, origPoint1);

    double pialPoint0[3] = { 0.0 };
    inputPialTransformFilter->GetOutput()->GetPoint(id0, pialPoint0);

    double pialPoint1[3] = { 0.0 };
    inputPialTransformFilter->GetOutput()->GetPoint(id1, pialPoint1);

    vtkIdType origId0 = spanPoints->InsertNextPoint(origPoint0);
    vtkIdType origId1 = spanPoints->InsertNextPoint(origPoint1);
    vtkIdType pialId0 = spanPoints->InsertNextPoint(pialPoint0);
    vtkIdType pialId1 = spanPoints->InsertNextPoint(pialPoint1);

    vtkNew<vtkIdList> triangle0;
    triangle0->InsertNextId(origId0);
    triangle0->InsertNextId(origId1);
    triangle0->InsertNextId(pialId0);
    spanPolys->InsertNextCell(triangle0);

    vtkNew<vtkIdList> triangle1;
    triangle1->InsertNextId(pialId0);
    triangle1->InsertNextId(origId1);
    triangle1->InsertNextId(pialId1);
    spanPolys->InsertNextCell(triangle1);
    }

  vtkNew<vtkPolyData> spanPolyData;
  spanPolyData->SetPoints(spanPoints);
  spanPolyData->SetPolys(spanPolys);

  vtkNew<vtkSelectPolyData> selectionFilter;
  selectionFilter->SetInputData(inputPialTransformFilter->GetOutput());
  selectionFilter->GenerateSelectionScalarsOn();
  selectionFilter->SetSelectionModeToSmallestRegion();
  selectionFilter->SetLoop(loopPoints);

  vtkNew<vtkClipPolyData> clipFilter;
  clipFilter->SetInputConnection(selectionFilter->GetOutputPort());
  clipFilter->InsideOutOn();
  clipFilter->Update();
  
  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputData(inputPatchTransformFilter->GetOutput());
  normals->FlipNormalsOn();
  normals->Update();

  vtkNew<vtkAppendPolyData> appendFilter;
  appendFilter->AddInputData(spanPolyData);
  appendFilter->AddInputData(normals->GetOutput());
  appendFilter->AddInputData(clipFilter->GetOutput());

  vtkNew<vtkCleanPolyData> cleanPolyData;
  cleanPolyData->SetInputConnection(appendFilter->GetOutputPort());
  cleanPolyData->Update();
  
  vtkNew<vtkPolyData> outputPolyData;
  outputPolyData->DeepCopy(cleanPolyData->GetOutput());
  outputModelNode->SetAndObservePolyData(outputPolyData);

  return true;
}
