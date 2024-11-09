cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (kxstitch VERSION "3.0.0")

find_package (ECM REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings NO_POLICY_SCOPE)
include (ECMSetupVersion)
include (ECMInstallIcons)
include (FeatureSummary)

kde_enable_exceptions()

find_package (Qt6 CONFIG REQUIRED
    Core
    PrintSupport
    Widgets
)

find_package (KF6DocTools)

find_package (KF6 REQUIRED COMPONENTS
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

find_package (PkgConfig REQUIRED)
pkg_check_modules (Magick++ REQUIRED IMPORTED_TARGET Magick++)
find_package (Doxygen)
find_package (SharedMimeInfo)

if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURENT_BINARY_DIR}
                       COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif (DOXYGEN_FOUND)

include_directories (BEFORE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)

ecm_setup_version(${kxstitch_VERSION}
    VARIABLE_PREFIX KXSTITCH
    VERSION_HEADER Version.h
)

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

    kxstitch.qrc
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
    Qt6::Core
    Qt6::PrintSupport
    Qt6::Widgets
    KF6::Completion
    KF6::ConfigGui
    KF6::GuiAddons
    KF6::KIOFileWidgets
    KF6::I18n
    KF6::TextWidgets
    KF6::WidgetsAddons
    KF6::XmlGui
    PkgConfig::Magick++
)

set (WITH_PROFILING OFF CACHE BOOL "Build with profiling support")

if (WITH_PROFILING)
    add_definitions( -pg )
    set_target_properties (kxstitch PROPERTIES LINK_FLAGS -pg)
endif (WITH_PROFILING)

if (SILENCE_DEPRECATED)
    add_definitions( -Wno-deprecated-declarations )
endif (SILENCE_DEPRECATED)

add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    message (STATUS "Processing translations")
    ki18n_install(po)
    kdoctools_install(po)
endif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")

install (TARGETS kxstitch DESTINATION ${KDE_INSTALL_BINDIR})
install (FILES kxstitch.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install (FILES org.kde.kxstitch.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install (FILES org.kde.kxstitch.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

IF (SHAREDMIMEINFO_FOUND AND IS_DIRECTORY ${KDE_INSTALL_MIMEDIR})
    install (FILES kxstitch.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
    update_xdg_mimetypes (${KDE_INSTALL_MIMEDIR})
ENDIF (SHAREDMIMEINFO_FOUND AND IS_DIRECTORY ${KDE_INSTALL_MIMEDIR})

feature_summary (WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
