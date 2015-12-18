/*
 * Copyright (C) 2009-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef KeycodeLineEdit_H
#define KeycodeLineEdit_H


#include <KLineEdit>
#include <QString>


class QKeyEvent;


class KeycodeLineEdit : public KLineEdit
{
    Q_OBJECT

public:
    explicit KeycodeLineEdit(QWidget *parent);

    int key();
    Qt::KeyboardModifiers modifiers();
    void setKeyModifiers(int key, Qt::KeyboardModifiers modifiers);
    static QString keyString(int qtKey, Qt::KeyboardModifiers modifiers);

protected:
    virtual void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

private:
    static QString findQtText(int qtKey);
    int     m_key;
    Qt::KeyboardModifiers m_modifiers;
};


#endif // KeycodeLineEdit_H
