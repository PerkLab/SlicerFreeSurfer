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

#ifndef __qSlicerFreeSurferMarkupsModule_h
#define __qSlicerFreeSurferMarkupsModule_h

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerFreeSurferMarkupsModuleExport.h"

class qSlicerFreeSurferMarkupsModulePrivate;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_FREESURFERMARKUPS_EXPORT
qSlicerFreeSurferMarkupsModule
  : public qSlicerLoadableModule
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerFreeSurferMarkupsModule(QObject *parent=0);
  virtual ~qSlicerFreeSurferMarkupsModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  QString helpText() const override;
  QString acknowledgementText() const override;
  QStringList contributors()const override;

  QIcon icon()const override;

  QStringList categories() const override;
  QStringList dependencies() const override;

protected:

  /// Initialize the module. Register the volumes reader/writer
  void setup() override;

  /// Create and return the widget representation associated to this module
  qSlicerAbstractModuleRepresentation * createWidgetRepresentation() override;

  /// Create and return the logic associated to this module
  vtkMRMLAbstractLogic* createLogic() override;

protected:
  QScopedPointer<qSlicerFreeSurferMarkupsModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerFreeSurferMarkupsModule);
  Q_DISABLE_COPY(qSlicerFreeSurferMarkupsModule);

};

#endif
