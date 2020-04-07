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

#ifndef __qSlicerFreeSurferImporterScalarOverlayReader_h
#define __qSlicerFreeSurferImporterScalarOverlayReader_h

// SlicerFreeSurferImporter includes
#include "qSlicerFreeSurferImporterModuleExport.h"

// SlicerQt includes
#include "qSlicerFileReader.h"

class qSlicerFreeSurferImporterScalarOverlayReaderPrivate;
class vtkSlicerFreeSurferImporterLogic;

//-----------------------------------------------------------------------------
class Q_SLICER_QTMODULES_FREESURFERIMPORTER_EXPORT qSlicerFreeSurferImporterScalarOverlayReader
  : public qSlicerFileReader
{
  Q_OBJECT
public:
  typedef qSlicerFileReader Superclass;
  qSlicerFreeSurferImporterScalarOverlayReader(vtkSlicerFreeSurferImporterLogic* logic, QObject* parent = nullptr);
  ~qSlicerFreeSurferImporterScalarOverlayReader() override;

  void setLogic(vtkSlicerFreeSurferImporterLogic* newFreeSurferImporterLogic);
  vtkSlicerFreeSurferImporterLogic* Logic() const;

  QString description()const override;
  IOFileType fileType()const override;
  QStringList extensions()const override;
  qSlicerIOOptions* options()const override;

  bool load(const IOProperties& properties) override;

protected:
  QScopedPointer<qSlicerFreeSurferImporterScalarOverlayReaderPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerFreeSurferImporterScalarOverlayReader);
  Q_DISABLE_COPY(qSlicerFreeSurferImporterScalarOverlayReader);
};

#endif
