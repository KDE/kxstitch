cmake_minimum_required(VERSION 2.6)

project (kxstitch)

find_package (KDE4 REQUIRED)
find_package (ImageMagick COMPONENTS MagickCore Magick++ REQUIRED)
find_package (X11)
find_package (Doxygen)
find_package (SharedMimeInfo)

if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURENT_BINARY_DIR}
                       COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif (DOXYGEN_FOUND)

include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${ImageMagick_Magick++_INCLUDE_DIR} ${ImageMagick_MagickCore_INCLUDE_DIR})
include_directories (${X11_INCLUDE_DIR})

set (kxstitch_SRCS
    src/BackgroundImage.cpp
    src/BackgroundImages.cpp
    src/Boundary.cpp
    src/Commands.cpp
    src/ConfigurationDialogs.cpp
    src/Document.cpp
    src/DocumentFloss.cpp
    src/DocumentPalette.cpp
    src/Editor.cpp
    src/Element.cpp
    src/Exceptions.cpp
    src/Floss.cpp
    src/FlossScheme.cpp
    src/KeycodeLineEdit.cpp
    src/Layer.cpp
    src/Layers.cpp
    src/LibraryFile.cpp
    src/LibraryPattern.cpp
    src/Main.cpp
    src/MainWindow.cpp
    src/Page.cpp
    src/Palette.cpp
    src/PaperSizes.cpp
    src/Pattern.cpp
    src/Preview.cpp
    src/PrinterConfiguration.cpp
    src/Renderer.cpp
    src/Scale.cpp
    src/SchemeManager.cpp
    src/SchemeParser.cpp
    src/Stitch.cpp
    src/StitchData.cpp
    src/Symbol.cpp
    src/SymbolLibrary.cpp
    src/SymbolManager.cpp
    src/XKeyLock.cpp

    src/AlphaSelect.cpp
    src/CalibrateFlossDlg.cpp
    src/ExtendPatternDlg.cpp
    src/FilePropertiesDlg.cpp
    src/ImageElementDlg.cpp
    src/ImportImageDlg.cpp
    src/KeyElementDlg.cpp
    src/LibraryFilePathsDlg.cpp
    src/LibraryListWidget.cpp
    src/LibraryListWidgetItem.cpp
    src/LibraryManagerDlg.cpp
    src/LibraryPatternPropertiesDlg.cpp
    src/LibraryTreeWidget.cpp
    src/LibraryTreeWidgetItem.cpp
    src/NewFlossDlg.cpp
    src/PaletteManagerDlg.cpp
    src/PageLayoutEditor.cpp
    src/PagePreviewListWidgetItem.cpp
    src/PagePropertiesDlg.cpp
    src/PatternElementDlg.cpp
    src/PrintSetupDlg.cpp
    src/SelectArea.cpp
    src/SymbolListWidget.cpp
    src/SymbolSelectorDlg.cpp
    src/TextElementDlg.cpp
    src/TextToolDlg.cpp
)

add_subdirectory(icons)
add_subdirectory(po)
add_subdirectory(schemes)
add_subdirectory(symbols)
add_subdirectory(doc)

kde4_add_kcfg_files(kxstitch_SRCS configuration.kcfgc)

kde4_add_ui_files(kxstitch_SRCS
    ui/CalibrateFloss.ui
    ui/ExtendPattern.ui
    ui/FileProperties.ui
    ui/ImageElement.ui
    ui/KeyElement.ui
    ui/ImportImage.ui
    ui/LibraryFilePaths.ui
    ui/LibraryManager.ui
    ui/LibraryPatternProperties.ui
    ui/NewFloss.ui
    ui/PageProperties.ui
    ui/PaletteManager.ui
    ui/PatternElement.ui
    ui/PrintSetup.ui
    ui/SymbolSelector.ui
    ui/TextElement.ui
    ui/TextTool.ui
    ui/EditorConfigPage.ui
    ui/PatternConfigPage.ui
    ui/ImportConfigPage.ui
    ui/LibraryConfigPage.ui
    ui/PrinterConfigPage.ui
)

kde4_install_icons(${ICON_INSTALL_DIR})

kde4_add_executable (kxstitch ${kxstitch_SRCS})

target_link_libraries (kxstitch ${KDE4_KDEUI_LIBS}
                       ${KDE4_KIO_LIBS}
                       ${ImageMagick_Magick++_LIBRARY} ${ImageMagick_MagickCore_LIBRARY}
                       ${X11_LIBRARIES})

set (WITH_PROFILING OFF CACHE BOOL "Build with profiling support")

if (WITH_PROFILING)
    add_definitions( -pg )
    set_target_properties (kxstitch PROPERTIES LINK_FLAGS -pg)
endif (WITH_PROFILING)

add_definitions( -DQT_NO_COMPAT )
add_definitions( -DKDE_NO_COMPAT )
add_definitions( -fexceptions )

install (TARGETS kxstitch DESTINATION ${BIN_INSTALL_DIR})
install (FILES kxstitchui.rc DESTINATION ${DATA_INSTALL_DIR}/kxstitch)
install (FILES kxstitch.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install (FILES kxstitch.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install (FILES kxstitch.1.gz DESTINATION ${MAN_INSTALL_DIR}/man1)

IF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
    install (FILES kxstitch.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes (${XDG_MIME_INSTALL_DIR})
ENDIF(SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
