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
#include "vtkSlicerFreeSurferExtrudeTool.h"

// MRML includes
#include <vtkMRMLColorTableStorageNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSegmentationNode.h>
#include <vtkMRMLSegmentationStorageNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkMRMLVolumeNode.h>

// Markups includes
#include <vtkCurveGenerator.h>
#include <vtkMRMLMarkupsCurveNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include <vtkMRMLMarkupsPlaneNode.h>

// Slicer includes
#include <vtkMRMLColorLogic.h>

// FreeSurfer MRML
#include <vtkMRMLFreeSurferModelOverlayStorageNode.h>
#include <vtkMRMLFreeSurferModelStorageNode.h>
#include <vtkMRMLFreeSurferProceduralColorNode.h>

// VTK includes
#include <vtkAssignAttribute.h>
#include <vtkCleanPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkSortDataArray.h>
#include <vtksys/SystemTools.hxx>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// vtkAddon includes
#include <vtkAddonMathUtilities.h>

// DynamicModeler includes
#include <vtkSlicerDynamicModelerToolFactory.h>

// FreeSurfer includes
#include <vtkFSSurfaceLabelReader.h>

// STD includes
#include <cassert>
#include <regex>

//----------------------------------------------------------------------------
const static std::string DEFAULT_FREESURFER_LABEL_FILENAME = "FreeSurferColorLUT20150729.txt";
std::string vtkSlicerFreeSurferImporterLogic::TempColorNodeID;

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

  if (newScene)
  {
    this->OnMRMLSceneNewEvent();
  }
}

//------------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::OnMRMLSceneNewEvent()
{
  // create the default color nodes, they don't get saved with the scenes as
  // they'll be created on start up, and when a new
  // scene is opened
  if (this->GetMRMLScene() == nullptr)
  {
    vtkWarningMacro("vtkMRMLColorLogic::AddDefaultColorNodes: no scene to which to add nodes\n");
    return;
  }

  // add freesurfer nodes
  this->AddFreeSurferNodes();
}

//-----------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  this->GetMRMLScene()->RegisterNodeClass(vtkSmartPointer<vtkMRMLFreeSurferModelOverlayStorageNode>::New());
  this->GetMRMLScene()->RegisterNodeClass(vtkSmartPointer<vtkMRMLFreeSurferModelStorageNode>::New());
  this->GetMRMLScene()->RegisterNodeClass(vtkSmartPointer<vtkMRMLFreeSurferProceduralColorNode>::New());

  // Allocate the singleton
  vtkSlicerDynamicModelerToolFactory* toolFactoryInstance = vtkSlicerDynamicModelerToolFactory::GetInstance();
  toolFactoryInstance->RegisterDynamicModelerTool(vtkSmartPointer<vtkSlicerFreeSurferExtrudeTool>::New());
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
  std::string extension = vtksys::SystemTools::GetFilenameExtension(filePath);
  extension.erase(0, 1);
  name += "_" + extension;

  vtkSmartPointer<vtkMRMLModelNode> surfNode = vtkSmartPointer<vtkMRMLModelNode>::Take(
    vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->CreateNodeByClass("vtkMRMLModelNode")));
  if (!surfNode)
  {
    return nullptr;
  }
  surfNode->SetName(name.c_str());

  vtkSmartPointer<vtkMRMLFreeSurferModelStorageNode> surfStorageNode = vtkSmartPointer<vtkMRMLFreeSurferModelStorageNode>::Take(vtkMRMLFreeSurferModelStorageNode::SafeDownCast(
    this->GetMRMLScene()->CreateNodeByClass("vtkMRMLFreeSurferModelStorageNode")));
  if (!surfStorageNode)
  {
    vtkErrorMacro("LoadFreeSurferModel: Could not add FreeSurfer storage node");
    return nullptr;
  }

  surfStorageNode->SetUseStripper(0);  // turn off stripping by default (breaks some pickers)
  if (surfStorageNode)
  {
    surfNode->SetAndObserveStorageNodeID(surfStorageNode->GetID());
    surfStorageNode->SetFileName(filePath.c_str());
    if (surfStorageNode->ReadData(surfNode))
    {
      this->GetMRMLScene()->AddNode(surfStorageNode);
      this->GetMRMLScene()->AddNode(surfNode);
      return surfNode;
    }
  }

  return nullptr;
}

//-----------------------------------------------------------------------------
bool vtkSlicerFreeSurferImporterLogic::LoadFreeSurferScalarOverlay(std::string filePath, vtkMRMLModelNode* modelNode)
{
  if (!modelNode)
  {
    return false;
  }

  vtkMRMLFreeSurferModelOverlayStorageNode* overlayStorageNode = vtkMRMLFreeSurferModelOverlayStorageNode::SafeDownCast(
    this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLFreeSurferModelOverlayStorageNode"));
  if (!overlayStorageNode)
  {
    vtkErrorMacro("LoadFreeSurferScalarOverlay: Could not add FreeSurfer overlay storage node");
    return false;
  }

  bool success = true;
  overlayStorageNode->SetFileName(filePath.c_str());
  if (!overlayStorageNode->ReadData(modelNode))
  {
    vtkErrorMacro("LoadFreeSurferScalarOverlay: Could not add FreeSurfer overlay");
    success = false;
  }

  this->GetMRMLScene()->RemoveNode(overlayStorageNode);
  return success;
}

//-----------------------------------------------------------------------------
bool vtkSlicerFreeSurferImporterLogic::LoadFreeSurferScalarOverlay(std::string filePath, vtkCollection* modelNodes)
{
  int numberOfOverlayLoaded = 0;
  for (int i = 0; i < modelNodes->GetNumberOfItems(); ++i)
  {
    vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(modelNodes->GetItemAsObject(i)); 
    if (!this->LoadFreeSurferScalarOverlay(filePath, modelNode))
    {
      continue;
    }
    numberOfOverlayLoaded += 1;
  }

  if (numberOfOverlayLoaded == 0)
  {
    return false;
  }
  return true;
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
  std::string lutDirectory = sharedDirectory + "/" + DEFAULT_FREESURFER_LABEL_FILENAME;

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

//-----------------------------------------------------------------------------
const char* vtkSlicerFreeSurferImporterLogic::GetFreeSurferColorNodeID(int type)
{
  vtkNew<vtkMRMLFreeSurferProceduralColorNode> basicNode;
  basicNode->SetType(type);
  std::string id = std::string(basicNode->GetClassName()) +
    std::string(basicNode->GetTypeAsString());
  vtkSlicerFreeSurferImporterLogic::TempColorNodeID = id;
  return vtkSlicerFreeSurferImporterLogic::TempColorNodeID.c_str();
}

//------------------------------------------------------------------------------
vtkMRMLFreeSurferProceduralColorNode* vtkSlicerFreeSurferImporterLogic::CreateFreeSurferNode(int type)
{
  vtkMRMLFreeSurferProceduralColorNode* node = vtkMRMLFreeSurferProceduralColorNode::New();
  vtkDebugMacro("vtkMRMLColorLogic::AddDefaultColorNodes: setting free surfer proc color node type to " << type);
  node->SetType(type);
  node->SetAttribute("Category", "FreeSurfer");
  node->SaveWithSceneOff();
  if (node->GetTypeAsString() == nullptr)
  {
    vtkWarningMacro("Node type as string is null");
    node->SetName("NoName");
  }
  else
  {
    node->SetName(node->GetTypeAsString());
  }
  vtkDebugMacro("vtkMRMLColorLogic::AddDefaultColorNodes: set proc node name to " << node->GetName());
  /*
  if (this->GetFreeSurferColorNodeID(i) == nullptr)
    {
    vtkDebugMacro("Error getting default node id for freesurfer node " << i);
    }
  */
  vtkDebugMacro("vtkMRMLColorLogic::AddDefaultColorNodes: Getting default fs color node id for type " << type);
  node->SetSingletonTag(node->GetTypeAsString());
  return node;
}

//------------------------------------------------------------------------------
vtkMRMLColorTableNode* vtkSlicerFreeSurferImporterLogic::CreateFreeSurferFileNode(const char* fileName)
{
  if (fileName == nullptr)
  {
    vtkErrorMacro("Unable to get the labels file name, not adding");
    return nullptr;
  }

  vtkMRMLColorTableNode* node = this->CreateFileNode(fileName);

  if (!node)
  {
    return nullptr;
  }
  node->SetAttribute("Category", "FreeSurfer");
  node->SetName("FreeSurferLabels");
  node->SetSingletonTag(node->GetTypeAsString());

  return node;
}

//--------------------------------------------------------------------------------
vtkMRMLColorTableNode* vtkSlicerFreeSurferImporterLogic::CreateFileNode(const char* fileName)
{
  vtkMRMLColorTableNode* ctNode = vtkMRMLColorTableNode::New();
  ctNode->SetTypeToFile();
  ctNode->SaveWithSceneOff();
  ctNode->HideFromEditorsOn();
  ctNode->SetScene(this->GetMRMLScene());

  // make a storage node
  vtkNew<vtkMRMLColorTableStorageNode> colorStorageNode;
  colorStorageNode->SaveWithSceneOff();
  if (this->GetMRMLScene())
  {
    this->GetMRMLScene()->AddNode(colorStorageNode.GetPointer());
    ctNode->SetAndObserveStorageNodeID(colorStorageNode->GetID());
  }

  ctNode->GetStorageNode()->SetFileName(fileName);
  std::string basename = vtksys::SystemTools::GetFilenameWithoutExtension(fileName);
  if (this->GetMRMLScene())
  {
    std::string uname(this->GetMRMLScene()->GetUniqueNameByString(basename.c_str()));
    ctNode->SetName(uname.c_str());
  }
  else
  {
    ctNode->SetName(basename.c_str());
  }
  vtkDebugMacro("CreateFileNode: About to read user file " << fileName);

  if (ctNode->GetStorageNode()->ReadData(ctNode) == 0)
  {
    vtkErrorMacro("Unable to read file as color table " << (ctNode->GetFileName() ? ctNode->GetFileName() : ""));

    if (this->GetMRMLScene())
    {
      ctNode->SetAndObserveStorageNodeID(nullptr);
      ctNode->SetScene(nullptr);
      this->GetMRMLScene()->RemoveNode(colorStorageNode.GetPointer());
    }

    ctNode->Delete();
    return nullptr;
  }

  // Latest version of FreeSurferLut have alpha of 0.0 for all colors.
  // Manually assign alpha of 1.0.
  MRMLNodeModifyBlocker blocker(ctNode);
  for (int i = 0; i < ctNode->GetNumberOfColors(); ++i)
  {
    double color[4] = { 0.0, 0.0, 0.0, 0.0 };
    ctNode->GetColor(i, color);
    color[3] = 1.0;
    ctNode->SetColor(i, color[0], color[1], color[2], color[3]);
  }

  vtkDebugMacro("CreateFileNode: finished reading user file " << fileName);
  ctNode->SetSingletonTag(
    vtkMRMLColorLogic::GetFileColorNodeSingletonTag(fileName).c_str());

  return ctNode;
}

//----------------------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::AddFreeSurferNode(int type)
{
  vtkMRMLFreeSurferProceduralColorNode* node = this->CreateFreeSurferNode(type);
  //if (this->GetMRMLScene()->GetNodeByID(node->GetSingletonTag()) == nullptr)
  {
    //this->GetMRMLScene()->RequestNodeID(node, node->GetSingletonTag());
    this->GetMRMLScene()->AddNode(node);
    vtkDebugMacro("vtkSlicerFreeSurferImporterLogic::AddFreeSurferNode: added node " << node->GetID() << ", requested id was " << node->GetSingletonTag() << ", type = " << node->GetTypeAsString() << endl);
  }
  //else
  //  {
  //  vtkDebugMacro("vtkSlicerFreeSurferImporterLogic::AddFreeSurferNode: didn't add node " << node->GetID() << " as it was already in the scene.\n");
  //  }
  node->Delete();
}

//----------------------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::AddFreeSurferLabelNodeFromFile(const char* fileName)
{
  vtkMRMLColorTableNode* node = this->CreateFreeSurferFileNode(fileName);
  if (node)
  {
    //if (this->GetMRMLScene()->GetNodeByID(node->GetSingletonTag()) == nullptr)
    {
      //this->GetMRMLScene()->RequestNodeID(node, node->GetSingletonTag());
      this->GetMRMLScene()->AddNode(node);
    }
    //else
    //  {
    //  vtkDebugMacro("Unable to add a new colour node " << node->GetSingletonTag()
    //                << " with freesurfer colours, from file: "
    //                << node->GetStorageNode()->GetFileName()
    //                << " as there is already a node with this id in the scene");
    //  }
    node->Delete();
  }
}

//----------------------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::AddFreeSurferNodes()
{
  vtkDebugMacro("AddDefaultColorNodes: Adding Freesurfer nodes");
  vtkNew<vtkMRMLFreeSurferProceduralColorNode> basicFSNode;
  vtkDebugMacro("AddDefaultColorNodes: first type = " << basicFSNode->GetFirstType() << ", last type = " << basicFSNode->GetLastType());
  for (int type = basicFSNode->GetFirstType(); type <= basicFSNode->GetLastType(); ++type)
  {
    this->AddFreeSurferNode(type);
  }

  // add a regular colour tables holding the freesurfer volume file colours and
  // surface colours
  // get the colour file in the freesurfer share dir
  std::vector<std::string> filePath;
  filePath.emplace_back(""); // for relative path
  filePath.push_back(this->GetModuleShareDirectory());
  filePath.emplace_back(DEFAULT_FREESURFER_LABEL_FILENAME);
  std::string lutFileName = vtksys::SystemTools::JoinPath(filePath);
  this->AddFreeSurferLabelNodeFromFile(lutFileName.c_str());
}

//----------------------------------------------------------------------------
const char* vtkSlicerFreeSurferImporterLogic::GetDefaultFreeSurferLabelMapColorNodeID()
{
  return vtkSlicerFreeSurferImporterLogic::GetFreeSurferColorNodeID(vtkMRMLFreeSurferProceduralColorNode::Labels);
}

//------------------------------------------------------------------------------
void vtkSlicerFreeSurferImporterLogic::SortByBranchlessMinimumSpanningTreePosition(vtkPoints* points, vtkDoubleArray* parameters)
{
  /// Based on vtkCurveGenerator::SortByMinimumSpanningTreePosition().
  /// Restricts tree expansion to the current start/end points of the curve to prevent branching.
  if (points == nullptr)
  {
    vtkGenericWarningMacro("Input points are null. Returning");
    return;
  }

  if (parameters == nullptr)
  {
    vtkGenericWarningMacro("Output point parameters are null. Returning");
    return;
  }

  int numberOfPoints = points->GetNumberOfPoints();
  // redundant error checking, to be safe
  if (numberOfPoints < 2)
  {
    vtkGenericWarningMacro("Not enough points to compute polynomial parameters. Need at least 2 points but " << numberOfPoints << " are provided.");
    return;
  }

  // vtk boost algorithms cannot be used because they are not built with 3D Slicer
  // so this is a custom implementation of:
  // 1. constructing an undirected graph as a 2D array
  // 2. Finding the two vertices that are the farthest apart
  // 3. running prim's algorithm on the graph
  // 4. extract the "trunk" path from the last vertex to the first
  // 5. based on the distance along that path, assign each vertex a polynomial parameter value


  // in the following code, construct an undirected graph
  std::vector< double > distances(numberOfPoints * numberOfPoints);
  distances.assign(numberOfPoints * numberOfPoints, 0.0);
  // iterate through all points
  for (int v = 0; v < numberOfPoints; v++)
  {
    double pointV[3];
    points->GetPoint(v, pointV);
    for (int u = 0; u < numberOfPoints; u++)
    {
      double pointU[3];
      points->GetPoint(u, pointU);
      double distanceSquared = vtkMath::Distance2BetweenPoints(pointU, pointV);
      double distance = sqrt(distanceSquared);
      distances[v * numberOfPoints + u] = distance;
    }
  }

  // use the 1D vector as a 2D vector
  std::vector< double* > graph(numberOfPoints);
  for (int v = 0; v < numberOfPoints; v++)
  {
    graph[v] = &(distances[v * numberOfPoints]);
  }

  // implementation of Prim's algorithm heavily based on:
  // http://www.geeksforgeeks.org/greedy-algorithms-set-5-prims-minimum-spanning-tree-mst-2/
  std::vector< int > parent(numberOfPoints); // Array to store constructed MST
  parent.assign(numberOfPoints, -1);

  std::vector< bool > mstSet(numberOfPoints);  // To represent set of vertices not yet included in MST

  // The current start and end points of the curve that we can use to expand the tree
  std::vector< int > endPoints(2);
  endPoints.assign(2, 0);
  endPoints[0] = 0;
  endPoints[1] = 0;

  // Initialize all keys as INFINITE (or at least as close as we can get)
  for (int i = 0; i < numberOfPoints; i++)
  {
    mstSet[i] = false;
  }
  mstSet[0] = true;

  // The MST will have numberOfPoints vertices
  for (int count = 0; count < numberOfPoints - 1; count++)
  {
    // Pick the minimum key vertex from the set of vertices
    // not yet included in MST
    int nextPointIndex = -1;
    double minDistance = VTK_DOUBLE_MAX;
    int endPointIndex = 0;
    for (int currentEndPointIndex = 0; currentEndPointIndex < 2; ++currentEndPointIndex)
    {
      if (currentEndPointIndex == 1 && endPoints[0] == endPoints[1])
      {
        continue;
      }

      for (int v = 0; v < numberOfPoints; v++)
      {
        double currentDistance = graph[endPoints[currentEndPointIndex]][v];
        if (mstSet[v] == false && currentDistance < minDistance)
        {
          minDistance = currentDistance;
          nextPointIndex = v;
          endPointIndex = currentEndPointIndex;
        }
      }
    }

    // Add the picked vertex to the MST Set
    mstSet[nextPointIndex] = true;
    if (endPointIndex == 0)
    {
      parent[endPoints[endPointIndex]] = nextPointIndex;
    }
    else
    {
      parent[nextPointIndex] = endPoints[endPointIndex];
    }
    endPoints[endPointIndex] = nextPointIndex;

  }

  // determine the "trunk" path of the tree, from first index to last index
  std::vector< int > pathIndices;
  int currentPathIndex = endPoints[1];
  while (currentPathIndex != -1)
  {
    pathIndices.push_back(currentPathIndex);
    currentPathIndex = parent[currentPathIndex]; // go up the tree one layer
  }

  // find the sum of distances along the trunk path of the tree
  double sumOfDistances = 0.0;
  for (unsigned int i = 0; i < pathIndices.size() - 1; i++)
  {
    int pathVertexIndexI = pathIndices[i];
    int pathVertexIndexIPlus1 = pathIndices[i + 1];
    sumOfDistances += graph[pathVertexIndexI][pathVertexIndexIPlus1];
  }

  // check this to prevent a division by zero (in case all points are duplicates)
  if (sumOfDistances == 0)
  {
    vtkGenericWarningMacro("Minimum spanning tree path has distance zero. No parameters will be assigned. Check inputs (are there duplicate points?).");
    return;
  }

  // find the parameters along the trunk path of the tree
  std::vector< double > pathParameters;
  double currentDistance = 0.0;
  for (unsigned int i = 0; i < pathIndices.size() - 1; i++)
  {
    pathParameters.push_back(currentDistance / sumOfDistances);
    int pathVertexIndexI = pathIndices[i];
    int pathVertexIndexIPlus1 = pathIndices[i + 1];
    currentDistance += graph[pathVertexIndexI][pathVertexIndexIPlus1];
  }
  pathParameters.push_back(currentDistance / sumOfDistances); // this should be 1.0

  // finally assign polynomial parameters to each point, and store in the output array
  parameters->Reset();
  for (int i = 0; i < numberOfPoints; i++)
  {
    int currentIndex = i;
    bool alongPath = false;
    int indexAlongPath = -1;
    for (unsigned int j = 0; j < pathIndices.size(); j++)
    {
      if (pathIndices[j] == currentIndex)
      {
        alongPath = true;
        indexAlongPath = j;
        break;
      }
    }
    while (!alongPath)
    {
      currentIndex = parent[currentIndex];
      for (unsigned int j = 0; j < pathIndices.size(); j++)
      {
        if (pathIndices[j] == currentIndex)
        {
          alongPath = true;
          indexAlongPath = j;
          break;
        }
      }
    }
    parameters->InsertNextTuple1(pathParameters[indexAlongPath]);
  }
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsCurveNode* vtkSlicerFreeSurferImporterLogic::LoadFreeSurferCurve(std::string fileName)
{
  if (!this->GetMRMLScene())
  {
    vtkErrorMacro("Invalid scene");
    return nullptr;
  }

  vtkNew<vtkFloatArray> floatArray;
  vtkNew<vtkPoints> points;

  vtkNew<vtkFSSurfaceLabelReader> reader;
  reader->SetFileName(fileName.c_str());
  reader->SetOutput(floatArray.GetPointer());
  reader->SetPoints(points);
  reader->UseFileIndicesOff();
  int errorCode = reader->ReadLabel();
  if (errorCode != 0)
  {
    // Reading failed
    std::string errorDetail;
    switch (errorCode)
    {
    case 1: errorDetail = "Output is null"; break;
    case 2: errorDetail = "FileName not specified"; break;
    case 3: errorDetail = "Could not open file"; break;
    case 4: errorDetail = "Number of values in the file is 0 or negative, or greater than number of vertices in the associated scalar file"; break;
    case 5: errorDetail = "Error allocating the array of floats"; break;
    case 6: errorDetail = "Unexpected EOF"; break;
    default: errorDetail = "Unknown error"; break;
    }
    vtkErrorMacro("Error reading FreeSurfer scalar overlay file " << fileName.c_str() << ": " << errorDetail << " (" << errorCode << ")");
    return nullptr;
  }

  if (points->GetNumberOfPoints() == 0)
  {
    vtkErrorMacro("No points found in label");
    return nullptr;
  }

  std::string nodeName = vtksys::SystemTools::GetFilenameName(fileName);
  vtkMRMLMarkupsCurveNode* curveNode = vtkMRMLMarkupsCurveNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLMarkupsCurveNode", nodeName));
  if (!curveNode)
  {
    vtkErrorMacro("Could not create curve node");
    return nullptr;
  }

  // Need to add a cell to prevent "unused" points from being deleted by vtkCleanPolyData
  vtkNew<vtkCellArray> lines;
  vtkNew<vtkIdList> line;
  for (int i = 0; i < points->GetNumberOfPoints(); ++i)
  {
    line->InsertNextId(i);
  }
  lines->InsertNextCell(line);

  vtkNew<vtkPolyData> curvePolyData;
  curvePolyData->SetPoints(points);
  curvePolyData->SetLines(lines);

  // Remove duplicate points from input
  vtkNew<vtkCleanPolyData> cleanFilter;
  cleanFilter->SetInputData(curvePolyData);
  cleanFilter->Update();
  points->ShallowCopy(cleanFilter->GetOutput()->GetPoints());

  vtkNew<vtkDoubleArray> weights;
  vtkSlicerFreeSurferImporterLogic::SortByBranchlessMinimumSpanningTreePosition(points, weights);
  vtkSortDataArray::Sort(weights, points->GetData());

  curveNode->SetCurveTypeToLinear();
  curveNode->SetControlPointPositionsWorld(points);
  return curveNode;
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsPlaneNode* vtkSlicerFreeSurferImporterLogic::LoadFreeSurferPlane(std::string fileName)
{
  double planeOrigin_Node[4] = { 0.0, 0.0, 0.0, 1.0 };
  double planeNormal_Node[4] = { 0.0, 0.0, 1.0, 0.0 };

  std::string extension = vtksys::SystemTools::GetFilenameExtension(fileName);
  if (extension == ".label")
  {
    vtkMRMLMarkupsCurveNode* curveNode = this->LoadFreeSurferCurve(fileName);

    bool success = false;
    if (curveNode)
    {
      vtkNew<vtkMatrix4x4> transformToBestFitPlane;
      success = vtkAddonMathUtilities::FitPlaneToPoints(curveNode->GetCurvePoints(), transformToBestFitPlane);
      if (success)
      {
        transformToBestFitPlane->MultiplyPoint(planeOrigin_Node, planeOrigin_Node);
        transformToBestFitPlane->MultiplyPoint(planeNormal_Node, planeNormal_Node);
      }

      this->GetMRMLScene()->RemoveNode(curveNode);
    }

    if (!success)
    {
      vtkErrorMacro("LoadFreeSurferPlane: Could not load plane from curve");
      return nullptr;
    }
  }
  else
  {
    std::ifstream filestream(fileName.c_str());
    std::string line;

    if (std::getline(filestream, line))
    {
      std::stringstream linestream;
      linestream << line;
      linestream >> planeOrigin_Node[0] >> planeOrigin_Node[1] >> planeOrigin_Node[2];
    }
    else
    {
      vtkErrorMacro("LoadFreeSurferPlane: Could not load plane");
      return nullptr;
    }

    // TODO: Update plane normal from file
    planeNormal_Node[0] = 0.0;
    planeNormal_Node[1] = 1.0;
    planeNormal_Node[2] = 0.0;
  }

  std::string nodeName = vtksys::SystemTools::GetFilenameName(fileName);
  vtkMRMLMarkupsPlaneNode* planeNode = vtkMRMLMarkupsPlaneNode::SafeDownCast(this->GetMRMLScene()->AddNewNodeByClass("vtkMRMLMarkupsPlaneNode", nodeName));
  if (!planeNode)
  {
    vtkErrorMacro("LoadFreeSurferPlane: Could not create plane node");
    return nullptr;
  }
  planeNode->SetNormalWorld(planeNormal_Node);
  planeNode->SetOriginWorld(planeOrigin_Node);

  double defaultPlaneSizeMm = 50.0;
  planeNode->SetSizeMode(vtkMRMLMarkupsPlaneNode::SizeModeAbsolute);
  planeNode->SetPlaneBounds(-defaultPlaneSizeMm, defaultPlaneSizeMm, -defaultPlaneSizeMm, defaultPlaneSizeMm);
  return planeNode;
}