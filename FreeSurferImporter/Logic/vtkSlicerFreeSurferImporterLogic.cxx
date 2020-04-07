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
#include "vtkSlicerFreeSurferImporterLogic.h"

// MRML includes
#include <vtkMRMLFreeSurferModelStorageNode.h>
#include <vtkMRMLFreeSurferModelOverlayStorageNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSegmentationNode.h>
#include <vtkMRMLSegmentationStorageNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtksys/SystemTools.hxx>

// STD includes
#include <cassert>
#include <regex>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFreeSurferImporterLogic);

//----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic::vtkSlicerFreeSurferImporterLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic::~vtkSlicerFreeSurferImporterLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//-----------------------------------------------------------------------------
vtkMRMLVolumeNode* vtkSlicerFreeSurferImporterLogic::LoadFreeSurferVolume(std::string filePath)
{
  std::string name = vtksys::SystemTools::GetFilenameWithoutExtension(filePath);
  vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLScalarVolumeNode"));
  volumeNode->SetName(name.c_str());
  volumeNode->AddDefaultStorageNode(filePath.c_str());

  vtkMRMLVolumeArchetypeStorageNode* volumeStorageNode = vtkMRMLVolumeArchetypeStorageNode::SafeDownCast(volumeNode->GetStorageNode());
  if (volumeStorageNode->ReadData(volumeNode))
  {
    volumeNode->CreateDefaultDisplayNodes();
    return volumeNode;
  }

  this->GetMRMLScene()->RemoveNode(volumeStorageNode);
  this->GetMRMLScene()->RemoveNode(volumeNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLSegmentationNode* vtkSlicerFreeSurferImporterLogic::LoadFreeSurferSegmentation(std::string filePath)
{
  std::string name = vtksys::SystemTools::GetFilenameWithoutExtension(filePath);
  vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLSegmentationNode"));
  if (!segmentationNode)
  {
    return nullptr;
  }
  segmentationNode->SetName(name.c_str());
  segmentationNode->AddDefaultStorageNode(filePath.c_str());

  vtkMRMLSegmentationStorageNode* segmentationStorageNode = vtkMRMLSegmentationStorageNode::SafeDownCast(segmentationNode->GetStorageNode());
  if (segmentationStorageNode && segmentationStorageNode->ReadData(segmentationNode))
  {
    this->ApplyFreeSurferSegmentationLUT(segmentationNode);
    return segmentationNode;
  }

  this->GetMRMLScene()->RemoveNode(segmentationStorageNode);
  this->GetMRMLScene()->RemoveNode(segmentationNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLModelNode* vtkSlicerFreeSurferImporterLogic::LoadFreeSurferModel(std::string filePath)
{
  std::string name = vtksys::SystemTools::GetFilenameWithoutExtension(filePath);
  vtkMRMLModelNode* surfNode = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLModelNode"));
  if (!surfNode)
  {
    return nullptr;
  }
  surfNode->SetName(name.c_str());

  vtkMRMLFreeSurferModelStorageNode* surfStorageNode = vtkMRMLFreeSurferModelStorageNode::SafeDownCast(
    this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLFreeSurferModelStorageNode"));
  if (surfStorageNode)
  {
    surfNode->SetAndObserveStorageNodeID(surfStorageNode->GetID());
    surfStorageNode->SetFileName(filePath.c_str());
    if (surfStorageNode->ReadData(surfNode))
    {
      return surfNode;
    }
  }

  this->GetMRMLScene()->RemoveNode(surfStorageNode);
  this->GetMRMLScene()->RemoveNode(surfNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
bool vtkSlicerFreeSurferImporterLogic::LoadFreeSurferScalarOverlay(std::string filePath, std::vector<vtkMRMLModelNode*> modelNodes)
{
  std::string name = vtksys::SystemTools::GetFilenameWithoutExtension(filePath);
  vtkMRMLFreeSurferModelOverlayStorageNode* overlayStorageNode = vtkMRMLFreeSurferModelOverlayStorageNode::SafeDownCast(
    this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLFreeSurferModelOverlayStorageNode"));
  if (!overlayStorageNode)
  {
    return false;
  }

  overlayStorageNode->SetFileName(filePath.c_str());

  bool success = true;
  int numberOfOverlayLoaded = 0;
  for (vtkMRMLModelNode* modelNode : modelNodes)
  {
    if (!modelNode->GetName())
    {
      continue;
    }

    std::string modelNodeHemisphere = vtksys::SystemTools::GetFilenameWithoutExtension(modelNode->GetName());
    if (modelNodeHemisphere != name)
    {
      continue;
    }

    // Scalar overlay is already loaded for this model
    if (modelNode->HasCellScalarName(name.c_str()))
    {
      continue;
    }

    if (!overlayStorageNode->ReadData(modelNode))
    {
      success = false;
      continue;
    }
    numberOfOverlayLoaded += 1;
  }

  this->GetMRMLScene()->RemoveNode(overlayStorageNode);
  if (numberOfOverlayLoaded == 0)
  {
    success = false;
  }
  return success;
}

//-----------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::TransformFreeSurferModelToWorld(vtkMRMLModelNode* modelNode, vtkMRMLVolumeNode* referenceVolumeNode)
{
  if (!modelNode || !referenceVolumeNode || !this->GetMRMLScene())
  {
    vtkErrorMacro("Invalid input arugments!");
    return;
  }

  int extent[6] = { 0 };
  referenceVolumeNode->GetImageData()->GetExtent(extent);

  int dimensions[3] = { 0 };
  referenceVolumeNode->GetImageData()->GetDimensions(dimensions);

  double center[4] = { 0, 0, 0, 1 };
  for (int i = 0; i < 3; ++i)
  {
    center[i] = extent[2 * i] + std::ceil((dimensions[i] / 2.0));
  }

  vtkNew<vtkMatrix4x4> ijkToRAS;
  referenceVolumeNode->GetIJKToRASMatrix(ijkToRAS);
  ijkToRAS->MultiplyPoint(center, center);

  std::string transformName = "FSModel_";
  if (referenceVolumeNode->GetName())
  {
    transformName += referenceVolumeNode->GetName();
  }
  transformName += "ToWorld";

  vtkNew<vtkTransform> modelToWorldTransform;
  modelToWorldTransform->Translate(center);

  vtkMRMLLinearTransformNode* modelToWorldNode = vtkMRMLLinearTransformNode::SafeDownCast(
    this->GetMRMLScene()->GetFirstNodeByName(transformName.c_str()));
  if (modelToWorldNode)
  {
    vtkNew<vtkMatrix4x4> modelToWorld;
    modelToWorldNode->GetMatrixTransformToParent(modelToWorld);

    // Compare the current matrix with the one found in the scene.
    // If they are identical, then use that matrix.
    bool matrixExists = true;
    for (int j = 0; j < 4; ++j)
    {
      for (int i = 0; i < 4; ++i)
      {
        double epsilon = 0.00001;
        if (std::abs(modelToWorld->GetElement(i, j) - modelToWorldTransform->GetMatrix()->GetElement(i, j)) > epsilon)
        {
          matrixExists = false;
          break;
        }
      }
      if (!matrixExists)
      {
        break;
      }
    }
    if (!matrixExists)
    {
      modelToWorldNode = nullptr;
    }
  }

  if (!modelToWorldNode)
  {
    modelToWorldNode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLLinearTransformNode"));
    modelToWorldNode->SetName(transformName.c_str());
  }
  if (!modelToWorldNode)
  {
    vtkErrorMacro("Could not create transform node!");
    return;
  }
  modelToWorldNode->SetMatrixTransformToParent(modelToWorldTransform->GetMatrix());
  if (referenceVolumeNode->GetParentTransformNode())
  {
    modelToWorldNode->SetAndObserveTransformNodeID(referenceVolumeNode->GetParentTransformNode()->GetID());
  }
  modelNode->SetAndObserveTransformNodeID(modelToWorldNode->GetID());
}

//-----------------------------------------------------------------------------
struct SegmentInfo
{
  std::string name = "Unknown";
  double color[3] = { 0.5 };
};

//-----------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::ApplyFreeSurferSegmentationLUT(vtkMRMLSegmentationNode* segmentationNode)
{
  if (!segmentationNode)
  {
    return;
  }
  MRMLNodeModifyBlocker blocker(segmentationNode);

  std::string sharedDirectory = this->GetModuleShareDirectory();
  std::string lutFilename = "FreeSurferColorLUT.txt";

  std::string lutDirectory = sharedDirectory + "/" + lutFilename;

  std::ifstream lutFile;
  lutFile.open(lutDirectory);
  if (!lutFile.is_open())
  {
    return;
  }

  std::map<int, SegmentInfo> segmentInfoMap;

  std::string line;
  while (std::getline(lutFile, line))
  {
    line = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");
    if (line.empty())
    {
      continue;
    }
    if (line[0] == '#')
    {
      continue;
    }

    std::vector<std::string> tokens;
    std::stringstream ss(line);;
    std::string token;
    while (std::getline(ss, token, ' '))
    {
      tokens.push_back(token);
    }
    if (tokens.size() != 6)
    {
      continue;
    }

    int value = vtkVariant(tokens[0]).ToInt();
    SegmentInfo info;
    info.name = tokens[1];
    info.color[0] = vtkVariant(tokens[2]).ToInt() / 255.0;
    info.color[1] = vtkVariant(tokens[3]).ToInt() / 255.0;
    info.color[2] = vtkVariant(tokens[4]).ToInt() / 255.0;
    segmentInfoMap[value] = info;
  }
  lutFile.close();

  vtkSegmentation* segmentation = segmentationNode->GetSegmentation();
  for (int i = 0; i < segmentation->GetNumberOfSegments(); ++i)
  {
    vtkSegment* segment = segmentation->GetNthSegment(i);
    SegmentInfo info = segmentInfoMap[segment->GetLabelValue()];
    segment->SetName(info.name.c_str());
    segment->SetColor(info.color);
  }
}
