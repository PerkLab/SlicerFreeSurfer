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

#ifndef __qSlicerFreeSurferImporterModuleWidget_h
#define __qSlicerFreeSurferImporterModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerFreeSurferImporterModuleExport.h"

class qSlicerFreeSurferImporterModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_FREESURFERIMPORTER_EXPORT qSlicerFreeSurferImporterModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerFreeSurferImporterModuleWidget(QWidget* parent = 0);
  virtual ~qSlicerFreeSurferImporterModuleWidget();

  /// Overridden to add node added/removed observer on scene
  void setMRMLScene(vtkMRMLScene* scene) override;

public slots:
  /// Adds files to the input selector boxes for the files in the input directory
  void updateFileList();
  /// Updates the reference volume selector for all currently loaded and to-be-loaded volumes
  void updateReferenceVolumeSelector();
  /// Load the selected files
  bool loadSelectedFiles();

protected:
  QScopedPointer<qSlicerFreeSurferImporterModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerFreeSurferImporterModuleWidget);
  Q_DISABLE_COPY(qSlicerFreeSurferImporterModuleWidget);
};

#endif
