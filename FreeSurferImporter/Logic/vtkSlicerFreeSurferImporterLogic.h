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

// .NAME vtkSlicerFreeSurferImporterLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFreeSurferImporterLogic_h
#define __vtkSlicerFreeSurferImporterLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
class vtkMRMLColorTableNode;
class vtkDoubleArray;
class vtkMRMLFreeSurferProceduralColorNode;
class vtkMRMLMarkupsNode;
class vtkMRMLModelNode;
class vtkMRMLMarkupsPlaneNode;
class vtkPoints;
class vtkMRMLSegmentationNode;
class vtkMRMLVolumeNode;

// STD includes
#include <cstdlib>
#include <string>

#include "vtkSlicerFreeSurferImporterModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FREESURFERIMPORTER_MODULE_LOGIC_EXPORT vtkSlicerFreeSurferImporterLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerFreeSurferImporterLogic* New();
  vtkTypeMacro(vtkSlicerFreeSurferImporterLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkMRMLVolumeNode* LoadFreeSurferVolume(std::string filePath);
  vtkMRMLSegmentationNode* LoadFreeSurferSegmentation(std::string filePath);
  vtkMRMLModelNode* LoadFreeSurferModel(std::string filePath);
  bool LoadFreeSurferScalarOverlay(std::string filePath, std::vector<vtkMRMLModelNode*> modelNodes);
  vtkMRMLMarkupsNode* LoadFreeSurferCurve(std::string fileName);
  vtkMRMLMarkupsPlaneNode* LoadFreeSurferPlane(std::string fileName);

  void TransformFreeSurferModelToWorld(vtkMRMLModelNode* surf, vtkMRMLVolumeNode* orig);
  void ApplyFreeSurferSegmentationLUT(vtkMRMLSegmentationNode* segmentation);

  /// Return the default freesurfer color node id for a given type
  static const char* GetFreeSurferColorNodeID(int type);

  /// Return a default color node id for a freesurfer label map volume
  virtual const char* GetDefaultFreeSurferLabelMapColorNodeID();

protected:
  vtkSlicerFreeSurferImporterLogic();
  virtual ~vtkSlicerFreeSurferImporterLogic();

  /// Called when the scene fires vtkMRMLScene::NewSceneEvent.
  /// We add the default LUTs.
  virtual void OnMRMLSceneNewEvent();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

  vtkMRMLFreeSurferProceduralColorNode* CreateFreeSurferNode(int type);
  vtkMRMLColorTableNode* CreateFreeSurferFileNode(const char* fileName);
  void AddFreeSurferNode(int type);
  void AddFreeSurferLabelNodeFromFile(const char* fileName);
  void AddFreeSurferNodes();
  vtkMRMLColorTableNode* CreateFileNode(const char* fileName);

  static void SortByBranchlessMinimumSpanningTreePosition(vtkPoints* points, vtkDoubleArray* parameters);

  static std::string TempColorNodeID;

private:

  vtkSlicerFreeSurferImporterLogic(const vtkSlicerFreeSurferImporterLogic&); // Not implemented
  void operator=(const vtkSlicerFreeSurferImporterLogic&); // Not implemented
};

#endif
