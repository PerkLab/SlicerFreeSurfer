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

#ifndef __qSlicerFreeSurferImporterModelReader_h
#define __qSlicerFreeSurferImporterModelReader_h

// SlicerFreeSurferImporter includes
#include "qSlicerFreeSurferImporterModuleExport.h"

// SlicerQt includes
#include "qSlicerFileReader.h"
class qSlicerFreeSurferImporterModelReaderPrivate;

// Slicer includes
class vtkSlicerFreeSurferImporterLogic;

//-----------------------------------------------------------------------------
class Q_SLICER_QTMODULES_FREESURFERIMPORTER_EXPORT qSlicerFreeSurferImporterModelReader
  : public qSlicerFileReader
{
  Q_OBJECT
public:
  typedef qSlicerFileReader Superclass;
  qSlicerFreeSurferImporterModelReader(vtkSlicerFreeSurferImporterLogic* logic, QObject* parent = 0);
  virtual ~qSlicerFreeSurferImporterModelReader();

  virtual QString description() const;
  virtual IOFileType fileType() const;
  virtual QStringList extensions() const;
  qSlicerIOOptions* options() const;

  virtual bool load(const IOProperties& properties);

  void setLogic(vtkSlicerFreeSurferImporterLogic* newFreeSurferImporterLogic);
  vtkSlicerFreeSurferImporterLogic* Logic() const;

protected:
  QScopedPointer< qSlicerFreeSurferImporterModelReaderPrivate > d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerFreeSurferImporterModelReader);
  Q_DISABLE_COPY(qSlicerFreeSurferImporterModelReader);
};

#endif
