/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
    @file
    Implement the main function that is the first function to be called when the application starts.
    Initialises the application and creates any MainWindows required.  Calls exec on the KApplication
    to begin the event loop.
    */


/**
    @mainpage KXStitch documentation
    @section intro_section Introduction
    KXStitch is a program that lets you create cross stitch patterns and charts. Patterns can be created
    from scratch on a user defined size of grid, which can be enlarged or reduced in size as your pattern
    progresses. Alternatively you can import images from many graphics formats or scan images using any
    Sane supported scanner. Both of these options allow you to restrict the conversion to full stitches
    or optionally use fractional stitches. You may also use an image as a background. These imported images
    can then be modified using the supplied tools to produce your final design.

    There are many tools to aid designing your pattern, from open and filled rectangles and ellipses, filled
    polygons and lines and backstitching. Additionally cut, copy and paste can be used to duplicate selected
    areas. Selected areas can also be rotated 90, 180 and 270 degrees anti clockwise, or mirrored horizontally
    or vertically.

    There is also a pattern library that can be used to store small and not so small portions of patterns that
    can then be reused in other patterns. The patterns in these library are stored in a hierarchical list that
    makes it easy to sort and navigate them to find the ones you want. These pattern libraries can also be used
    as alphabets by assigning a character to each one. By using the Alphabet tool, each key press is searched
    for in the current library and if present, the associated pattern will be inserted in the pattern.

    When your ready, you can print out your design in several formats. There are options to print out cover
    sheets, instructions and a floss key including amount of thread used and number of stitches.

    @section features_section Features
    - Creation of new patterns.
    - Editing of existing pattern, KXStitch is also capable of reading PC Stitch 5 files.
    - Use of various floss pallets, DMC, Anchor, Madeira, JP Coates.
    - Creation of custom pallets and colours.
    - Use of standard stitches.
    - Free use of back stitching.
    - Importing of various picture formats.
    - Printing of patterns and floss keys.

    @section license_section License
    KXStitch is provided as free software; you can redistribute it and/or modify it under the terms of the
    GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.under the GNU General Public License V2
    */


#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KUrl>


#include "configuration.h"
#include "MainWindow.h"


/**
    The main function creates an instance of a KAboutData object and populates it with any
    information necessary for the application.

    A KApplication object is created to manage the application.  If the session is restored
    the previous MainWindows will be restored to their former state when the session was saved.

    Alternatively a KCmdLineArgs object is created to manage any arguments passed on the command
    line.  For each of the arguments provided, a new MainWindow is created using the arguments url.
    This MainWindow is then shown on the desktop.  If no arguments are provided a new MainWindow is
    created using an empty KUrl, creating a new document, which is then shown on the desktop.

    The KApplication instance is then executed which begins the event loop allowing user interaction.
    */
int main(int argc, char *argv[])
{
    KAboutData aboutData("kxstitch",
                         "kxstitch",
                         ki18n("KXStitch"), "1.1.2",
                         ki18n("A cross stitch pattern creator."),
                         KAboutData::License_GPL_V2,
                         ki18n("(c)2010-2014 Stephen Allewell"),
                         KLocalizedString(),
                         "http://userbase.kde.org/KXStitch");
    aboutData.addAuthor(ki18n("Stephen Allewell"), ki18n("Project Lead"), "steve.allewell@gmail.com");
    aboutData.addCredit(ki18n("Pierre Brua"), ki18n("Bug fixes, application icons"), "kxstitchdev@paralline.com");
    aboutData.addCredit(ki18n("Eric Pareja"), ki18n("Man page"), "xenos@upm.edu.ph");
    aboutData.addCredit(ki18n("Adam Gundy"), ki18n("Bug fixes, improvements"), "adam@starsilk.net");
    aboutData.setTranslator(ki18nc("NAME OF TRANSLATORS", "Your names"),
                            ki18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("+[file]", ki18n("Document to open"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    if (app.isSessionRestored()) {
        kRestoreMainWindows<MainWindow>();
    } else {
        MainWindow *mainWindow;
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        if (args->count() > 0) {
            for (int i = 0 ; i < args->count() ; ++i) {
                KUrl url = args->url(i);
                mainWindow = new MainWindow(url);
                mainWindow->show();
            }
        } else {
            mainWindow = new MainWindow(KUrl());
            mainWindow->show();
        }

        args->clear();
    }

    return app.exec();
}
