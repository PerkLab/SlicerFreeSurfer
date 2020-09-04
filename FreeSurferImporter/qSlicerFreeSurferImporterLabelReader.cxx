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
#include "qSlicerFreeSurferImporterLabelReader.h"
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
class qSlicerFreeSurferImporterLabelReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerFreeSurferImporterLogic> Logic;
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterLabelReader::qSlicerFreeSurferImporterLabelReader(
  vtkSlicerFreeSurferImporterLogic* _logic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterLabelReaderPrivate)
{
  this->setLogic(_logic);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterLabelReader::~qSlicerFreeSurferImporterLabelReader()
= default;

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterLabelReader::setLogic(vtkSlicerFreeSurferImporterLogic * logic)
{
  Q_D(qSlicerFreeSurferImporterLabelReader);
  d->Logic = logic;
}

//-----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic* qSlicerFreeSurferImporterLabelReader::Logic() const
{
  Q_D(const qSlicerFreeSurferImporterLabelReader);
  return d->Logic;
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterLabelReader::description()const
{
  return "FreeSurfer label";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerFreeSurferImporterLabelReader::fileType()const
{
  return QString("FreeSurferLabelFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterLabelReader::extensions()const
{
  QStringList supportedExtensions;
  supportedExtensions << "*.label";
  return supportedExtensions;
}

//-----------------------------------------------------------------------------
qSlicerIOOptions* qSlicerFreeSurferImporterLabelReader::options()const
{
  qSlicerIOOptionsWidget* options = new qSlicerFreeSurferImporterScalarOverlayOptionsWidget;
  options->setMRMLScene(this->mrmlScene());
  return options;
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterLabelReader::load(const IOProperties & properties)
{
  Q_D(qSlicerFreeSurferImporterLabelReader);
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
  vtkMRMLMarkupsNode* markupNode = d->Logic->LoadFreeSurferLabel(fileName.toStdString());
  if (markupNode)
  {
    loadedNodes << QString(markupNode->GetID());
  }
  this->setLoadedNodes(loadedNodes);
  return markupNode != nullptr;
}
