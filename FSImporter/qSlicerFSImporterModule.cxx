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

// FSImporter Logic includes
#include <vtkSlicerFSImporterLogic.h>

// FSImporter includes
#include "qSlicerFSImporterModule.h"
#include "qSlicerFSImporterModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFSImporterModulePrivate
{
public:
  qSlicerFSImporterModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFSImporterModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFSImporterModulePrivate::qSlicerFSImporterModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFSImporterModule methods

//-----------------------------------------------------------------------------
qSlicerFSImporterModule::qSlicerFSImporterModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFSImporterModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFSImporterModule::~qSlicerFSImporterModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFSImporterModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFSImporterModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFSImporterModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFSImporterModule::icon() const
{
  return QIcon(":/Icons/FSImporter.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFSImporterModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFSImporterModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFSImporterModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFSImporterModule
::createWidgetRepresentation()
{
  return new qSlicerFSImporterModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFSImporterModule::createLogic()
{
  return vtkSlicerFSImporterLogic::New();
}
