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

// FreeSurferImporter Logic includes
#include "vtkSlicerFreeSurferMarkupsLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>

// Slicer includes
#include <vtkMRMLColorLogic.h>

// FreeSurfer Markups MRML includes
#include <vtkMRMLMarkupsFreeSurferCurveNode.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFreeSurferMarkupsLogic);

//----------------------------------------------------------------------------
vtkSlicerFreeSurferMarkupsLogic::vtkSlicerFreeSurferMarkupsLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerFreeSurferMarkupsLogic::~vtkSlicerFreeSurferMarkupsLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferMarkupsLogic::ObserveMRMLScene()
{
  vtkMRMLApplicationLogic* mrmlAppLogic = this->GetMRMLApplicationLogic();
  vtkMRMLSelectionNode* selectionNode = mrmlAppLogic->GetSelectionNode();
  if (selectionNode)
  {
    vtkNew<vtkMRMLMarkupsFreeSurferCurveNode> freeSurferCurve;
    selectionNode->AddNewPlaceNodeClassNameToList(
      freeSurferCurve->GetClassName(),
      ":/Icons/MarkupsCurveMouseModePlace.png",
      "FreeSurfer curve");
  }
}