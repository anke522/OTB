#
# Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
#
# This file is part of Orfeo Toolbox
#
#     https://www.orfeo-toolbox.org/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(BASELINE ${OTB_DATA_ROOT}/Baseline/Examples/IO)
set(INPUTDATA ${OTB_DATA_ROOT}/Examples)

# ------- ExtractROITest----------

otb_add_test(NAME ioTeExtractROITest COMMAND ${OTB_TEST_DRIVER}
  --compare-n-images ${NOTOL} 2
    ${BASELINE}/ROI_IMAGERY_RGB.png
    ${TEMP}/ROI_IMAGERY_RGB.png
    ${BASELINE}/ROI_IMAGERY_MIR.png
    ${TEMP}/ROI_IMAGERY_MIR.png
  Execute $<TARGET_FILE:ExtractROI>
    ${INPUTDATA}/IMAGERY_SSECH.tif
    ${TEMP}/ROI_IMAGERY_RGB.png
    ${TEMP}/ROI_IMAGERY_MIR.png
    0 0 100 100
)

otb_add_test(NAME ioTeDEMToImageGeneratorTest COMMAND ${OTB_TEST_DRIVER}
  --compare-image ${EPSILON_5}
    ${BASELINE}/DEMToImageGenerator.tif
    ${TEMP}/DEMToImageGenerator.tif
  Execute $<TARGET_FILE:DEMToImageGenerator>
    ${TEMP}/DEMToImageGenerator.tif
    ${TEMP}/pretty_DEMToImageGenerator.png
    6.5
    45.5
    500
    500
    0.002
    -0.002
    ${INPUTDATA}/DEM_srtm
)

otb_add_test(NAME prTeDEMHandlerExampleTest COMMAND ${OTB_TEST_DRIVER}
  Execute $<TARGET_FILE:DEMHandlerExample>
    ${INPUTDATA}/DEM_srtm
    ${INPUTDATA}/DEM_srtm/egm96.grd
    40
    8.434583
    44.647083
    383.580313671
    0.001
)

if(OTBlibkml_LOADED)
otb_add_test(NAME ioTeImageToKmzAndMapFileProductExample COMMAND ${OTB_TEST_DRIVER}
  Execute $<TARGET_FILE:ImageToKmzAndMapFileProductExample>
    ${OTB_DATA_ROOT}/Input/Haiti1_sr10.jpeg
    ${TEMP}/ioTeHaitiKmzProduct.kmz
    ${TEMP}/mapserverExample/ioTeHaitiMapFileProduct.map
    ${TEMP}/mapserverExample/shapeindex
    http://localhost/cgi-bin/mapserv
    ${INPUTDATA}/DEM_srtm
    198.000000 199.000000 -72.351257 18.555075 0.000000
    196.000000 194.000000 -72.352051 18.556946 0.000000
    208.000000 157.000000 -72.346130 18.574358 0.000000
    215.000000 143.000000 -72.342377 18.580441 0.000000
    243.000000 141.000000 -72.328964 18.581335 0.000000
    256.000000 93.000000  -72.322571 18.602974 0.000000
    349.000000 140.000000 -72.278343 18.580887 0.000000
    289.000000 144.000000 -72.306908 18.579464 0.000000
    210.000000 247.000000 -72.345741 18.532839 0.000000
    194.000000 247.000000 -72.353317 18.532595 0.00000
    164.000000 241.000000 -72.367691 18.535810 0.000000
    144.000000 245.000000 -72.377457 18.534243 0.000000
    69.000000  201.000000 -72.413315 18.554974 0.000000
    74.000000  237.000000 -72.410934 18.538496 0.000000
    86.000000  280.000000 -72.405739 18.518456 0.000000
    193.000000 286.000000 -72.353920 18.515240 0.000000
    334.000000 282.000000 -72.286240 18.515160 0.000000
    373.000000 177.000000 -72.267570 18.563681 0.000000
    368.000000 158.000000 -72.269203 18.572529 0.000000
    350.000000 93.000000  -72.282402 18.602425 0.000000
    343.000000 86.000000  -72.280792 18.605862 0.000000
    282.000000 71.000000  -72.309692 18.613203 0.000000
    189.000000 36.000000  -72.354301 18.629776 0.000000
    399.000000 264.000000 -72.255020 18.523664 0.000000
    42.000000  232.000000 -72.426552 18.540876 0.000000
)
set_property(TEST ioTeImageToKmzAndMapFileProductExample PROPERTY RESOURCE_LOCK web_access)
endif()

if(OTB_USE_DEPRECATED)
    otb_add_test(NAME ioTeTileMapImageIOExampleTest COMMAND ${OTB_TEST_DRIVER}
  --compare-image 10.0
    ${BASELINE}/openStreetMap.png
    ${TEMP}/openStreetMap.png
  Execute $<TARGET_FILE:TileMapImageIOExample>
    ${INPUTDATA}/osmfile.otb
    ${TEMP}/openStreetMap.png
    ${TEMP}
    1.4835345
    43.55968261
    12
    )
endif()
