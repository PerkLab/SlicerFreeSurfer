/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QDebug>

// 
#include "qSlicerFreeSurferImporterScalarOverlayReader.h"
#include "vtkSlicerFreeSurferImporterLogic.h"
#include "qSlicerFreeSurferImporterScalarOverlayOptionsWidget.h"

// MRML includes
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLStorageNode.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerFreeSurferImporterScalarOverlayReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerFreeSurferImporterLogic> Logic;
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterScalarOverlayReader::qSlicerFreeSurferImporterScalarOverlayReader(
  vtkSlicerFreeSurferImporterLogic* _logic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterScalarOverlayReaderPrivate)
{
  this->setLogic(_logic);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterScalarOverlayReader::~qSlicerFreeSurferImporterScalarOverlayReader()
= default;

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterScalarOverlayReader::setLogic(vtkSlicerFreeSurferImporterLogic * logic)
{
  Q_D(qSlicerFreeSurferImporterScalarOverlayReader);
  d->Logic = logic;
}

//-----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic* qSlicerFreeSurferImporterScalarOverlayReader::Logic() const
{
  Q_D(const qSlicerFreeSurferImporterScalarOverlayReader);
  return d->Logic;
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterScalarOverlayReader::description()const
{
  return "FreeSurfer scalar overlay";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerFreeSurferImporterScalarOverlayReader::fileType()const
{
  return QString("FreeSurferScalarOverlayFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterScalarOverlayReader::extensions()const
{
  QStringList supportedExtensions;
  supportedExtensions << "*.area";
  supportedExtensions << "*.sulc";
  supportedExtensions << "*.curv";
  supportedExtensions << "*.W";
  supportedExtensions << "*.annot";
  supportedExtensions << "*.label";
  supportedExtensions << "*.*";
  return supportedExtensions;
}

//-----------------------------------------------------------------------------
qSlicerIOOptions* qSlicerFreeSurferImporterScalarOverlayReader::options()const
{
  qSlicerIOOptionsWidget* options = new qSlicerFreeSurferImporterScalarOverlayOptionsWidget;
  options->setMRMLScene(this->mrmlScene());
  return options;
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterScalarOverlayReader::load(const IOProperties & properties)
{
  Q_D(qSlicerFreeSurferImporterScalarOverlayReader);
  Q_ASSERT(properties.contains("fileName"));
  if (!properties.contains("modelNodeId"))
  {
    qCritical() << Q_FUNC_INFO << " failed: missing fileName or modelNodeId property";
    return false;
  }
  QString fileName = properties["fileName"].toString();

  QString modelNodeId = properties["modelNodeId"].toString();
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(
    this->mrmlScene()->GetNodeByID(modelNodeId.toUtf8()));
  if (modelNode == nullptr)
  {
    qCritical() << Q_FUNC_INFO << " failed: modelNodeId refers to invalid/non-existent node";
    return false;
  }

  if (d->Logic == nullptr)
  {
    qCritical() << Q_FUNC_INFO << " failed: invalid module logic";
    return false;
  }

  QStringList loadedNodes;
  bool success = d->Logic->LoadFreeSurferScalarOverlay(fileName.toStdString(), modelNode);
  if (success)
  {
    loadedNodes << QString(modelNodeId);
  }
  this->setLoadedNodes(loadedNodes);
  return success;
}
