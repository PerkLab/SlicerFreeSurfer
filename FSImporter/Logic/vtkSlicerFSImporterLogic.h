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

// .NAME vtkSlicerFSImporterLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFSImporterLogic_h
#define __vtkSlicerFSImporterLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
class vtkMRMLScalarVolumeNode;
class vtkMRMLSegmentationNode;
class vtkMRMLModelNode;

// STD includes
#include <cstdlib>

#include "vtkSlicerFSImporterModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FSIMPORTER_MODULE_LOGIC_EXPORT vtkSlicerFSImporterLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerFSImporterLogic *New();
  vtkTypeMacro(vtkSlicerFSImporterLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkMRMLScalarVolumeNode* loadVolume(std::string fsDirectory, std::string name);
  vtkMRMLSegmentationNode* loadSegmentation(std::string fsDirectory, std::string name);
  vtkMRMLModelNode* loadModel(std::string fsDirectory, std::string name);

  void transformModelToRAS(vtkMRMLModelNode* surf, vtkMRMLScalarVolumeNode* orig);
  void applyFreeSurferSegmentationNames(vtkMRMLSegmentationNode* segmentation);

protected:
  vtkSlicerFSImporterLogic();
  virtual ~vtkSlicerFSImporterLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerFSImporterLogic(const vtkSlicerFSImporterLogic&); // Not implemented
  void operator=(const vtkSlicerFSImporterLogic&); // Not implemented
};

#endif
