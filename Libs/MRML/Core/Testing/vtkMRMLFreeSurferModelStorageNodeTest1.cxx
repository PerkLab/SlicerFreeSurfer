/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLFreeSurferModelStorageNode.h"
#include "vtkURIHandler.h"


#include "TestingMacros.h"

int vtkMRMLFreeSurferModelStorageNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLFreeSurferModelStorageNode > node1 = vtkSmartPointer< vtkMRMLFreeSurferModelStorageNode >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  EXERCISE_BASIC_STORAGE_MRML_METHODS(vtkMRMLFreeSurferModelStorageNode, node1);

  return EXIT_SUCCESS;
}
