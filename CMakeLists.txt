cmake_minimum_required(VERSION 2.6)

project (kxstitch)

find_package (KDE4 REQUIRED)
find_package (ImageMagick COMPONENTS MagickCore Magick++ REQUIRED)
find_package (Doxygen)

if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif (DOXYGEN_FOUND)

include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
include_directories (${ImageMagick_Magick++_INCLUDE_DIR} ${ImageMagick_MagickCore_INCLUDE_DIR})

set (kxstitch_SRCS
  src/main.cpp
  src/mainwindow.cpp
  src/scale.cpp
  src/editor.cpp
  src/preview.cpp
  src/palette.cpp
  src/document.cpp
  src/stitch.cpp
  src/schememanager.cpp
  src/schemeparser.cpp
  src/flossscheme.cpp
  src/palettemanagerdlg.cpp
  src/commands.cpp
  src/backgroundimage.cpp
  src/floss.cpp
)

add_subdirectory(icons)
add_subdirectory(schemes)

kde4_add_kcfg_files(kxstitch_SRCS configuration.kcfgc)

kde4_add_ui_files(kxstitch_SRCS
  ui/appearance.ui
  ui/general.ui
  ui/colors.ui
  ui/fonts.ui
  ui/palettemanager.ui
  ui/patternproperties.ui
)

kde4_add_app_icon (lo32-app-kxstitch.png lo16-app-kxstitch.png)

kde4_add_executable (kxstitch ${kxstitch_SRCS})

target_link_libraries (kxstitch ${KDE4_KDEUI_LIBS}
                                ${KDE4_KIO_LIBS}
                                ${ImageMagick_Magick++_LIBRARY} ${ImageMagick_MagickCore_LIBRARY})

install (TARGETS kxstitch DESTINATION ${BIN_INSTALL_DIR})
install (FILES kxstitchui.rc DESTINATION ${DATA_INSTALL_DIR}/kxstitch)
install (FILES kxstitch.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install (FILES kxstitch.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
