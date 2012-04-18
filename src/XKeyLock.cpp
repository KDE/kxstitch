/***************************************************************************
                         xkeylock.cpp  -  description
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

// application specific includes
#include "XKeyLock.h"

// includes files for Qt

// includes files for KDE

// X11 includes
extern "C"
{
  #include <X11/Xlib.h>
  #include <X11/keysym.h>
//  #include <X11/extensions/XTest.h>
}
      

XKeyLock::XKeyLock(Display *a_display)
  : numlock_mask(0), capslock_mask(0), scrolllock_mask(0)
{
  display = a_display;

  KeyCode keycode;

  keycode = XKeysymToKeycode(display, XK_Num_Lock);
  if (keycode != NoSymbol)
  {
    numlock_mask = getModifierMapping(keycode);
    if (numlock_mask == 0) numlock_mask = setModifierMapping(keycode);
  }

  keycode = XKeysymToKeycode(display, XK_Caps_Lock);
  if (keycode != NoSymbol)
  {
    capslock_mask = getModifierMapping(keycode);
    if (capslock_mask == 0) capslock_mask = setModifierMapping(keycode);
  }

  keycode = XKeysymToKeycode(display, XK_Scroll_Lock);
  if (keycode != NoSymbol)
  {
    scrolllock_mask = getModifierMapping(keycode);
    if (scrolllock_mask == 0) scrolllock_mask = setModifierMapping(keycode);
  }
}

XKeyLock::~XKeyLock()
{
}

unsigned int XKeyLock::getModifierMapping(KeyCode keycode)
{
  unsigned int mask = 0;

  XModifierKeymap* map = XGetModifierMapping(display);

  for(int i = 0 ; i < 8; ++i)
    if (map->modifiermap[map->max_keypermod * i] == keycode) 
      mask = 1 << i;

  XFreeModifiermap(map);
  return mask;
}

unsigned int XKeyLock::setModifierMapping(KeyCode keycode)
{
  unsigned int mask = 0;
  XModifierKeymap* map = XGetModifierMapping(display);

  for(int i = 0 ; i < 8; ++i)
    if (map->modifiermap[map->max_keypermod * i] == 0) 
    {
      map->modifiermap[map->max_keypermod * i] = keycode;
      XSetModifierMapping(display, map);
      mask = 1 << i;
      break;
    }

  XFreeModifiermap(map);
  return mask;
}


bool XKeyLock::isNumLockReadable()
{
  return (numlock_mask != 0);
}

bool XKeyLock::isCapsLockReadable()
{
  return (capslock_mask != 0);
}

bool XKeyLock::isScrollLockReadable()
{
  return (scrolllock_mask != 0);
}


bool XKeyLock::getNumLock()
{
  if (!XKeyLock::isNumLockReadable()) 
    return false;
  else 
    return (bool) (getIndicatorStates() & numlock_mask);
}

bool XKeyLock::getCapsLock()
{
  if (!XKeyLock::isCapsLockReadable()) 
    return false;
  else
    return (bool) (getIndicatorStates() & capslock_mask);
}

bool XKeyLock::getScrollLock()
{
  if (!XKeyLock::isScrollLockReadable()) 
    return false;
  else
    return (bool) (getIndicatorStates() & scrolllock_mask);
}

/*
void XKeyLock::toggleNumLock()
{
  XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Num_Lock), true, CurrentTime);
  XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Num_Lock), false, CurrentTime);
}

void XKeyLock::toggleCapsLock()
{
  XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Caps_Lock), true, CurrentTime);
  XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Caps_Lock), false, CurrentTime);
}

void XKeyLock::toggleScrollLock()
{
  XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Scroll_Lock), true, CurrentTime);
  XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Scroll_Lock), false, CurrentTime);
}

void XKeyLock::setNumLock(bool state)
{
  if (numlock_mask !=0 && getNumLock() != state)
    toggleNumLock();
}

void XKeyLock::setCapsLock(bool state)
{
  if (capslock_mask !=0 && getCapsLock() != state)
    toggleCapsLock();
}

void XKeyLock::setScrollLock(bool state)
{
  if (scrolllock_mask !=0 && getScrollLock() != state)
    toggleScrollLock();
}
*/

unsigned int XKeyLock::getIndicatorStates()
{
  Window dummy1, dummy2;
  int dummy3, dummy4, dummy5, dummy6;
  unsigned int indicatorStates;

  XQueryPointer(display, DefaultRootWindow(display), &dummy1, &dummy2,&dummy3, &dummy4, &dummy5, &dummy6, &indicatorStates);

  return indicatorStates;
}

