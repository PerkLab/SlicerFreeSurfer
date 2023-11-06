
#-----------------------------------------------------------------------------
set(MODULE_NAME FreeSurferImporter)
set(MODULE_TITLE "FreeSurfer Importer")

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(MRML)
add_subdirectory(Logic)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Current_{source,binary} and Slicer_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${FreeSurfer_INCLUDE_DIRS}
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}CurveReader.cxx
  qSlicer${MODULE_NAME}CurveReader.h
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  qSlicer${MODULE_NAME}PlaneReader.cxx
  qSlicer${MODULE_NAME}PlaneReader.h
  qSlicer${MODULE_NAME}ScalarOverlayReader.cxx
  qSlicer${MODULE_NAME}ScalarOverlayReader.h
  qSlicer${MODULE_NAME}ScalarOverlayOptionsWidget.cxx
  qSlicer${MODULE_NAME}ScalarOverlayOptionsWidget.h
  qSlicer${MODULE_NAME}SegmentationReader.cxx
  qSlicer${MODULE_NAME}SegmentationReader.h
  qSlicerXcedeCatalogReader.cxx
  qSlicerXcedeCatalogReader.h
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}CurveReader.h
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  qSlicer${MODULE_NAME}ScalarOverlayReader.h
  qSlicer${MODULE_NAME}ScalarOverlayOptionsWidget.h
  qSlicer${MODULE_NAME}PlaneReader.h
  qSlicer${MODULE_NAME}SegmentationReader.h
  qSlicerXcedeCatalogReader.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}ModuleWidget.ui
  Resources/UI/qSlicer${MODULE_NAME}ScalarOverlayOptionsWidget.ui
  )

set(MODULE_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  FreeSurfer
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

# --------------------------------------------------------------------------
# Install support files
# --------------------------------------------------------------------------
# Should this location change, update Libs\MRML\vtkMRMLFreeSurferProceduralColorNode.cxx

# Copy the resource files to the build directory

set(DATA_RESOURCE_DIR Resources/Data)
set(RESFILES
  FreeSurferColorLUT20060522.txt
  FreeSurferColorLUT20120827.txt
  FreeSurferColorLUT20150729.txt
  Simple_surface_labels2002.txt
  )
foreach(file ${RESFILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${DATA_RESOURCE_DIR}/${file}
    ${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_SHARE_DIR}/${MODULE_NAME}/${file}
    COPYONLY
    )

  install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DATA_RESOURCE_DIR}/${file}
    DESTINATION ${Slicer_INSTALL_QTLOADABLEMODULES_SHARE_DIR}/${MODULE_NAME}
    COMPONENT Runtime
    )
endforeach()

#-----------------------------------------------------------------------------
slicerMacroBuildLoadableModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
