/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLFreeSurferModelStorageNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/


#include "vtkObjectFactory.h"
#include "vtkMRMLFreeSurferModelStorageNode.h"

const char* FREESURFER_FILE_TYPE_ATTRIBUTE = "FreeSurfer.FileType";

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLFreeSurferModelStorageNode);

//----------------------------------------------------------------------------
vtkMRMLFreeSurferModelStorageNode::vtkMRMLFreeSurferModelStorageNode() = default;

//----------------------------------------------------------------------------
vtkMRMLFreeSurferModelStorageNode::~vtkMRMLFreeSurferModelStorageNode() = default;

//------------------------------------------------------------------------------
const char* vtkMRMLFreeSurferModelStorageNode::GetFreeSurferFileTypeAttributeName()
{
  return FREESURFER_FILE_TYPE_ATTRIBUTE;
}