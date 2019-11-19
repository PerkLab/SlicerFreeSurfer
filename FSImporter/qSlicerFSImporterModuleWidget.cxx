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

#include "qSlicerFSImporterModule.h"
#include "vtkSlicerFSImporterLogic.h"

// SlicerQt includes
#include "qSlicerFSImporterModuleWidget.h"
#include "ui_qSlicerFSImporterModuleWidget.h"

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
class qSlicerFSImporterModuleWidgetPrivate: public Ui_qSlicerFSImporterModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerFSImporterModuleWidget);
public:
  qSlicerFSImporterModuleWidgetPrivate(qSlicerFSImporterModuleWidget& object);

  vtkMRMLScalarVolumeNode* loadVolume(QString fsDirectory, QString name);
  vtkMRMLSegmentationNode* loadSegmentation(QString fsDirectory, QString name);
  vtkMRMLModelNode* loadModel(QString fsDirectory, QString name);

  void transformModelToRAS(vtkMRMLModelNode* surf, vtkMRMLScalarVolumeNode*orig);
  void applyFreeSurferSegmentationNames(vtkMRMLSegmentationNode* segmentation);

  void updateStatus(bool success, QString statusMessage = "");

  qSlicerFSImporterModuleWidget* q_ptr;
};

//-----------------------------------------------------------------------------
// qSlicerFSImporterModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFSImporterModuleWidgetPrivate::qSlicerFSImporterModuleWidgetPrivate(qSlicerFSImporterModuleWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void qSlicerFSImporterModuleWidgetPrivate::updateStatus(bool success, QString statusMessage/*=""*/)
{
  this->filesGroupBox->setEnabled(success);
  this->statusLabel->setText(statusMessage);
  if (!statusMessage.isEmpty())
    {
    qCritical() << statusMessage;
    }
}

//-----------------------------------------------------------------------------
// qSlicerFSImporterModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerFSImporterModuleWidget::qSlicerFSImporterModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerFSImporterModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerFSImporterModuleWidget::~qSlicerFSImporterModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerFSImporterModuleWidget::setup()
{
  Q_D(qSlicerFSImporterModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Erase status label
  d->statusLabel->setText("");

  QObject::connect(d->fsDirectoryButton, &ctkDirectoryButton::directoryChanged, this, &qSlicerFSImporterModuleWidget::updateFileList);
  QObject::connect(d->loadButton, &QPushButton::clicked, this, &qSlicerFSImporterModuleWidget::loadSelectedFiles);
  this->updateFileList();
}

//-----------------------------------------------------------------------------
void qSlicerFSImporterModuleWidget::updateFileList()
{
  Q_D(qSlicerFSImporterModuleWidget);

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
bool qSlicerFSImporterModuleWidget::loadSelectedFiles()
{
  Q_D(qSlicerFSImporterModuleWidget);
  QString directory = d->fsDirectoryButton->directory();

  QString mriDirectory = directory + "/mri/";
  vtkMRMLScalarVolumeNode* origNode = d->loadVolume(mriDirectory, "orig.mgz");
  if (!origNode)
    {
    d->updateStatus(true, "Could not find orig.mgz!");
    return false;
    }
  
  QModelIndexList selectedVolumes = d->volumeSelectorBox->checkedIndexes();
  for (QModelIndex selectedVolume : selectedVolumes)
    {
    QString volumeName = d->volumeSelectorBox->itemText(selectedVolume.row());
    vtkMRMLScalarVolumeNode* volumeNode = d->loadVolume(mriDirectory, volumeName);
    if (!volumeNode)
      {
      d->updateStatus(true, "Could not load surface " + volumeName + "!");
      }
    }

  QModelIndexList selectedSegmentations = d->segmentationSelectorBox->checkedIndexes();
  for (QModelIndex selectedSegmentation : selectedSegmentations)
    {
    QString segmentationName = d->segmentationSelectorBox->itemText(selectedSegmentation.row());
    vtkMRMLSegmentationNode* segmentationNode = d->loadSegmentation(mriDirectory, segmentationName);
    if (!segmentationNode)
      {
      d->updateStatus(true, "Could not load surface " + segmentationName + "!");
      }
    }

  std::vector<vtkMRMLModelNode*> modelNodes;
  QString surfDirectory = directory + "/surf/"; 
  QModelIndexList selectedModels = d->modelSelectorBox->checkedIndexes();
  for (QModelIndex selectedModel : selectedModels)
    {
    QString modelName = d->modelSelectorBox->itemText(selectedModel.row());

    vtkMRMLModelNode* modelNode = d->loadModel(surfDirectory, modelName);
    if (!modelNode)
      {
      d->updateStatus(true, "Could not load surface " + modelName + "!");
      continue;
      }
    d->transformModelToRAS(modelNode, origNode);
    modelNodes.push_back(modelNode);
    }

  for (vtkMRMLModelNode* modelNode : modelNodes)
    {
    modelNode->CreateDefaultDisplayNodes();
    }

  this->mrmlScene()->RemoveNode(origNode);
  return true;
}

//-----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* qSlicerFSImporterModuleWidgetPrivate::loadVolume(QString fsDirectory, QString name)
{
  Q_Q(qSlicerFSImporterModuleWidget);

  QString volumeFile = fsDirectory + name;
  if (!QFile::exists(volumeFile))
    {
    this->updateStatus(false, "Could not find " + volumeFile + "!");
    return nullptr;
    }

  std::string fileNameTemp = volumeFile.toStdString();
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(q->mrmlScene()->AddNewNodeByClass("vtkMRMLScalarVolumeNode"));

  std::string nodeName = name.toStdString();
  volumeNode->SetName(nodeName.c_str());
  volumeNode->AddDefaultStorageNode(fileNameTemp.c_str());
  
  vtkMRMLVolumeArchetypeStorageNode* volumeStorageNode = vtkMRMLVolumeArchetypeStorageNode::SafeDownCast(volumeNode->GetStorageNode());
  if (volumeStorageNode->ReadData(volumeNode))
    {
    volumeNode->CreateDefaultDisplayNodes();
    return volumeNode;
    }

  q->mrmlScene()->RemoveNode(volumeStorageNode);
  q->mrmlScene()->RemoveNode(volumeNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLSegmentationNode* qSlicerFSImporterModuleWidgetPrivate::loadSegmentation(QString fsDirectory, QString name)
{
  Q_Q(qSlicerFSImporterModuleWidget);

  QString segmentationFile = fsDirectory + name;
  if (!QFile::exists(segmentationFile))
  {
    qCritical("Could not find surf.");
    return nullptr;
  }

  std::string fileNameTemp = segmentationFile.toStdString();
  vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(q->mrmlScene()->AddNewNodeByClass("vtkMRMLSegmentationNode"));

  std::string nodeName = name.toStdString();
  segmentationNode->SetName(nodeName.c_str());
  segmentationNode->AddDefaultStorageNode(fileNameTemp.c_str());

  vtkMRMLSegmentationStorageNode* segmentationStorageNode = vtkMRMLSegmentationStorageNode::SafeDownCast(segmentationNode->GetStorageNode());
  if (segmentationStorageNode->ReadData(segmentationNode))
    {
    this->applyFreeSurferSegmentationNames(segmentationNode);
    return segmentationNode;
    }

  q->mrmlScene()->RemoveNode(segmentationStorageNode);
  q->mrmlScene()->RemoveNode(segmentationNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLModelNode* qSlicerFSImporterModuleWidgetPrivate::loadModel(QString fsDirectory, QString name)
{
  Q_Q(qSlicerFSImporterModuleWidget);

  QString surfFile = fsDirectory + name;
  if (!QFile::exists(surfFile))
    {
    qCritical("Could not find surf.");
    return nullptr;
    }

  std::string fileNameTemp = surfFile.toStdString();
  vtkMRMLModelNode* surfNode = vtkMRMLModelNode::SafeDownCast(q->mrmlScene()->AddNewNodeByClass("vtkMRMLModelNode"));

  std::string nodeName = name.toStdString();
  surfNode->SetName(nodeName.c_str());
  surfNode->AddDefaultStorageNode(fileNameTemp.c_str());

  vtkMRMLModelStorageNode* surfStorageNode = vtkMRMLModelStorageNode::SafeDownCast(surfNode->GetStorageNode());
  if (surfStorageNode->ReadData(surfNode))
    {
    return surfNode;
    }

  q->mrmlScene()->RemoveNode(surfStorageNode);
  q->mrmlScene()->RemoveNode(surfNode);
  return nullptr;
}

//-----------------------------------------------------------------------------
void qSlicerFSImporterModuleWidgetPrivate::transformModelToRAS(vtkMRMLModelNode* modelNode, vtkMRMLScalarVolumeNode* origVolumeNode)
{
  Q_Q(qSlicerFSImporterModuleWidget);
  if (!modelNode || !origVolumeNode)
    {
    return;
    }

  int extent[6] = { 0 };
  origVolumeNode->GetImageData()->GetExtent(extent);

  int dimensions[3] = { 0 };
  origVolumeNode->GetImageData()->GetDimensions(dimensions);

  double center[4] = { 0, 0, 0, 1 };
  for (int i = 0; i < 3; ++i)
    {
    center[i] = extent[2 * i] + std::ceil((dimensions[i] / 2.0));
    }

  vtkNew<vtkMatrix4x4> ijkToRAS;
  origVolumeNode->GetIJKToRASMatrix(ijkToRAS);
  ijkToRAS->MultiplyPoint(center, center); 

  vtkNew<vtkTransform> transform;
  transform->Translate(center);

  vtkNew<vtkTransformPolyDataFilter> transformer;
  transformer->SetTransform(transform);
  transformer->SetInputData(modelNode->GetPolyData());
  transformer->Update();
  modelNode->GetPolyData()->ShallowCopy(transformer->GetOutput());
  modelNode->GetPolyData()->Modified();
}

struct SegmentInfo
{
  std::string name = "Unknown";
  QColor color = QColor(100, 100, 100, 255);
};

//-----------------------------------------------------------------------------
void qSlicerFSImporterModuleWidgetPrivate::applyFreeSurferSegmentationNames(vtkMRMLSegmentationNode* segmentationNode)
{
  Q_Q(qSlicerFSImporterModuleWidget);
  if (!segmentationNode)
    {
    return;
    }
  MRMLNodeModifyBlocker blocker(segmentationNode);

  qSlicerFSImporterModule* module = qobject_cast<qSlicerFSImporterModule*>(q->module());
  vtkSlicerFSImporterLogic* logic = vtkSlicerFSImporterLogic::SafeDownCast(module->logic());
  std::string sharedDirectory = logic->GetModuleShareDirectory();
  std::string lutFilename = "FreeSurferColorLUT.txt";

  QString lutDirectory = QString::fromStdString(sharedDirectory + "/" + lutFilename);
  QFile lutFile(lutDirectory);
  if (!lutFile.exists())
    {
    return;
    }

  std::map<int, SegmentInfo> segmentInfoMap;

  QTextStream lutStream(&lutFile);
  if (lutFile.open(QIODevice::ReadOnly))
    {
    while (!lutStream.atEnd())
      {
        QString line = lutStream.readLine();
        line = line.trimmed();
        if (line.isEmpty())
          {
          continue;
          }
        if (line.at(0) == '#')
          {
          continue;
          }
        line = line.replace(QRegularExpression("[ ]+"), " ");

        QStringList tokens = line.split(" ");
        if (tokens.size() != 6)
          {
          continue;
          }

        int value = tokens[0].toInt();
        std::string name = tokens[1].toStdString();
        QColor color;
        color.setRed(tokens[2].toInt());
        color.setGreen(tokens[3].toInt());
        color.setBlue(tokens[4].toInt());
        color.setAlpha(tokens[5].toInt());

        SegmentInfo info;
        info.name = name;
        info.color = color;
        segmentInfoMap[value] = info;
      }
    lutFile.close();
    }

  vtkSegmentation* segmentation = segmentationNode->GetSegmentation();
  for (int i = 0; i < segmentation->GetNumberOfSegments(); ++i)
    {
    vtkSegment* segment = segmentation->GetNthSegment(i);
    SegmentInfo info = segmentInfoMap[segment->GetLabelValue()];
    segment->SetName(info.name.c_str());
    segment->SetColor(info.color.red()/255.0, info.color.green() / 255.0, info.color.blue() / 255.0);
    }
}