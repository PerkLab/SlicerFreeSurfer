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
#include "qSlicerFreeSurferImporterPlaneReader.h"
#include "vtkSlicerFreeSurferImporterLogic.h"
#include "qSlicerFreeSurferImporterScalarOverlayOptionsWidget.h"

// MRML includes
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLMarkupsPlaneNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLStorageNode.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerFreeSurferImporterPlaneReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerFreeSurferImporterLogic> Logic;
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterPlaneReader::qSlicerFreeSurferImporterPlaneReader(
  vtkSlicerFreeSurferImporterLogic* _logic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterPlaneReaderPrivate)
{
  this->setLogic(_logic);
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterPlaneReader::~qSlicerFreeSurferImporterPlaneReader()
= default;

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterPlaneReader::setLogic(vtkSlicerFreeSurferImporterLogic * logic)
{
  Q_D(qSlicerFreeSurferImporterPlaneReader);
  d->Logic = logic;
}

//-----------------------------------------------------------------------------
vtkSlicerFreeSurferImporterLogic* qSlicerFreeSurferImporterPlaneReader::Logic() const
{
  Q_D(const qSlicerFreeSurferImporterPlaneReader);
  return d->Logic;
}

//-----------------------------------------------------------------------------
QString qSlicerFreeSurferImporterPlaneReader::description()const
{
  return "FreeSurfer Plane";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerFreeSurferImporterPlaneReader::fileType()const
{
  return QString("FreeSurferPlaneFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFreeSurferImporterPlaneReader::extensions()const
{
  QStringList supportedExtensions;
  supportedExtensions << "*";
  return supportedExtensions;
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterPlaneReader::load(const IOProperties & properties)
{
  Q_D(qSlicerFreeSurferImporterPlaneReader);
  Q_ASSERT(properties.contains("fileName"));
  if (d->Logic == nullptr)
  {
    qCritical() << Q_FUNC_INFO << " failed: invalid module logic";
    return false;
  }

  QString fileName = properties["fileName"].toString(); 

  QStringList loadedNodes;
  vtkMRMLMarkupsPlaneNode* planeNode = d->Logic->LoadFreeSurferPlane(fileName.toStdString());
  if (planeNode)
  {
    loadedNodes << QString(planeNode->GetID());
  }
  this->setLoadedNodes(loadedNodes);
  return planeNode != nullptr;
}
