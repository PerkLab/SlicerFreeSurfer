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

// FreeSurferImporter Logic includes
#include <vtkSlicerFreeSurferImporterLogic.h>

// FreeSurferImporter includes
#include "qSlicerFreeSurferImporterModule.h"
#include "qSlicerFreeSurferImporterModuleWidget.h"
#include "qSlicerFreeSurferImporterModelReader.h"

// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerCoreIOManager.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFreeSurferImporterModulePrivate
{
public:
  qSlicerFreeSurferImporterModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFreeSurferImporterModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModulePrivate::qSlicerFreeSurferImporterModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFreeSurferImporterModule methods

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModule::qSlicerFreeSurferImporterModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModule::~qSlicerFreeSurferImporterModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterModule::helpText() const
{
  return "This is a module for importing FreeSurfer files into Slicer";
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterModule::acknowledgementText() const
{
  return "This work was supported through CANARIE's Research Software Program, Cancer Care Ontario,"
    "OpenAnatomy, and Brigham and Women's Hospital through NIH grant R01MH112748.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kyle Sunderland (PerkLab, Queen's University)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFreeSurferImporterModule::icon() const
{
  return QIcon(":/Icons/FreeSurferImporter.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModule::setup()
{
  this->Superclass::setup();

  qSlicerCoreApplication* app = qSlicerCoreApplication::application();

  // Register the IO
  vtkSlicerFreeSurferImporterLogic* logic = vtkSlicerFreeSurferImporterLogic::SafeDownCast(this->logic());
  app->coreIOManager()->registerIO(new qSlicerFreeSurferImporterModelReader(logic, this));
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFreeSurferImporterModule
::createWidgetRepresentation()
{
  return new qSlicerFreeSurferImporterModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFreeSurferImporterModule::createLogic()
{
  return vtkSlicerFreeSurferImporterLogic::New();
}
