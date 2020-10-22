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

// .NAME vtkSlicerFreeSurferMarkupsLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFreeSurferMarkupsLogic_h
#define __vtkSlicerFreeSurferMarkupsLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

#include "vtkSlicerFreeSurferMarkupsModuleLogicExport.h"

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FREESURFERMARKUPS_MODULE_LOGIC_EXPORT vtkSlicerFreeSurferMarkupsLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerFreeSurferMarkupsLogic* New();
  vtkTypeMacro(vtkSlicerFreeSurferMarkupsLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkSlicerFreeSurferMarkupsLogic();
  virtual ~vtkSlicerFreeSurferMarkupsLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  void RegisterNodes() override;
  void UpdateFromMRMLScene() override;

  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;

  void ObserveMRMLScene() override;

private:

  vtkSlicerFreeSurferMarkupsLogic(const vtkSlicerFreeSurferMarkupsLogic&); // Not implemented
  void operator=(const vtkSlicerFreeSurferMarkupsLogic&); // Not implemented
};

#endif
