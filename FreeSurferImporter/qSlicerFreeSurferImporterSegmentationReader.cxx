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
and was supported through CANARIE's Research Software Program, and Cancer
Care Ontario.

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerFreeSurferImporter include
#include "vtkSlicerFreeSurferImporterLogic.h"
#include "qSlicerFreeSurferImporterModule.h"

// SlicerQt includes
#include "qSlicerFreeSurferImporterSegmentationReader.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSegmentationNode.h>

// VTK includes
#include <vtkSmartPointer.h>


//-----------------------------------------------------------------------------
class qSlicerFreeSurferImporterSegmentationReaderPrivate
{
public:
  vtkSlicerFreeSurferImporterLogic* Logic;
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterSegmentationReader::qSlicerFreeSurferImporterSegmentationReader(vtkSlicerFreeSurferImporterLogic* newFreeSurferImporterLogic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterSegmentationReaderPrivate)
{
  this->setLogic(newFreeSurferImporterLogic);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterSegmentationReader::~qSlicerFreeSurferImporterSegmentationReader()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterSegmentationReader::description() const
{
  return "FreeSurfer segmentation";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerFreeSurferImporterSegmentationReader::fileType() const
{
  return "FreeSurferSegmentationFile";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterSegmentationReader::extensions() const
{
  QStringList supportedExtensions = QStringList();
  supportedExtensions << "FreeSurfer segmentation (aseg.mgz)";
  supportedExtensions << "FreeSurfer segmentation (aseg.*.mgz)";
  supportedExtensions << "FreeSurfer segmentation (aparc.*.mgz)";
  supportedExtensions << "FreeSurfer segmentation (aparc+aseg.mgz)";
  supportedExtensions << "FreeSurfer segmentation (wmparc.mgz)";
  supportedExtensions << "FreeSurfer segmentation (*.mgz)";
  return supportedExtensions;
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterSegmentationReader::setLogic(vtkSlicerFreeSurferImporterLogic* newFreeSurferImporterLogic)
{
  Q_D(qSlicerFreeSurferImporterSegmentationReader);
  d->Logic = newFreeSurferImporterLogic;
}

//-----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic* qSlicerFreeSurferImporterSegmentationReader::Logic() const
{
  Q_D(const qSlicerFreeSurferImporterSegmentationReader);
  return d->Logic;
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterSegmentationReader::load(const IOProperties& properties)
{
  Q_D(qSlicerFreeSurferImporterSegmentationReader);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  vtkMRMLSegmentationNode* node = d->Logic->LoadFreeSurferSegmentation(fileName.toStdString());
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
  return true;
}
