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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerFSImporterFooBarWidget.h"
#include "ui_qSlicerFSImporterFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_FSImporter
class qSlicerFSImporterFooBarWidgetPrivate
  : public Ui_qSlicerFSImporterFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerFSImporterFooBarWidget);
protected:
  qSlicerFSImporterFooBarWidget* const q_ptr;

public:
  qSlicerFSImporterFooBarWidgetPrivate(
    qSlicerFSImporterFooBarWidget& object);
  virtual void setupUi(qSlicerFSImporterFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerFSImporterFooBarWidgetPrivate
::qSlicerFSImporterFooBarWidgetPrivate(
  qSlicerFSImporterFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerFSImporterFooBarWidgetPrivate
::setupUi(qSlicerFSImporterFooBarWidget* widget)
{
  this->Ui_qSlicerFSImporterFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerFSImporterFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerFSImporterFooBarWidget
::qSlicerFSImporterFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerFSImporterFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerFSImporterFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerFSImporterFooBarWidget
::~qSlicerFSImporterFooBarWidget()
{
}
