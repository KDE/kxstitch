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
	src/BackgroundImage.cpp
	src/BackgroundImages.cpp
	src/Commands.cpp
	src/Document.cpp
	src/DocumentFloss.cpp
	src/DocumentPalette.cpp
	src/Editor.cpp
	src/Floss.cpp
	src/FlossScheme.cpp
	src/KeycodeLineEdit.cpp
	src/Layer.cpp
	src/Layers.cpp
	src/LibraryFile.cpp
	src/LibraryPattern.cpp
	src/Main.cpp
	src/MainWindow.cpp
	src/Palette.cpp
	src/PatternMimeData.cpp
	src/Preview.cpp
	src/Renderer.cpp
	src/Scale.cpp
	src/SchemeManager.cpp
	src/SchemeParser.cpp
	src/Stitch.cpp
	src/StitchData.cpp

	src/AlphaSelect.cpp
	src/CalibrateFlossDlg.cpp
	src/ExtendPatternDlg.cpp
	src/FilePropertiesDlg.cpp
	src/ImportImageDlg.cpp
	src/LibraryFilePathsDlg.cpp
	src/LibraryListWidget.cpp
	src/LibraryListWidgetItem.cpp
	src/LibraryManagerDlg.cpp
	src/LibraryPatternPropertiesDlg.cpp
	src/LibraryTreeWidget.cpp
	src/LibraryTreeWidgetItem.cpp
	src/NewFlossDlg.cpp
	src/PaletteManagerDlg.cpp
	src/TextToolDlg.cpp
)

add_subdirectory(icons)
add_subdirectory(schemes)

kde4_add_kcfg_files(kxstitch_SRCS configuration.kcfgc)

kde4_add_ui_files(kxstitch_SRCS
	ui/CalibrateFloss.ui
	ui/ExtendPattern.ui
	ui/FileProperties.ui
	ui/ImportImage.ui
	ui/LibraryFilePaths.ui
	ui/LibraryManager.ui
	ui/LibraryPatternProperties.ui
	ui/NewFloss.ui
	ui/PaletteManager.ui
	ui/TextTool.ui
)

kde4_add_app_icon (hi32-app-kxstitch.png lo32-app-kxstitch.png lo16-app-kxstitch.png)

kde4_add_executable (kxstitch ${kxstitch_SRCS})

target_link_libraries (kxstitch ${KDE4_KDEUI_LIBS}
				${KDE4_KIO_LIBS}
				${ImageMagick_Magick++_LIBRARY} ${ImageMagick_MagickCore_LIBRARY})

set (CMAKE_CXX_FLAGS "-DQT_NO_COMPAT -DKDE_NO_COMPAT")

install (TARGETS kxstitch DESTINATION ${BIN_INSTALL_DIR})
install (FILES kxstitchui.rc DESTINATION ${DATA_INSTALL_DIR}/kxstitch)
install (FILES kxstitch.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install (FILES kxstitch.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
