/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copied from
 * http://blog.bigpixel.ro/2010/04/23/storing-pointer-in-qvariant/
 */


#ifndef QVariantPtr_H
#define QVariantPtr_H


template <class T> class QVariantPtr
{
public:
    static T* asPtr(QVariant v) {
        return (T *) v.value<void *>();
    }

    static QVariant asQVariant(T *ptr) {
        return qVariantFromValue((void *) ptr);
    }
};


#endif // QVariantPtr_H
