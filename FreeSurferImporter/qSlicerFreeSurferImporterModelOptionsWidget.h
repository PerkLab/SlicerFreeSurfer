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

#ifndef __qSlicerFreeSurferImporterModelOptionsWidget_h
#define __qSlicerFreeSurferImporterModelOptionsWidget_h

// CTK includes
#include <ctkPimpl.h>

// SlicerFreeSurferImporter includes
#include "qSlicerFreeSurferImporterModuleExport.h"

// SlicerQt includes
#include "qSlicerIOOptionsWidget.h"

class qSlicerFreeSurferImporterModelOptionsWidgetPrivate;

class Q_SLICER_QTMODULES_FREESURFERIMPORTER_EXPORT qSlicerFreeSurferImporterModelOptionsWidget
  : public qSlicerIOOptionsWidget
{
  Q_OBJECT
public:
  typedef qSlicerIOOptionsWidget Superclass;
  qSlicerFreeSurferImporterModelOptionsWidget(QWidget *parent=nullptr);
  ~qSlicerFreeSurferImporterModelOptionsWidget() override;

protected slots:
  void updateProperties();

private:
  Q_DECLARE_PRIVATE_D(qGetPtrHelper(qSlicerIOOptions::d_ptr), qSlicerFreeSurferImporterModelOptionsWidget);
  Q_DISABLE_COPY(qSlicerFreeSurferImporterModelOptionsWidget);
};

#endif
