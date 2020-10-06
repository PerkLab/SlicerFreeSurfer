/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

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

// Qt includes
#include <QDebug>

// 
#include "qSlicerFreeSurferImporterCurveReader.h"
#include "vtkSlicerFreeSurferImporterLogic.h"
#include "qSlicerFreeSurferImporterScalarOverlayOptionsWidget.h"

// MRML includes
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLStorageNode.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerFreeSurferImporterCurveReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerFreeSurferImporterLogic> Logic;
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterCurveReader::qSlicerFreeSurferImporterCurveReader(
  vtkSlicerFreeSurferImporterLogic* _logic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterCurveReaderPrivate)
{
  this->setLogic(_logic);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterCurveReader::~qSlicerFreeSurferImporterCurveReader()
= default;

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterCurveReader::setLogic(vtkSlicerFreeSurferImporterLogic * logic)
{
  Q_D(qSlicerFreeSurferImporterCurveReader);
  d->Logic = logic;
}

//-----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic* qSlicerFreeSurferImporterCurveReader::Logic() const
{
  Q_D(const qSlicerFreeSurferImporterCurveReader);
  return d->Logic;
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterCurveReader::description()const
{
  return "FreeSurfer Curve";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerFreeSurferImporterCurveReader::fileType()const
{
  return QString("FreeSurferCurveFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterCurveReader::extensions()const
{
  QStringList supportedExtensions;
  supportedExtensions << "*.label";
  return supportedExtensions;
}

//-----------------------------------------------------------------------------
qSlicerIOOptions* qSlicerFreeSurferImporterCurveReader::options()const
{
  qSlicerIOOptionsWidget* options = new qSlicerFreeSurferImporterScalarOverlayOptionsWidget;
  options->setMRMLScene(this->mrmlScene());
  return options;
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterCurveReader::load(const IOProperties & properties)
{
  Q_D(qSlicerFreeSurferImporterCurveReader);
  Q_ASSERT(properties.contains("fileName"));
  if (d->Logic == nullptr)
  {
    qCritical() << Q_FUNC_INFO << " failed: invalid module logic";
    return false;
  }

  QString fileName = properties["fileName"].toString();

  vtkMRMLModelNode* modelNode = nullptr;
  
  if (properties.contains("modelNodeId"))
  {
    QString modelNodeId = properties["modelNodeId"].toString();
    modelNode = vtkMRMLModelNode::SafeDownCast(
      this->mrmlScene()->GetNodeByID(modelNodeId.toUtf8()));
  }

  QStringList loadedNodes;
  vtkMRMLMarkupsNode* markupNode = d->Logic->LoadFreeSurferCurve(fileName.toStdString());
  if (markupNode)
  {
    loadedNodes << QString(markupNode->GetID());
  }
  this->setLoadedNodes(loadedNodes);
  return markupNode != nullptr;
}
