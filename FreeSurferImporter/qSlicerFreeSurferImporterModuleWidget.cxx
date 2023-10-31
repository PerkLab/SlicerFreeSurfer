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

// Qt includes
#include <QDebug>

#include "qSlicerFreeSurferImporterModule.h"
#include "vtkSlicerFreeSurferImporterLogic.h"

// SlicerQt includes
#include "qSlicerFreeSurferImporterModuleWidget.h"
#include "ui_qSlicerFreeSurferImporterModuleWidget.h"
#include <qSlicerIOManager.h>

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLSegmentationNode.h>
#include <vtkMRMLSegmentationDisplayNode.h>
#include <vtkMRMLSegmentationStorageNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLFreeSurferModelOverlayStorageNode.h>

// VTK include
#include <vtkImageData.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtksys/SystemTools.hxx>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFreeSurferImporterModuleWidgetPrivate : public Ui_qSlicerFreeSurferImporterModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerFreeSurferImporterModuleWidget);
public:
  qSlicerFreeSurferImporterModuleWidgetPrivate(qSlicerFreeSurferImporterModuleWidget& object);

  void updateStatus(bool success, QString statusMessage = "");

  qSlicerFreeSurferImporterModuleWidget* q_ptr;
};

//-----------------------------------------------------------------------------
// qSlicerFreeSurferImporterModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModuleWidgetPrivate::qSlicerFreeSurferImporterModuleWidgetPrivate(qSlicerFreeSurferImporterModuleWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModuleWidgetPrivate::updateStatus(bool success, QString statusMessage/*=""*/)
{
  this->filesGroupBox->setEnabled(success);
  this->statusLabel->setText(statusMessage);
  if (!statusMessage.isEmpty())
  {
    qCritical() << statusMessage;
  }
}

//-----------------------------------------------------------------------------
// qSlicerFreeSurferImporterModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModuleWidget::qSlicerFreeSurferImporterModuleWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFreeSurferImporterModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerFreeSurferImporterModuleWidget::~qSlicerFreeSurferImporterModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModuleWidget::setup()
{
  Q_D(qSlicerFreeSurferImporterModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Erase status label
  d->statusLabel->setText("");

  QObject::connect(d->pathLineEdit, SIGNAL(currentPathChanged(QString)), this, SLOT(updateFileList()));
  QObject::connect(d->loadButton, SIGNAL(clicked()), this, SLOT(loadSelectedFiles()));
  QObject::connect(d->modelShowAllCheckBox, SIGNAL(clicked()), this, SLOT(updateFileList()));
  this->updateFileList();
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Superclass::setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModuleWidget::updateFileList()
{
  Q_D(qSlicerFreeSurferImporterModuleWidget);

  d->modelSelectorBox->clear();
  d->segmentationSelectorBox->clear();
  d->volumeSelectorBox->clear();
  d->scalarOverlaySelectorBox->clear();

  QString directory = d->pathLineEdit->currentPath();

  QDir mriDirectory(directory + "/mri");
  mriDirectory.setNameFilters(QStringList() << "*.mgz" << "*.cor" << "*.bshort");
  QStringList mgzFiles = mriDirectory.entryList();
  for (QString mgzFile : mgzFiles)
  {
    if (vtksys::SystemTools::GetFilenameExtension(mgzFile.toStdString()) == ".seg.mgz")
    {
      continue;
    }
    d->volumeSelectorBox->addItem(mgzFile);
  }

  QDir segDirectory(directory + "/mri");
  segDirectory.setNameFilters(QStringList() << "*seg*.mgz");
  QStringList segFiles = segDirectory.entryList();
  for (QString segFile : segFiles)
  {
    d->segmentationSelectorBox->addItem(segFile);
  }

  QDir surfDirectory(directory + "/surf");
  if (!d->modelShowAllCheckBox->isChecked())
  {
    surfDirectory.setNameFilters(QStringList() << "*h.white" << "*h.pial" << "*h.inflated" << "*h.sphere" << "*h.sphere.reg" << "*h.orig");
  }
  else
  {
    surfDirectory.setNameFilters(QStringList() << "*.*");
  }
  surfDirectory.setFilter(QDir::Files);
  QStringList surfFiles = surfDirectory.entryList();
  for (QString surfFile : surfFiles)
  {
    d->modelSelectorBox->addItem(surfFile);
  }

  QDir scalarDirectory(directory + "/surf");
  scalarDirectory.setNameFilters(QStringList() << "*.area*" << "*.curv*" << "*.sulc" << "*.W");
  QStringList scalarFiles = scalarDirectory.entryList();
  for (QString scalarFile : scalarFiles)
  {
    d->scalarOverlaySelectorBox->addItem(scalarFile);
  }

  d->updateStatus(true);
}

//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterModuleWidget::loadSelectedFiles()
{
  Q_D(qSlicerFreeSurferImporterModuleWidget);

  vtkSlicerApplicationLogic* applicationLogic = this->appLogic();
  if (applicationLogic)
  {
    applicationLogic->PauseRender();
  }

  qSlicerFreeSurferImporterModule* module = qobject_cast<qSlicerFreeSurferImporterModule*>(this->module());
  vtkSlicerFreeSurferImporterLogic* logic = vtkSlicerFreeSurferImporterLogic::SafeDownCast(module->logic());

  QString directory = d->pathLineEdit->currentPath();
  QString mriDirectory = directory + "/mri/";

  vtkMRMLVolumeNode* focusVolumeNode = nullptr;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QModelIndexList selectedVolumes = d->volumeSelectorBox->checkedIndexes();
  for (QModelIndex selectedVolume : selectedVolumes)
  {
    QString volumeName = d->volumeSelectorBox->itemText(selectedVolume.row());
    vtkMRMLVolumeNode* volumeNode = logic->LoadFreeSurferVolume((mriDirectory + volumeName).toStdString());
    if (!volumeNode)
    {
      d->updateStatus(true, "Could not load volume " + volumeName);
      continue;
    }

    // If we are loading a volume, then show the first volume in the slice view
    // when loading is complete.
    if (!focusVolumeNode)
    {
      focusVolumeNode = volumeNode;
    }
    d->volumeSelectorBox->setCheckState(selectedVolume, Qt::CheckState::Unchecked);
  }

  QModelIndexList selectedSegmentations = d->segmentationSelectorBox->checkedIndexes();
  for (QModelIndex selectedSegmentation : selectedSegmentations)
  {
    QString segmentationName = d->segmentationSelectorBox->itemText(selectedSegmentation.row());
    vtkMRMLSegmentationNode* segmentationNode = logic->LoadFreeSurferSegmentation((mriDirectory + segmentationName).toStdString());
    if (!segmentationNode)
    {
      d->updateStatus(true, "Could not load segmentation " + segmentationName);
      continue;
    }
    d->segmentationSelectorBox->setCheckState(selectedSegmentation, Qt::CheckState::Unchecked);
  }

  std::vector<vtkMRMLModelNode*> modelNodes;
  QString surfDirectory = directory + "/surf/";
  QModelIndexList selectedModels = d->modelSelectorBox->checkedIndexes();

  for (QModelIndex selectedModel : selectedModels)
  {
    QString modelName = d->modelSelectorBox->itemText(selectedModel.row());

    qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
    qSlicerIO::IOProperties properties;
    properties["fileName"] = surfDirectory + modelName;
    vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(ioManager->loadNodesAndGetFirst("FreeSurfer model", properties));
    if (!modelNode)
    {
      d->updateStatus(true, "Could not load surface " + modelName);
      continue;
    }
    d->modelSelectorBox->setCheckState(selectedModel, Qt::CheckState::Unchecked);
    modelNodes.push_back(modelNode);
  }

  vtkNew<vtkCollection> modelNodeCollection;
  for (vtkMRMLModelNode* modelNode : modelNodes)
  {
    modelNode->CreateDefaultDisplayNodes();
    modelNodeCollection->AddItem(modelNode);
  }

  QModelIndexList selectedScalarOverlays = d->scalarOverlaySelectorBox->checkedIndexes();
  for (QModelIndex selectedScalarOverlay : selectedScalarOverlays)
  {
    QString scalarOverlayName = d->scalarOverlaySelectorBox->itemText(selectedScalarOverlay.row());
    bool success = logic->LoadFreeSurferScalarOverlay((surfDirectory + scalarOverlayName).toStdString(), modelNodeCollection);
    if (!success)
    {
      d->updateStatus(true, "Could not load scalar overlay " + scalarOverlayName + "!");
      continue;
    }
    d->scalarOverlaySelectorBox->setCheckState(selectedScalarOverlay, Qt::CheckState::Unchecked);
  }

  if (focusVolumeNode)
  {
    // Show the focused volume in the slice views
    vtkMRMLSelectionNode* selectionNode =
      this->appLogic() ? this->appLogic()->GetSelectionNode() : nullptr;
    if (selectionNode)
    {
      selectionNode->SetActiveVolumeID(focusVolumeNode->GetID());
      if (this->appLogic())
      {
        this->appLogic()->PropagateVolumeSelection(); // includes FitSliceToAll by default
      }
    }
  }

  QApplication::restoreOverrideCursor();

  if (applicationLogic)
  {
    applicationLogic->ResumeRender();
  }
  return true;
}
