file(GLOB _icon_files *.png *.svgz)

ecm_install_icons(ICONS ${_icon_files} DESTINATION ${KDE_INSTALL_ICONDIR})
