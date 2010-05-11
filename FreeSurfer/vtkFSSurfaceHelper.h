/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkFSSurfaceHelper.h,v $
  Date:      $Date: 2006/05/26 19:40:14 $
  Version:   $Revision: 1.9 $

=========================================================================auto=*/
/// .NAME vtkFSSurfaceHelper - Provides tools
/// .SECTION Description
/// Utility functions associated to FreeSurfer data.

#ifndef __vtkFSSurfaceHelper_h
#define __vtkFSSurfaceHelper_h

#include <vtkObject.h>
#include "vtkFreeSurferWin32Header.h"
class vtkMatrix4x4;

class VTK_FreeSurfer_EXPORT vtkFSSurfaceHelper: public vtkObject
{
public:
  static vtkFSSurfaceHelper* New();
  /// Description:
  /// Convenience method to compute a volume's Vox2RAS-tkreg Matrix
  static void ComputeTkRegVox2RASMatrix(double* spacing, int* dimensions, vtkMatrix4x4 *M );

  /// Description:
  /// Computes matrix we need to register V1Node to V2Node given the
  /// "register.dat" matrix from tkregister2 (FreeSurfer)
  static void TranslateFreeSurferRegistrationMatrixIntoSlicerRASToRASMatrix( 
    double* V1Spacing, int* V1Dim, vtkMatrix4x4* V1IJKToRASMatrix,
    double* V2Spacing, int* V2Dim, vtkMatrix4x4* V2RASToIJKMatrix,
    vtkMatrix4x4 *FSRegistrationMatrix, vtkMatrix4x4 *RAS2RASMatrix);
protected:
  vtkFSSurfaceHelper();
};

#endif
