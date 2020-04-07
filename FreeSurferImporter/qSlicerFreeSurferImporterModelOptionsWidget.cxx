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

  This file was originally developed by Kyle Sunderland, PerkLab, Queen's University
  and was supported through CANARIE's Research Software Program, Cancer
  Care Ontario, OpenAnatomy, and Brigham and Women's Hospital through NIH grant R01MH112748.

==============================================================================*/

/// Qt includes
#include <QDebug>

/// ScalarOverlay includes
#include "qSlicerIOOptions_p.h"
#include "qSlicerFreeSurferImporterModelOptionsWidget.h"
#include "ui_qSlicerFreeSurferImporterModelOptionsWidget.h"

// MRML includes
#include <vtkMRMLNode.h>

//-----------------------------------------------------------------------------
class qSlicerFreeSurferImporterModelOptionsWidgetPrivate
  : public qSlicerIOOptionsPrivate
  , public Ui_qSlicerFreeSurferImporterModelOptionsWidget
{
public:
};

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModelOptionsWidget::qSlicerFreeSurferImporterModelOptionsWidget(QWidget* parentWidget)
  : Superclass(new qSlicerFreeSurferImporterModelOptionsWidgetPrivate, parentWidget)
{
  Q_D(qSlicerFreeSurferImporterModelOptionsWidget);
  d->setupUi(this);

  connect(d->VolumeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(updateProperties()));
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModelOptionsWidget::~qSlicerFreeSurferImporterModelOptionsWidget()
= default;

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModelOptionsWidget::updateProperties()
{
  Q_D(qSlicerFreeSurferImporterModelOptionsWidget);
  vtkMRMLNode* volumeNode = d->VolumeSelector->currentNode();
  if (volumeNode)
  {
    d->Properties["referenceVolumeID"] = QString(volumeNode->GetID());
  }
  else
  {
    d->Properties.remove("referenceVolumeID");
  }
  this->updateValid();
}
