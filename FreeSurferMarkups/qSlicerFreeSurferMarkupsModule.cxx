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

#include <qSlicerFreeSurferMarkupsModule.h>

// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerCoreIOManager.h>

// MRMLDisplayableManager includes
#include <vtkMRMLSliceViewDisplayableManagerFactory.h>
#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

// MRML includes
#include <vtkMRMLMarkupsFreeSurferCurveNode.h>

// Logic includes
#include <vtkSlicerFreeSurferMarkupsLogic.h>

// Markups logic includes
#include <vtkSlicerMarkupsLogic.h>

// Markups VTKWidgets includes
#include <vtkSlicerCurveWidget.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFreeSurferMarkupsModulePrivate
{
public:
  qSlicerFreeSurferMarkupsModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFreeSurferMarkupsModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFreeSurferMarkupsModulePrivate::qSlicerFreeSurferMarkupsModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFreeSurferMarkupsModule methods

//-----------------------------------------------------------------------------
qSlicerFreeSurferMarkupsModule::qSlicerFreeSurferMarkupsModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferMarkupsModulePrivate)
{
  this->setWidgetRepresentationCreationEnabled(false);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferMarkupsModule::~qSlicerFreeSurferMarkupsModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferMarkupsModule::helpText() const
{
  return "This is a module for importing FreeSurfer files into Slicer";
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferMarkupsModule::acknowledgementText() const
{
  return "This work was supported through CANARIE's Research Software Program, Cancer Care Ontario,"
    "OpenAnatomy, and Brigham and Women's Hospital through NIH grant R01MH112748.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferMarkupsModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kyle Sunderland (PerkLab, Queen's University)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFreeSurferMarkupsModule::icon() const
{
  return QIcon(":/Icons/FreeSurferImporter.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferMarkupsModule::categories() const
{
  return QStringList() << "Utilities";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferMarkupsModule::dependencies() const
{
  return QStringList() << "Markups";
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferMarkupsModule::setup()
{
  this->Superclass::setup();

  vtkSlicerMarkupsLogic* markupsLogic = vtkSlicerMarkupsLogic::SafeDownCast(this->logic()->GetMRMLApplicationLogic()->GetModuleLogic("Markups"));
  if (!markupsLogic)
  {
    qCritical("Could not find Markups logic");
    return;
  }

  vtkNew<vtkSlicerCurveWidget> curveWidget;
  vtkNew<vtkMRMLMarkupsFreeSurferCurveNode> curveNode;
  markupsLogic->RegisterMarkupsNode(curveNode, curveWidget, true);
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFreeSurferMarkupsModule
::createWidgetRepresentation()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFreeSurferMarkupsModule::createLogic()
{
  return vtkSlicerFreeSurferMarkupsLogic::New();
}
