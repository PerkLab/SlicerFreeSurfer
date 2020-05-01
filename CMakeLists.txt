cmake_minimum_required(VERSION 3.13.4)

project(SlicerFreeSurfer)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://github.com/PerkLab/SlicerFreeSurfer")
set(EXTENSION_CATEGORY "Examples")
set(EXTENSION_CONTRIBUTORS "Kyle Sunderland (Perk Lab (Queen's University)), Andras Lasso (Perk Lab (Queen's University))")
set(EXTENSION_DESCRIPTION "Extension for handling FreeSurfer files in Slicer. Features importer for models, scalar overlay, and segmentations. Handles transformation of models to from FreeSurfer to Slicer coordinate system.")
set(EXTENSION_ICONURL "https://github.com/PerkLab/SlicerFreeSurfer/raw/master/SlicerFreeSurfer.png")
set(EXTENSION_SCREENSHOTURLS "https://github.com/PerkLab/SlicerFreeSurfer/raw/master/Screenshots/FreeSurferImporterInterface.png")
set(EXTENSION_DEPENDS "NA") # Specified as a space separated string, a list or 'NA' if any

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(FreeSurferImporter)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
