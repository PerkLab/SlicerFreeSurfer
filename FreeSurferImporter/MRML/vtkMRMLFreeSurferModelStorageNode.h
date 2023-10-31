/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLFreeSurferModelStorageNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/

#ifndef __vtkMRMLFreeSurferModelStorageNode_h
#define __vtkMRMLFreeSurferModelStorageNode_h

#include "vtkMRMLModelStorageNode.h"
#include "vtkSlicerFreeSurferImporterModuleMRMLExport.h"

/// \brief MRML node for model storage on disk.
///
/// Storage nodes has methods to read/write vtkPolyData to/from disk
/// It uses the superclass vtkMRMLModelStorageNode for writing
/// (same file format supported).
class VTK_SLICER_FREESURFERIMPORTER_MODULE_MRML_EXPORT vtkMRMLFreeSurferModelStorageNode : public vtkMRMLModelStorageNode
{
public:
  static vtkMRMLFreeSurferModelStorageNode* New();
  vtkTypeMacro(vtkMRMLFreeSurferModelStorageNode, vtkMRMLModelStorageNode);

  vtkMRMLNode* CreateNodeInstance() override;

  ///
  /// Get node XML tag name (like Storage, Model)
  const char* GetNodeTagName() override { return "FreeSurferModelStorage"; };

  static const char* GetFreeSurferFileTypeAttributeName();

protected:
  vtkMRMLFreeSurferModelStorageNode();
  ~vtkMRMLFreeSurferModelStorageNode() override;
  vtkMRMLFreeSurferModelStorageNode(const vtkMRMLFreeSurferModelStorageNode&);
  void operator=(const vtkMRMLFreeSurferModelStorageNode&);
};

#endif



