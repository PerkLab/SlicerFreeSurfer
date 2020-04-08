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
and was supported through CANARIE's Research Software Program, and Cancer
Care Ontario.

==============================================================================*/

#ifndef __qSlicerFreeSurferImporterSegmentationReader_h
#define __qSlicerFreeSurferImporterSegmentationReader_h

// SlicerFreeSurferImporter includes
#include "qSlicerFreeSurferImporterModuleExport.h"

// SlicerQt includes
#include "qSlicerFileReader.h"
class qSlicerFreeSurferImporterSegmentationReaderPrivate;

// Slicer includes
class vtkSlicerFreeSurferImporterLogic;

//-----------------------------------------------------------------------------
class Q_SLICER_QTMODULES_FREESURFERIMPORTER_EXPORT qSlicerFreeSurferImporterSegmentationReader
  : public qSlicerFileReader
{
  Q_OBJECT
public:
  typedef qSlicerFileReader Superclass;
  qSlicerFreeSurferImporterSegmentationReader(vtkSlicerFreeSurferImporterLogic* logic, QObject* parent = 0);
  virtual ~qSlicerFreeSurferImporterSegmentationReader();

  virtual QString description() const;
  virtual IOFileType fileType() const;
  virtual QStringList extensions() const;

  virtual bool load(const IOProperties& properties);

  void setLogic(vtkSlicerFreeSurferImporterLogic* newFreeSurferImporterLogic);
  vtkSlicerFreeSurferImporterLogic* Logic() const;

protected:
  QScopedPointer< qSlicerFreeSurferImporterSegmentationReaderPrivate > d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerFreeSurferImporterSegmentationReader);
  Q_DISABLE_COPY(qSlicerFreeSurferImporterSegmentationReader);
};

#endif
