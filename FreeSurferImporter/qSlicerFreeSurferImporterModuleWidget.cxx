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

// Qt includes
#include <QDebug>

#include "qSlicerFreeSurferImporterModule.h"
#include "vtkSlicerFreeSurferImporterLogic.h"

// SlicerQt includes
#include "qSlicerFreeSurferImporterModuleWidget.h"
#include "ui_qSlicerFreeSurferImporterModuleWidget.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLSegmentationNode.h>
#include <vtkMRMLSegmentationDisplayNode.h>
#include <vtkMRMLSegmentationStorageNode.h>

// VTK include
#include <vtkImageData.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFreeSurferImporterModuleWidgetPrivate: public Ui_qSlicerFreeSurferImporterModuleWidget
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
  : Superclass( _parent )
  , d_ptr( new qSlicerFreeSurferImporterModuleWidgetPrivate(*this) )
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

  QObject::connect(d->fsDirectoryButton, &ctkDirectoryButton::directoryChanged, this, &qSlicerFreeSurferImporterModuleWidget::updateFileList);
  QObject::connect(d->loadButton, &QPushButton::clicked, this, &qSlicerFreeSurferImporterModuleWidget::loadSelectedFiles);
  this->updateFileList();
}

//-----------------------------------------------------------------------------
void qSlicerFreeSurferImporterModuleWidget::updateFileList()
{
  Q_D(qSlicerFreeSurferImporterModuleWidget);

  d->modelSelectorBox->clear();
  d->segmentationSelectorBox->clear();
  d->volumeSelectorBox->clear();

  QString directory = d->fsDirectoryButton->directory();

  QString origFile = directory + "/mri/orig.mgz";
  if (!QFile::exists(origFile))
    {
    d->updateStatus(false, "Could not find orig.mgz!");
    return;
    }

  QDir mriDirectory(directory + "/mri");
  mriDirectory.setNameFilters(QStringList() << "*.mgz" << "*.cor" << "*.bshort");
  QStringList mgzFiles = mriDirectory.entryList();
  for (QString mgzFile : mgzFiles)
    {
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
  surfDirectory.setNameFilters(QStringList() << "*.white" << "*.pial");
  QStringList surfFiles = surfDirectory.entryList();
  for (QString surfFile : surfFiles)
    {
    d->modelSelectorBox->addItem(surfFile);
    }
  d->updateStatus(true);
}


//-----------------------------------------------------------------------------
bool qSlicerFreeSurferImporterModuleWidget::loadSelectedFiles()
{
  Q_D(qSlicerFreeSurferImporterModuleWidget);

  qSlicerFreeSurferImporterModule* module = qobject_cast<qSlicerFreeSurferImporterModule*>(this->module());
  vtkSlicerFreeSurferImporterLogic* logic = vtkSlicerFreeSurferImporterLogic::SafeDownCast(module->logic());

  QString directory = d->fsDirectoryButton->directory();
  QString mriDirectory = directory + "/mri/";
  vtkMRMLScalarVolumeNode* origNode = logic->loadFreeSurferVolume(mriDirectory.toStdString(), "orig.mgz");
  if (!origNode)
    {
    d->updateStatus(true, "Could not find orig.mgz!");
    return false;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  bool keepOrigNode = false;
  QModelIndexList selectedVolumes = d->volumeSelectorBox->checkedIndexes();
  for (QModelIndex selectedVolume : selectedVolumes)
    {
    QString volumeName = d->volumeSelectorBox->itemText(selectedVolume.row());
    if (volumeName == "orig.mgz")
      {
      keepOrigNode = true;
      if (origNode)
        {
        continue;
        }
      }
    vtkMRMLScalarVolumeNode* volumeNode = logic->loadFreeSurferVolume(mriDirectory.toStdString(), volumeName.toStdString());
    if (!volumeNode)
      {
      d->updateStatus(true, "Could not load surface " + volumeName + "!");
      continue;
      }
    d->segmentationSelectorBox->setCheckState(selectedVolume, Qt::CheckState::Unchecked);
    }

  QModelIndexList selectedSegmentations = d->segmentationSelectorBox->checkedIndexes();
  for (QModelIndex selectedSegmentation : selectedSegmentations)
    {
    QString segmentationName = d->segmentationSelectorBox->itemText(selectedSegmentation.row());
    vtkMRMLSegmentationNode* segmentationNode = logic->loadFreeSurferSegmentation(mriDirectory.toStdString(), segmentationName.toStdString());
    if (!segmentationNode)
      {
      d->updateStatus(true, "Could not load surface " + segmentationName + "!");
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

    vtkMRMLModelNode* modelNode = logic->loadFreeSurferModel(surfDirectory.toStdString(), modelName.toStdString());
    if (!modelNode)
      {
      d->updateStatus(true, "Could not load surface " + modelName + "!");
      continue;
      }
    d->segmentationSelectorBox->setCheckState(selectedModel, Qt::CheckState::Unchecked);
    logic->transformFreeSurferModelToRAS(modelNode, origNode);
    modelNodes.push_back(modelNode);
    }

  for (vtkMRMLModelNode* modelNode : modelNodes)
    {
    modelNode->CreateDefaultDisplayNodes();
    }

  if (!keepOrigNode)
    {
    this->mrmlScene()->RemoveNode(origNode);
    }

  QApplication::restoreOverrideCursor();

  return true;
}
