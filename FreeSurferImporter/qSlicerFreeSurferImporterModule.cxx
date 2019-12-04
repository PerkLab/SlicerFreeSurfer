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
#include <vtkSlicerFreeSurferImporterLogic.h>

// FreeSurferImporter includes
#include "qSlicerFreeSurferImporterModule.h"
#include "qSlicerFreeSurferImporterModuleWidget.h"

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
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
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
