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
class vtkMRMLVolumeNode;
class vtkMRMLSegmentationNode;
class vtkMRMLModelNode;

// STD includes
#include <cstdlib>

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

  void TransformFreeSurferModelToWorld(vtkMRMLModelNode* surf, vtkMRMLVolumeNode* orig);
  void ApplyFreeSurferSegmentationLUT(vtkMRMLSegmentationNode* segmentation);

protected:
  vtkSlicerFreeSurferImporterLogic();
  virtual ~vtkSlicerFreeSurferImporterLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerFreeSurferImporterLogic(const vtkSlicerFreeSurferImporterLogic&); // Not implemented
  void operator=(const vtkSlicerFreeSurferImporterLogic&); // Not implemented
};

#endif
