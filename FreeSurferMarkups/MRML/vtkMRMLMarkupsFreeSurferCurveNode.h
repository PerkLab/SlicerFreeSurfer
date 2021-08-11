/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __vtkMRMLMarkupsFreeSurferCurveNode_h
#define __vtkMRMLMarkupsFreeSurferCurveNode_h

// MRML includes
#include "vtkMRMLDisplayableNode.h"
#include "vtkMRMLModelNode.h"

// Markups includes
#include "vtkSlicerMarkupsModuleMRMLExport.h"
#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLMarkupsCurveNode.h"

// VTK includes
#include <vtkStringArray.h>

// std includes
#include <vector>

class vtkFreeSurferCurveGenerator;

// export
#include "vtkSlicerFreeSurferMarkupsModuleMRMLExport.h"

/// \brief MRML node to represent a curve markup using FreeSurfer surface pathfinding
/// Curve Markups nodes contain N control points.
///
/// \ingroup Slicer_QtModules_Markups
class VTK_SLICER_FREESURFERMARKUPS_MODULE_MRML_EXPORT vtkMRMLMarkupsFreeSurferCurveNode : public vtkMRMLMarkupsCurveNode
{
public:
  static vtkMRMLMarkupsFreeSurferCurveNode *New();
  vtkTypeMacro(vtkMRMLMarkupsFreeSurferCurveNode, vtkMRMLMarkupsCurveNode);
  /// Print out the node information to the output stream
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //--------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------

  vtkMRMLNode* CreateNodeInstance() override;
  /// Get node XML tag name (like Volume, Model)
  const char* GetNodeTagName() override {return "MarkupsFreeSurferCurve";}

  /// Get markup name
  const char* GetMarkupType() override { return "FreeSurferCurve"; };

  /// Read node attributes from XML file
  void ReadXMLAttributes( const char** atts) override;

  /// Write this node's information to a MRML file in XML format.
  void WriteXML(ostream& of, int indent) override;

  /// Copy node content (excludes basic data, such as name and node references).
  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentMacro(vtkMRMLMarkupsFreeSurferCurveNode);

  void SetDistanceWeight(double weight);
  void SetCurvatureWeight(double weight);
  void SetSulcalHeightWeight(double weight);
  void SetDistanceCurvatureWeight(double weight);
  void SetDistanceSulcalHeightWeight(double weight);
  void SetCurvatureSulcalHeightWeight(double weight);
  void SetDistanceCurvatureSulcalHeightWeight(double weight);
  void SetDirectionWeight(double weight);
  void SetCurvaturePenalty(double weight);
  void SetSulcalHeightPenalty(double weight);
  void SetDistanceCurvaturePenalty(double weight);
  void SetDistanceSulcalHeightPenalty(double weight);
  void SetCurvatureSulcalHeightPenalty(double weight);
  void SetDistanceCurvatureSulcalHeightPenalty(double weight);
  void SetInvertScalars(bool invert);

  double GetDistanceWeight();
  double GetCurvatureWeight();
  double GetSulcalHeightWeight();
  double GetDistanceCurvatureWeight();
  double GetDistanceSulcalHeightWeight();
  double GetCurvatureSulcalHeightWeight();
  double GetDistanceCurvatureSulcalHeightWeight();
  double GetDirectionWeight();
  double GetCurvaturePenalty();
  double GetSulcalHeightPenalty();
  double GetDistanceCurvaturePenalty();
  double GetDistanceSulcalHeightPenalty();
  double GetCurvatureSulcalHeightPenalty();
  double GetDistanceCurvatureSulcalHeightPenalty();
  bool GetInvertScalars();

protected:
  vtkSmartPointer<vtkFreeSurferCurveGenerator> FreeSurferCurveGenerator;

protected:
  vtkMRMLMarkupsFreeSurferCurveNode();
  ~vtkMRMLMarkupsFreeSurferCurveNode() override;
  vtkMRMLMarkupsFreeSurferCurveNode(const vtkMRMLMarkupsFreeSurferCurveNode&);
  void operator=(const vtkMRMLMarkupsFreeSurferCurveNode&);
};

#endif
