import logging
import os
import vtk
import slicer
from slicer.ScriptedLoadableModule import *
from operator import itemgetter
import numpy as np
from dataclasses import dataclass

#
# NiBabelModelIO
#

class NiBabelModelIO(ScriptedLoadableModule):
    """Uses ScriptedLoadableModule base class, available at:
    https://github.com/Slicer/Slicer/blob/main/Base/Python/slicer/ScriptedLoadableModule.py
    """

    def __init__(self, parent):
        ScriptedLoadableModule.__init__(self, parent)
        self.parent.title = "NiBabelModelIO"  # TODO: make this more human readable by adding spaces
        self.parent.categories = ["Examples"]  # TODO: set categories (folders where the module shows up in the module selector)
        self.parent.dependencies = []
        self.parent.contributors = ["Kyle Sunderland (Perk Lab, Queen's University)"]
        self.parent.helpText = """""" # Not displayed to the user.
        # TODO: replace with organization, grant and thanks
        self.parent.acknowledgementText = """
This file was originally developed by Kyle Sunderland (Perk Lab, Queen's University), and was partially funded by Brigham and Women's Hospital through NIH grant R01MH112748
"""
        self.parent.hidden = True # Do not show module UI

        # Additional initialization step after application startup is complete
        slicer.app.connect("startupCompleted()", registerSampleData)

#
# Register sample data sets in Sample Data module
#

def registerSampleData():
    """
    Add data sets to Sample Data module.
    """
    # TODO: Add FreeSurfer sample data
    pass


@dataclass
class VolGeom:
    """
    From: https://github.com/nipy/NiBabel/discussions/1249
    """
    # Using tuples to show shape. Will mostly be numpy arrays.
    shape: tuple[int, int, int]
    zooms: tuple[float, float, float]
    cosines: tuple[tuple[float, float, float], tuple[float, float, float], tuple[float, float, float]]
    c_ras: tuple[float, float, float]

    @classmethod
    def from_mghheader(cls, header):
        return cls(
            shape=header.dims[:3],
            zooms=header.delta,
            cosines=header.Mdc.T,
            c_ras=header.Pxyz_c,
        )

    @classmethod
    def from_surf_footer(cls, footer):
        return cls(
            shape=footer['volume'],
            zooms=footer['voxelsize'],
            cosines=np.vstack(itemgetter('xras', 'yras', 'zras')(footer)).T,
            c_ras=footer['cras'],
        )

    @classmethod
    def from_gifti_metadata(cls, meta):
        return cls(
            shape=np.int16((meta['VolGeomWidth'], meta['VolGeomHeight'], meta['VolGeomDepth'])),
            zooms=np.float64((meta['VolGeomXsize'], meta['VolGeomYsize'], meta['VolGeomZsize'])),
            cosines=np.float64(
                itemgetter(*(f'VolGeom{col}_{row}' for row in 'RAS' for col in 'XYZ'))(meta)
            ).reshape((3, 3)),
            c_ras=np.float64(meta['VolGeomC_R'], meta['VolGeomC_A'], meta['VolGeomC_S']),
        )

    def tkreg_affine(self):
        tkrcosines = np.array([[-1, 0, 0], [0, 0, 1], [0, -1, 0]])
        mat = tkrcosines * self.zooms
        import nibabel as nb
        return nb.affines.from_matvec(mat, -mat @ self.shape / 2)

    def scanner_affine(self):
        mat = self.cosines * self.zooms
        import nibabel as nb
        return nb.affines.from_matvec(mat, self.c_ras - mat @ self.shape / 2)

    def tkreg2scanner(self):
        return self.scanner_affine() @ np.linalg.inv(self.tkreg_affine())

#
# NiBabelModelIOLogic
#

class NiBabelModelIOLogic(ScriptedLoadableModuleLogic):
    """This class should implement all the actual
    computation done by your module.  The interface
    should be such that other python code can import
    this class and make use of the functionality without
    requiring an instance of the Widget.
    Uses ScriptedLoadableModuleLogic base class, available at:
    https://github.com/Slicer/Slicer/blob/main/Base/Python/slicer/ScriptedLoadableModule.py
    """

    def __init__(self) -> None:
        """
        Called when the logic class is instantiated. Can be used for initializing member variables.
        """
        ScriptedLoadableModuleLogic.__init__(self)

        try:
            import nibabel
        except ImportError:
            slicer.util.pip_install("NiBabel")

    @vtk.calldata_type(vtk.VTK_OBJECT)
    def onNodeAdded(self, caller, event, node):
        if node.IsA("vtkMRMLFreeSurferModelStorageNode"):
            self.addObserver(node, node.ReadDataRequestedEvent, self.onReadDataRequested)

    @vtk.calldata_type(vtk.VTK_OBJECT)
    def onNodeRemoved(self, caller, event, node):
        if node.IsA("vtkMRMLFreeSurferModelStorageNode"):
            self.removeObserver(node, self.onReadDataRequested)

    def updateNodeObservers(self, caller=None, event=None):
        self.removeObservers(self.onReadDataRequested)
        for node in slicer.util.getNodesByClass("vtkMRMLFreeSurferModelStorageNode"):
            self.addObserver(node, node.ReadDataRequestedEvent, self.onReadDataRequested)

    @vtk.calldata_type(vtk.VTK_OBJECT)
    def onReadDataRequested(self, storageNode, event, modelNode):
        self.readData(storageNode, modelNode)
    
    def createAndReadModelNode(self, filepath, baseName="", calculateNormals=True):
        """
        Reads the FreeSurfer surface data, and adds it to a new model node.
        """
        if len(baseName) == 0:
            nameAndExtension = os.path.splitext(os.path.basename(filepath))
            baseName = f"{nameAndExtension[0]}_{nameAndExtension[1][1:]}"
        baseName = slicer.mrmlScene.GenerateUniqueName(baseName)
        modelNode = slicer.mrmlScene.AddNewNodeByClass("vtkMRMLModelNode", baseName)
        success = self.readModelNode(filepath, modelNode, calculateNormals)
        if not success:
            slicer.mrmlScene.RemoveNode(modelNode)
            return None
        return modelNode
    
    def readModelNode(self, path, modelNode, calculateNormals=True):
        """
        Reads the FreeSurfer surface into the model node.
        """
        import nibabel as nb

        try:
            points, polys, metadata = nb.freesurfer.io.read_geometry(path, True)

            geom = VolGeom.from_surf_footer(metadata)

            points = nb.affines.apply_affine(geom.tkreg2scanner(), points).astype('f4')
        
            polyData = vtk.vtkPolyData()
            pointsVTK = vtk.vtkPoints()
            pointsVTK.SetData(vtk.util.numpy_support.numpy_to_vtk(points))
            polyData.SetPoints(pointsVTK)
        
            cellArray = vtk.vtkCellArray()
            polys = polys.astype(np.int64)
            polys = polys.flatten()
            idTypeArray =  vtk.util.numpy_support.numpy_to_vtkIdTypeArray(polys, deep=True)
            cellArray.SetData(3, idTypeArray)
            polyData.SetPolys(cellArray)

            if calculateNormals:
                normals = vtk.vtkPolyDataNormals()
                normals.SetInputData(polyData)
                normals.ComputePointNormalsOn()
                normals.SplittingOff()
                normals.ConsistencyOn()
                normals.AutoOrientNormalsOn()
                normals.Update()
                polyData = normals.GetOutput()

            modelNode.SetAndObservePolyData(polyData)
        
        except:
            import traceback
            traceback.print_exc()
            return False
        return True

#
# NiBabelModelIOTest
#
class NiBabelModelIOTest(ScriptedLoadableModuleTest):
    """
    This is the test case for your scripted module.
    Uses ScriptedLoadableModuleTest base class, available at:
    https://github.com/Slicer/Slicer/blob/main/Base/Python/slicer/ScriptedLoadableModule.py
    """

    def setUp(self):
        """ Do whatever is needed to reset the state - typically a scene clear will be enough.
        """
        slicer.mrmlScene.Clear()

    def runTest(self):
        """Run as few or as many tests as needed here.
        """
        self.setUp()
        self.test_NiBabelModelIO1()

    def test_NiBabelModelIO1(self):
        """ Ideally you should have several levels of tests.  At the lowest level
        tests should exercise the functionality of the logic with different inputs
        (both valid and invalid).  At higher levels your tests should emulate the
        way the user would interact with your code and confirm that it still works
        the way you intended.
        One of the most important features of the tests is that it should alert other
        developers when their changes will have an impact on the behavior of your
        module.  For example, if a developer removes a feature that you depend on,
        your test should break so they know that the feature is needed.
        """

        self.delayDisplay("Starting the test")
        self.delayDisplay('Test passed')

class NiBabelModelIOFileReader:

    def __init__(self, parent):
        self.parent = parent

    def description(self):
        return 'FreeSurfer model'

    def fileType(self):
        return 'FreeSurfer model'

    def extensions(self):
        modelExtensions = ['orig', 'inflated', 'sphere', 'white', 'smoothwm', 'pial']
        modelExtensionsString = 'FreeSurfer model ('
        for extension in modelExtensions:
            modelExtensionsString += f'*.{extension} '
        modelExtensionsString += ')'
        return [modelExtensionsString]

    def canLoadFile(self, filePath):
        return True

    def load(self, properties):
        try:
            filePath = properties['fileName']

            # Get node base name from filename
            baseName = ""
            if 'name' in properties.keys():
                baseName = properties['name']

            # Read file content
            logic = NiBabelModelIOLogic()
            loadedNode = logic.createAndReadModelNode(filePath, baseName)

            # Uncomment the next line to display a warning message to the user.
            if loadedNode is None:
                self.parent.userMessages().AddMessage(vtk.vtkCommand.ErrorEvent, "Could not load file: " + filePath)
                return

        except Exception as e:
            import traceback
            traceback.print_exc()
            errorMessage = f"Failed to read file: {str(e)}"
            self.parent.userMessages().AddMessage(vtk.vtkCommand.ErrorEvent, errorMessage)
            return False

        loadedNode.CreateDefaultDisplayNodes()
        self.parent.loadedNodes = [loadedNode.GetID()]
        return True
