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
  Care Ontario, OpenAnatomy, and Brigham and Women’s Hospital through NIH grant R01MH112748.

==============================================================================*/

#ifndef __vtkSlicerFreeSurferDijkstraGraphGeodesicPath_h
#define __vtkSlicerFreeSurferDijkstraGraphGeodesicPath_h

// Markups MRML includes
#include <vtkSlicerDijkstraGraphGeodesicPath.h>

// export
#include "vtkSlicerFreeSurferMarkupsModuleMRMLExport.h"

/// Filter that generates curves between points of an input polydata
class VTK_SLICER_FREESURFERMARKUPS_MODULE_MRML_EXPORT vtkSlicerFreeSurferDijkstraGraphGeodesicPath : public vtkSlicerDijkstraGraphGeodesicPath
{
public:
  vtkTypeMacro(vtkSlicerFreeSurferDijkstraGraphGeodesicPath, vtkDijkstraGraphGeodesicPath);
  static vtkSlicerFreeSurferDijkstraGraphGeodesicPath* New();
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// The weighting applied to the cost function distance component
  /// wd from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceWeight, double);
  vtkGetMacro(DistanceWeight, double);

  /// The weighting applied to the cost function curvature component
  /// wc from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(CurvatureWeight, double);
  vtkGetMacro(CurvatureWeight, double);

  /// The weighting applied to the cost function sulcal height component
  /// wh from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(SulcalHeightWeight, double);
  vtkGetMacro(SulcalHeightWeight, double);

  /// The weighting applied to the cost function distance*curvature component
  /// wdc from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceCurvatureWeight, double);
  vtkGetMacro(DistanceCurvatureWeight, double);

  /// The weighting applied to the cost function distance*sulcal height component
  /// wdh from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceSulcalHeightWeight, double);
  vtkGetMacro(DistanceSulcalHeightWeight, double);

  /// The weighting applied to the cost function curvature*sulcal height component
  /// wch from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(CurvatureSulcalHeightWeight, double);
  vtkGetMacro(CurvatureSulcalHeightWeight, double);

  /// The weighting applied to the cost function distance*curvature*sulcal height component
  /// wdch from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceCurvatureSulcalHeightWeight, double);
  vtkGetMacro(DistanceCurvatureSulcalHeightWeight, double);

  /// The weighting applied to the cost function direction component
  /// wdir from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DirectionWeight, double);
  vtkGetMacro(DirectionWeight, double);

  /// The penalty applied to the cost function curvature component
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(CurvaturePenalty, double);
  vtkGetMacro(CurvaturePenalty, double);

  /// The penalty applied to the cost function sulcal height component
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(SulcalHeightPenalty, double);
  vtkGetMacro(SulcalHeightPenalty, double);

  /// The penalty applied to the cost function distance*curvature component
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceCurvaturePenalty, double);
  vtkGetMacro(DistanceCurvaturePenalty, double);

  /// The penalty applied to the cost function distance*sulcal height component
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceSulcalHeightPenalty, double);
  vtkGetMacro(DistanceSulcalHeightPenalty, double);

  /// The penalty applied to the cost function curvature*sulcal height component
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(CurvatureSulcalHeightPenalty, double);
  vtkGetMacro(CurvatureSulcalHeightPenalty, double);

  /// The penalty applied to the cost function distance*curvature*sulcal height component
  /// wdch from FreeSurfer
  /// \sa CalculateDynamicEdgeCost()
  vtkSetMacro(DistanceCurvatureSulcalHeightPenalty, double);
  vtkGetMacro(DistanceCurvatureSulcalHeightPenalty, double);

protected:
  /// The fixed cost going from vertex u to v.
  /// Reimplemented to provide additonal cost function types.
  /// \sa SetCostFunctionType()
  double CalculateStaticEdgeCost(vtkDataSet* inData, vtkIdType u, vtkIdType v) override;

  /// The fixed cost going from vertex u to v.
  /// Implements the FreeSurfer cost function found here:
  /// https://github.com/freesurfer/freesurfer/blob/4db941ef298c0ac5fb78c29fd0e95571ac363e16/mris_pmake/env.cpp#L2017
  double CalculateDynamicEdgeCost(vtkDataSet* inData, vtkIdType u, vtkIdType v) override;

protected:
  vtkSlicerFreeSurferDijkstraGraphGeodesicPath();
  ~vtkSlicerFreeSurferDijkstraGraphGeodesicPath() override;
  vtkSlicerFreeSurferDijkstraGraphGeodesicPath(const vtkSlicerFreeSurferDijkstraGraphGeodesicPath&) = delete;
  void operator=(const vtkSlicerFreeSurferDijkstraGraphGeodesicPath&) = delete;

  double DistanceWeight;
  double CurvatureWeight;
  double SulcalHeightWeight;
  double DistanceCurvatureWeight;
  double DistanceSulcalHeightWeight;
  double CurvatureSulcalHeightWeight;
  double DistanceCurvatureSulcalHeightWeight;
  double DirectionWeight;

  double CurvaturePenalty;
  double SulcalHeightPenalty;
  double DistanceCurvaturePenalty;
  double DistanceSulcalHeightPenalty;
  double CurvatureSulcalHeightPenalty;
  double DistanceCurvatureSulcalHeightPenalty;
};

#endif
