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

// SlicerFreeSurferImporter include
#include "vtkSlicerFreeSurferImporterLogic.h"
#include "qSlicerFreeSurferImporterModule.h"
#include "qSlicerFreeSurferImporterModelOptionsWidget.h"

// SlicerQt includes
#include "qSlicerFreeSurferImporterModelReader.h"

// MRML includes
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeNode.h>

// VTK includes
#include <vtkSmartPointer.h>


//-----------------------------------------------------------------------------
class qSlicerFreeSurferImporterModelReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerFreeSurferImporterLogic> Logic;
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModelReader::qSlicerFreeSurferImporterModelReader(vtkSlicerFreeSurferImporterLogic* logic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterModelReaderPrivate)
{
  this->setLogic(logic);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModelReader::~qSlicerFreeSurferImporterModelReader()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterModelReader::description() const
{
  return "FreeSurfer model";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerFreeSurferImporterModelReader::fileType() const
{
  return "FreeSurferModelFile";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterModelReader::extensions() const
{
  QStringList supportedExtensions = QStringList();
  supportedExtensions << "FreeSurfer model (*.white)";
  supportedExtensions << "FreeSurfer model (*.pial)";
  supportedExtensions << "FreeSurfer model (*.orig)";
  supportedExtensions << "FreeSurfer model (*.inflated)";
  supportedExtensions << "FreeSurfer model (*.sphere)";
  return supportedExtensions;
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModelReader::setLogic(vtkSlicerFreeSurferImporterLogic* logic)
{
  Q_D(qSlicerFreeSurferImporterModelReader);
  d->Logic = logic;
}

//-----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic* qSlicerFreeSurferImporterModelReader::Logic() const
{
  Q_D(const qSlicerFreeSurferImporterModelReader);
  return d->Logic;
}

//-----------------------------------------------------------------------------
qSlicerIOOptions* qSlicerFreeSurferImporterModelReader::options() const
{
  qSlicerIOOptionsWidget* options = new qSlicerFreeSurferImporterModelOptionsWidget;
  options->setMRMLScene(this->mrmlScene());
  return options;
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterModelReader::load(const IOProperties& properties)
{
  Q_D(qSlicerFreeSurferImporterModelReader);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  vtkMRMLModelNode* node = d->Logic->LoadFreeSurferModel(fileName.toStdString());
  if (!node)
  {
    return false;
  }
  node->CreateDefaultDisplayNodes();
  this->setLoadedNodes(QStringList(QString(node->GetID())));

  if (properties.contains("name"))
  {
    std::string uname = this->mrmlScene()->GetUniqueNameByString(
      properties["name"].toString().toUtf8());
    node->SetName(uname.c_str());
  }

  if (properties.contains("referenceVolumeID") && this->mrmlScene())
  {
    std::string referenceVolumeID = properties["referenceVolumeID"].toString().toUtf8();
    vtkMRMLVolumeNode* referenceNode = vtkMRMLVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(referenceVolumeID));
    if (referenceNode)
    {
      d->Logic->TransformFreeSurferModelToWorld(node, referenceNode);
    }
  }

  return true;
}
