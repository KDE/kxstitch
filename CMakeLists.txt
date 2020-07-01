cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif (POLICY CMP0063)

project (kxstitch)

find_package (ECM REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings)
include (ECMInstallIcons)
include (FeatureSummary)

kde_enable_exceptions()

find_package (Qt5 CONFIG REQUIRED
    Core
    PrintSupport
    Widgets
)

find_package (KF5DocTools)

find_package (KF5 REQUIRED COMPONENTS
    Config
    ConfigWidgets
    Completion
    GuiAddons
    I18n
    KIO
    TextWidgets
    WidgetsAddons
    XmlGui
)

find_package (ImageMagick COMPONENTS MagickCore Magick++ REQUIRED)
find_package (Doxygen)
find_package (SharedMimeInfo)

if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURENT_BINARY_DIR}
                       COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif (DOXYGEN_FOUND)

include_directories (BEFORE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)
# use both the _DIR and _DIRS variables, since the former are available
# only in older cmake versions, while the latter only in newer ones
include_directories (${ImageMagick_Magick++_INCLUDE_DIR} ${ImageMagick_MagickCore_INCLUDE_DIR})
include_directories (${ImageMagick_Magick++_INCLUDE_DIRS} ${ImageMagick_MagickCore_INCLUDE_DIRS})

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
    src/ScaledPixmapLabel.cpp
    src/SchemeManager.cpp
    src/SchemeParser.cpp
    src/Stitch.cpp
    src/StitchData.cpp
    src/Symbol.cpp
    src/SymbolLibrary.cpp
    src/SymbolManager.cpp

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

file(GLOB kxstitch_UI ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)

ki18n_wrap_ui(kxstitch_SRCS ${kxstitch_UI})

add_subdirectory(icons)
add_subdirectory(schemes)
add_subdirectory(symbols)
add_subdirectory(library)
add_subdirectory(doc)

kconfig_add_kcfg_files(kxstitch_SRCS configuration.kcfgc)

add_executable (kxstitch ${kxstitch_SRCS})

target_link_libraries (kxstitch
    Qt5::Core
    Qt5::PrintSupport
    Qt5::Widgets
    KF5::Completion
    KF5::ConfigGui
    KF5::GuiAddons
    KF5::KIOFileWidgets
    KF5::I18n
    KF5::TextWidgets
    KF5::WidgetsAddons
    KF5::XmlGui
    ${ImageMagick_Magick++_LIBRARY} ${ImageMagick_MagickCore_LIBRARY}
)

set (WITH_PROFILING OFF CACHE BOOL "Build with profiling support")

if (WITH_PROFILING)
    add_definitions( -pg )
    set_target_properties (kxstitch PROPERTIES LINK_FLAGS -pg)
endif (WITH_PROFILING)

if (SILENCE_DEPRECATED)
    add_definitions( -Wno-deprecated-declarations )
endif (SILENCE_DEPRECATED)

exec_program (Magick++-config ARGS --cxxflags OUTPUT_VARIABLE magick_config)

string (REGEX MATCH "-DMAGICKCORE_HDRI_ENABLE=." magick_config_hdri ${magick_config})
string (REGEX MATCH "-DMAGICKCORE_QUANTUM_DEPTH=[0-9]+" magick_config_quantum ${magick_config})

# earlier versions of Magick++-config don't provide the necessary options
if (magick_config_hdri STREQUAL "")
    # set a default for MAGICKCORE_HDRI_ENABLE to 0
    set (magick_config_hdri "-DMAGICKCORE_HDRI_ENABLE=0")
endif (magick_config_hdri STREQUAL "")

if (magick_config_quantum STREQUAL "")
    # MAGICKCORE_QUANTUM_DEPTH can also be found in the version string of an ImageMagick application
    exec_program (mogrify ARGS --version OUTPUT_VARIABLE mogrify_version)
    string (REGEX MATCH "Q8|Q16" mogrify_version_quantum ${mogrify_version})

    if (mogrify_version_quantum STREQUAL "Q8")
        set (magick_config_quantum "-DMAGICKCORE_QUANTUM_DEPTH=8")
    else (mogrify_version_quantum STREQUAL "Q8")
        set (magick_config_quantum "-DMAGICKCORE_QUANTUM_DEPTH=16")
    endif (mogrify_version_quantum STREQUAL "Q8")
endif (magick_config_quantum STREQUAL "")

message (STATUS "Set magic_config_quantum: " ${magick_config_quantum})
message (STATUS "Set magic_config_hdri: " ${magick_config_hdri})

add_definitions(
    -DQT_NO_COMPAT
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DKDE_NO_COMPAT
    -DQT_NO_URL_CAST_FROM_STRING
    ${magick_config_hdri}
    ${magick_config_quantum}
)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    message (STATUS "Processing translations")
    ki18n_install(po)
    kdoctools_install(po)
endif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")

install (TARGETS kxstitch DESTINATION ${BIN_INSTALL_DIR})
install (FILES kxstitchui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kxstitch)
install (FILES kxstitch.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install (FILES org.kde.kxstitch.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install (FILES org.kde.kxstitch.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

IF (SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})
    install (FILES kxstitch.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
    update_xdg_mimetypes (${XDG_MIME_INSTALL_DIR})
ENDIF(SHARED_MIME_INFO_FOUND AND IS_DIRECTORY ${XDG_MIME_INSTALL_DIR})

feature_summary (WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

