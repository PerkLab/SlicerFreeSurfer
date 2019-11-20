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

// FSImporter Logic includes
#include "vtkSlicerFSImporterLogic.h"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSegmentationNode.h>
#include <vtkMRMLSegmentationStorageNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// STD includes
#include <cassert>
#include <regex>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerFSImporterLogic);

//----------------------------------------------------------------------------
vtkSlicerFSImporterLogic::vtkSlicerFSImporterLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerFSImporterLogic::~vtkSlicerFSImporterLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerFSImporterLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerFSImporterLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerFSImporterLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFSImporterLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerFSImporterLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerFSImporterLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//-----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkSlicerFSImporterLogic::loadFreeSurferVolume(std::string fsDirectory, std::string name)
{
  std::string volumeFile = fsDirectory + name;
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLScalarVolumeNode"));
  volumeNode->SetName(name.c_str());
  volumeNode->AddDefaultStorageNode(volumeFile.c_str());
  
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
vtkMRMLSegmentationNode* vtkSlicerFSImporterLogic::loadFreeSurferSegmentation(std::string fsDirectory, std::string name)
{
  std::string segmentationFile = fsDirectory + name;
  vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLSegmentationNode"));
  segmentationNode->SetName(name.c_str());
  segmentationNode->AddDefaultStorageNode(segmentationFile.c_str());

  vtkMRMLSegmentationStorageNode* segmentationStorageNode = vtkMRMLSegmentationStorageNode::SafeDownCast(segmentationNode->GetStorageNode());
  if (segmentationStorageNode->ReadData(segmentationNode))
    {
    this->applyFreeSurferSegmentationLUT(segmentationNode);
    return segmentationNode;
    }

  this->GetMRMLScene()->RemoveNode(segmentationStorageNode);
  this->GetMRMLScene()->RemoveNode(segmentationNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLModelNode* vtkSlicerFSImporterLogic::loadFreeSurferModel(std::string fsDirectory, std::string name)
{
  std::string surfFile = fsDirectory + name;
  vtkMRMLModelNode* surfNode = vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLModelNode"));
  surfNode->SetName(name.c_str());
  surfNode->AddDefaultStorageNode(surfFile.c_str());

  vtkMRMLModelStorageNode* surfStorageNode = vtkMRMLModelStorageNode::SafeDownCast(surfNode->GetStorageNode());
  if (surfStorageNode->ReadData(surfNode))
    {
    return surfNode;
    }

  this->GetMRMLScene()->RemoveNode(surfStorageNode);
  this->GetMRMLScene()->RemoveNode(surfNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
void vtkSlicerFSImporterLogic::transformFreeSurferModelToRAS(vtkMRMLModelNode* modelNode, vtkMRMLScalarVolumeNode* origVolumeNode)
{
  if (!modelNode || !origVolumeNode)
    {
    return;
    }

  int extent[6] = { 0 };
  origVolumeNode->GetImageData()->GetExtent(extent);

  int dimensions[3] = { 0 };
  origVolumeNode->GetImageData()->GetDimensions(dimensions);

  double center[4] = { 0, 0, 0, 1 };
  for (int i = 0; i < 3; ++i)
    {
    center[i] = extent[2 * i] + std::ceil((dimensions[i] / 2.0));
    }

  vtkNew<vtkMatrix4x4> ijkToRAS;
  origVolumeNode->GetIJKToRASMatrix(ijkToRAS);
  ijkToRAS->MultiplyPoint(center, center); 

  vtkNew<vtkTransform> transform;
  transform->Translate(center);

  vtkNew<vtkTransformPolyDataFilter> transformer;
  transformer->SetTransform(transform);
  transformer->SetInputData(modelNode->GetPolyData());
  transformer->Update();
  modelNode->GetPolyData()->ShallowCopy(transformer->GetOutput());
  modelNode->GetPolyData()->Modified();
}

//-----------------------------------------------------------------------------
struct SegmentInfo
{
  std::string name = "Unknown";
  double color[3] = { 0.5 };
};

//-----------------------------------------------------------------------------
void vtkSlicerFSImporterLogic::applyFreeSurferSegmentationLUT(vtkMRMLSegmentationNode* segmentationNode)
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
