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

#ifndef __qSlicerFreeSurferImporterScalarOverlayOptionsWidget_h
#define __qSlicerFreeSurferImporterScalarOverlayOptionsWidget_h

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerIOOptionsWidget.h"

// SlicerFreeSurferImporter includes
#include "qSlicerFreeSurferImporterModuleExport.h"

class qSlicerFreeSurferImporterScalarOverlayOptionsWidgetPrivate;

class Q_SLICER_QTMODULES_FREESURFERIMPORTER_EXPORT qSlicerFreeSurferImporterScalarOverlayOptionsWidget
  : public qSlicerIOOptionsWidget
{
  Q_OBJECT
public:
  typedef qSlicerIOOptionsWidget Superclass;
  qSlicerFreeSurferImporterScalarOverlayOptionsWidget(QWidget *parent=nullptr);
  ~qSlicerFreeSurferImporterScalarOverlayOptionsWidget() override;

  bool isValid()const override;

protected slots:
  void updateProperties();

private:
  Q_DECLARE_PRIVATE_D(qGetPtrHelper(qSlicerIOOptions::d_ptr), qSlicerFreeSurferImporterScalarOverlayOptionsWidget);
  Q_DISABLE_COPY(qSlicerFreeSurferImporterScalarOverlayOptionsWidget);
};

#endif
