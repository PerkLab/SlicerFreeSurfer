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

  This file was originally developed by Kyle Sunderland, PerkLab, Queen's University
  and was supported through CANARIE's Research Software Program, Cancer
  Care Ontario, OpenAnatomy, and Brigham and Women's Hospital through NIH grant R01MH112748.

==============================================================================*/

#ifndef __vtkMRMLFreeSurferMarkupsDisplayableManager_h
#define __vtkMRMLFreeSurferMarkupsDisplayableManager_h

// VTK includes
#include <vtkObjectFactory.h>

// MarkupsModule includes
#include "vtkSlicerFreeSurferMarkupsModuleMRMLDisplayableManagerExport.h"

#include <vtkMRMLMarkupsDisplayableManager.h>

/// \ingroup Slicer_QtModules_Markups
class VTK_SLICER_FREESURFERMARKUPS_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkMRMLFreeSurferMarkupsDisplayableManager :
    public vtkMRMLMarkupsDisplayableManager
{
public:

  static vtkMRMLFreeSurferMarkupsDisplayableManager *New();
  vtkTypeMacro(vtkMRMLFreeSurferMarkupsDisplayableManager, vtkMRMLMarkupsDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:

  vtkMRMLFreeSurferMarkupsDisplayableManager();
  ~vtkMRMLFreeSurferMarkupsDisplayableManager()override;

private:
  vtkMRMLFreeSurferMarkupsDisplayableManager(const vtkMRMLFreeSurferMarkupsDisplayableManager&) = delete;
  void operator=(const vtkMRMLFreeSurferMarkupsDisplayableManager&) = delete;
};

#endif
