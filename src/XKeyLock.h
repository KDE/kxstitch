/***************************************************************************
                         xkeylock.h  -  description
                             -------------------
    begin                : Mon Mar 10 2003
    copyright            : (C) 2003 by Aurelien Jarno
    email                : aurelien@aurel32.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/**
  *@author Aurelien Jarno
  */

#ifndef XKEYLOCK_H
#define XKEYLOCK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// includes files for Qt

// X11 includes
extern "C"
{
#include <X11/Xlib.h>
}

class XKeyLock
{
//   Q_OBJECT
public:
    explicit XKeyLock(Display *display);
    ~XKeyLock();

    bool isNumLockReadable();
    bool isCapsLockReadable();
    bool isScrollLockReadable();

    bool getNumLock();
    bool getCapsLock();
    bool getScrollLock();
    /*
      void toggleNumLock();
      void toggleCapsLock();
      void toggleScrollLock();

      void setNumLock(bool state);
      void setCapsLock(bool state);
      void setScrollLock(bool state);
    */
protected:
    unsigned int getModifierMapping(KeyCode keycode);
    unsigned int setModifierMapping(KeyCode keycode);

    unsigned int getIndicatorStates();

    Display *display;
    unsigned int numlock_mask, capslock_mask, scrolllock_mask;
};

#endif

