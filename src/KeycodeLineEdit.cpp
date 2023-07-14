/*
 * Copyright (C) 2009-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "KeycodeLineEdit.h"

#include <QKeyEvent>


const struct KEYCODE {
    int qtKey;
    QString qtText;
} keyCodes[] = {
    { Qt::Key_Escape, QStringLiteral("Escape") },
    { Qt::Key_Tab, QStringLiteral("Tab") },
    { Qt::Key_Backtab, QStringLiteral("Backtab") },
// { Qt::Key_Backspace, "Backspace" },
// { Qt::Key_Return, "Return" },
// { Qt::Key_Enter, "Enter" },
    { Qt::Key_Insert, QStringLiteral("Insert") },
// { Qt::Key_Delete, "Delete" },
    { Qt::Key_Pause, QStringLiteral("Pause") },
    { Qt::Key_Print, QStringLiteral("Print") },
    { Qt::Key_SysReq, QStringLiteral("SysReq") },
    { Qt::Key_Clear, QStringLiteral("Clear") },
    { Qt::Key_Home, QStringLiteral("Home") },
    { Qt::Key_End, QStringLiteral("End") },
    { Qt::Key_Left, QStringLiteral("Left") },
    { Qt::Key_Up, QStringLiteral("Up") },
    { Qt::Key_Right, QStringLiteral("Right") },
    { Qt::Key_Down, QStringLiteral("Down") },
    { Qt::Key_PageUp, QStringLiteral("PageUp") },
    { Qt::Key_PageDown, QStringLiteral("PageDown") },
    { Qt::Key_Shift, QStringLiteral("Shift") },
    { Qt::Key_Control, QStringLiteral("Control") },
    { Qt::Key_Meta, QStringLiteral("Meta") },
    { Qt::Key_Alt, QStringLiteral("Alt") },
// { Qt::Key_AltGr, "AltGr" },
// { Qt::Key_CapsLock, "CapsLock" },
// { Qt::Key_NumLock, "NumLock" },
// { Qt::Key_ScrollLock, "ScrollLock" },
    { Qt::Key_F1, QStringLiteral("F1") },
    { Qt::Key_F2, QStringLiteral("F2") },
    { Qt::Key_F3, QStringLiteral("F3") },
    { Qt::Key_F4, QStringLiteral("F4") },
    { Qt::Key_F5, QStringLiteral("F5") },
    { Qt::Key_F6, QStringLiteral("F6") },
    { Qt::Key_F7, QStringLiteral("F7") },
    { Qt::Key_F8, QStringLiteral("F8") },
    { Qt::Key_F9, QStringLiteral("F9") },
    { Qt::Key_F10, QStringLiteral("F10") },
    { Qt::Key_F11, QStringLiteral("F11") },
    { Qt::Key_F12, QStringLiteral("F12") },
    { Qt::Key_F13, QStringLiteral("F13") },
    { Qt::Key_F14, QStringLiteral("F14") },
    { Qt::Key_F15, QStringLiteral("F15") },
    { Qt::Key_F16, QStringLiteral("F16") },
    { Qt::Key_F17, QStringLiteral("F17") },
    { Qt::Key_F18, QStringLiteral("F18") },
    { Qt::Key_F19, QStringLiteral("F19") },
    { Qt::Key_F20, QStringLiteral("F20") },
    { Qt::Key_F21, QStringLiteral("F21") },
    { Qt::Key_F22, QStringLiteral("F22") },
    { Qt::Key_F23, QStringLiteral("F23") },
    { Qt::Key_F24, QStringLiteral("F24") },
    { Qt::Key_F25, QStringLiteral("F25") },
    { Qt::Key_F26, QStringLiteral("F26") },
    { Qt::Key_F27, QStringLiteral("F27") },
    { Qt::Key_F28, QStringLiteral("F28") },
    { Qt::Key_F29, QStringLiteral("F29") },
    { Qt::Key_F30, QStringLiteral("F30") },
    { Qt::Key_F31, QStringLiteral("F31") },
    { Qt::Key_F32, QStringLiteral("F32") },
    { Qt::Key_F33, QStringLiteral("F33") },
    { Qt::Key_F34, QStringLiteral("F34") },
    { Qt::Key_F35, QStringLiteral("F35") },
    { Qt::Key_Super_L, QStringLiteral("Super_L") },
    { Qt::Key_Super_R, QStringLiteral("Super_R") },
    { Qt::Key_Menu, QStringLiteral("Menu") },
    { Qt::Key_Hyper_L, QStringLiteral("Hyper_L") },
    { Qt::Key_Hyper_R, QStringLiteral("Hyper_R") },
    { Qt::Key_Help, QStringLiteral("Help") },
    { Qt::Key_Direction_L, QStringLiteral("Direction_L") },
    { Qt::Key_Direction_R, QStringLiteral("Direction_R") },
    { Qt::Key_Space, QStringLiteral("Space") },
    { Qt::Key_Any, QStringLiteral("Any") },
    { Qt::Key_Exclam, QStringLiteral("Exclam") },
    { Qt::Key_QuoteDbl, QStringLiteral("QuoteDbl") },
    { Qt::Key_NumberSign, QStringLiteral("NumberSign") },
    { Qt::Key_Dollar, QStringLiteral("Dollar") },
    { Qt::Key_Percent, QStringLiteral("Percent") },
    { Qt::Key_Ampersand, QStringLiteral("Ampersand") },
    { Qt::Key_Apostrophe, QStringLiteral("Apostrophe") },
    { Qt::Key_ParenLeft, QStringLiteral("ParenLeft") },
    { Qt::Key_ParenRight, QStringLiteral("ParenRight") },
    { Qt::Key_Asterisk, QStringLiteral("Asterisk") },
    { Qt::Key_Plus, QStringLiteral("Plus") },
    { Qt::Key_Comma, QStringLiteral("Comma") },
    { Qt::Key_Minus, QStringLiteral("Minus") },
    { Qt::Key_Period, QStringLiteral("Period") },
    { Qt::Key_Slash, QStringLiteral("Slash") },
    { Qt::Key_0, QStringLiteral("0") },
    { Qt::Key_1, QStringLiteral("1") },
    { Qt::Key_2, QStringLiteral("2") },
    { Qt::Key_3, QStringLiteral("3") },
    { Qt::Key_4, QStringLiteral("4") },
    { Qt::Key_5, QStringLiteral("5") },
    { Qt::Key_6, QStringLiteral("6") },
    { Qt::Key_7, QStringLiteral("7") },
    { Qt::Key_8, QStringLiteral("8") },
    { Qt::Key_9, QStringLiteral("9") },
    { Qt::Key_Colon, QStringLiteral("Colon") },
    { Qt::Key_Semicolon, QStringLiteral("Semicolon") },
    { Qt::Key_Less, QStringLiteral("Less") },
    { Qt::Key_Equal, QStringLiteral("Equal") },
    { Qt::Key_Greater, QStringLiteral("Greater") },
    { Qt::Key_Question, QStringLiteral("Question") },
    { Qt::Key_At, QStringLiteral("At") },
    { Qt::Key_A, QStringLiteral("A") },
    { Qt::Key_B, QStringLiteral("B") },
    { Qt::Key_C, QStringLiteral("C") },
    { Qt::Key_D, QStringLiteral("D") },
    { Qt::Key_E, QStringLiteral("E") },
    { Qt::Key_F, QStringLiteral("F") },
    { Qt::Key_G, QStringLiteral("G") },
    { Qt::Key_H, QStringLiteral("H") },
    { Qt::Key_I, QStringLiteral("I") },
    { Qt::Key_J, QStringLiteral("J") },
    { Qt::Key_K, QStringLiteral("K") },
    { Qt::Key_L, QStringLiteral("L") },
    { Qt::Key_M, QStringLiteral("M") },
    { Qt::Key_N, QStringLiteral("N") },
    { Qt::Key_O, QStringLiteral("O") },
    { Qt::Key_P, QStringLiteral("P") },
    { Qt::Key_Q, QStringLiteral("Q") },
    { Qt::Key_R, QStringLiteral("R") },
    { Qt::Key_S, QStringLiteral("S") },
    { Qt::Key_T, QStringLiteral("T") },
    { Qt::Key_U, QStringLiteral("U") },
    { Qt::Key_V, QStringLiteral("V") },
    { Qt::Key_W, QStringLiteral("W") },
    { Qt::Key_X, QStringLiteral("X") },
    { Qt::Key_Y, QStringLiteral("Y") },
    { Qt::Key_Z, QStringLiteral("Z") },
    { Qt::Key_BracketLeft, QStringLiteral("BracketLeft") },
    { Qt::Key_Backslash, QStringLiteral("Backslash") },
    { Qt::Key_BracketRight, QStringLiteral("BracketRight") },
    { Qt::Key_AsciiCircum, QStringLiteral("AsciiCircum") },
    { Qt::Key_Underscore, QStringLiteral("Underscore") },
    { Qt::Key_QuoteLeft, QStringLiteral("QuoteLeft") },
    { Qt::Key_BraceLeft, QStringLiteral("BraceLeft") },
    { Qt::Key_Bar, QStringLiteral("Bar") },
    { Qt::Key_BraceRight, QStringLiteral("BraceRight") },
    { Qt::Key_AsciiTilde, QStringLiteral("AsciiTilde") },
    { Qt::Key_nobreakspace, QStringLiteral("nobreakspace") },
    { Qt::Key_exclamdown, QStringLiteral("exclamdown") },
    { Qt::Key_cent, QStringLiteral("cent") },
    { Qt::Key_sterling, QStringLiteral("sterling") },
    { Qt::Key_currency, QStringLiteral("currency") },
    { Qt::Key_yen, QStringLiteral("yen") },
    { Qt::Key_brokenbar, QStringLiteral("brokenbar") },
    { Qt::Key_section, QStringLiteral("section") },
    { Qt::Key_diaeresis, QStringLiteral("diaeresis") },
    { Qt::Key_copyright, QStringLiteral("copyright") },
    { Qt::Key_ordfeminine, QStringLiteral("ordfeminine") },
    { Qt::Key_guillemotleft, QStringLiteral("guillemotleft") },
    { Qt::Key_notsign, QStringLiteral("notsign") },
    { Qt::Key_hyphen, QStringLiteral("hyphen") },
    { Qt::Key_registered, QStringLiteral("registered") },
    { Qt::Key_macron, QStringLiteral("macron") },
    { Qt::Key_degree, QStringLiteral("degree") },
    { Qt::Key_plusminus, QStringLiteral("plusminus") },
    { Qt::Key_twosuperior, QStringLiteral("twosuperior") },
    { Qt::Key_threesuperior, QStringLiteral("threesuperior") },
    { Qt::Key_acute, QStringLiteral("acute") },
    { Qt::Key_mu, QStringLiteral("mu") },
    { Qt::Key_paragraph, QStringLiteral("paragraph") },
    { Qt::Key_periodcentered, QStringLiteral("periodcentered") },
    { Qt::Key_cedilla, QStringLiteral("cedilla") },
    { Qt::Key_onesuperior, QStringLiteral("onesuperior") },
    { Qt::Key_masculine, QStringLiteral("masculine") },
    { Qt::Key_guillemotright, QStringLiteral("guillemotright") },
    { Qt::Key_onequarter, QStringLiteral("onequarter") },
    { Qt::Key_onehalf, QStringLiteral("onehalf") },
    { Qt::Key_threequarters, QStringLiteral("threequarters") },
    { Qt::Key_questiondown, QStringLiteral("questiondown") },
    { Qt::Key_Agrave, QStringLiteral("Agrave") },
    { Qt::Key_Aacute, QStringLiteral("Aacute") },
    { Qt::Key_Acircumflex, QStringLiteral("Acircumflex") },
    { Qt::Key_Atilde, QStringLiteral("Atilde") },
    { Qt::Key_Adiaeresis, QStringLiteral("Adiaeresis") },
    { Qt::Key_Aring, QStringLiteral("Aring") },
    { Qt::Key_AE, QStringLiteral("AE") },
    { Qt::Key_Ccedilla, QStringLiteral("Ccedilla") },
    { Qt::Key_Egrave, QStringLiteral("Egrave") },
    { Qt::Key_Eacute, QStringLiteral("Eacute") },
    { Qt::Key_Ecircumflex, QStringLiteral("Ecircumflex") },
    { Qt::Key_Ediaeresis, QStringLiteral("Ediaeresis") },
    { Qt::Key_Igrave, QStringLiteral("Igrave") },
    { Qt::Key_Iacute, QStringLiteral("Iacute") },
    { Qt::Key_Icircumflex, QStringLiteral("Icircumflex") },
    { Qt::Key_Idiaeresis, QStringLiteral("Idiaeresis") },
    { Qt::Key_ETH, QStringLiteral("ETH") },
    { Qt::Key_Ntilde, QStringLiteral("Ntilde") },
    { Qt::Key_Ograve, QStringLiteral("Ograve") },
    { Qt::Key_Oacute, QStringLiteral("Oacute") },
    { Qt::Key_Ocircumflex, QStringLiteral("Ocircumflex") },
    { Qt::Key_Otilde, QStringLiteral("Otilde") },
    { Qt::Key_Odiaeresis, QStringLiteral("Odiaeresis") },
    { Qt::Key_multiply, QStringLiteral("multiply") },
    { Qt::Key_Ooblique  , QStringLiteral("Ooblique") },
    { Qt::Key_Ugrave, QStringLiteral("Ugrave") },
    { Qt::Key_Uacute, QStringLiteral("Uacute") },
    { Qt::Key_Ucircumflex, QStringLiteral("Ucircumflex") },
    { Qt::Key_Udiaeresis, QStringLiteral("Udiaeresis") },
    { Qt::Key_Yacute, QStringLiteral("Yacute") },
    { Qt::Key_THORN, QStringLiteral("THORN") },
    { Qt::Key_ssharp, QStringLiteral("ssharp") },
    { Qt::Key_division, QStringLiteral("division") },
    { Qt::Key_ydiaeresis, QStringLiteral("ydiaeresis") },
// { Qt::Key_Multi_key, "Multi_key" },
// { Qt::Key_Codeinput, "Codeinput" },
// { Qt::Key_SingleCandidate, "SingleCandidate" },
// { Qt::Key_MultipleCandidate, "MultipleCandidate" },
// { Qt::Key_PreviousCandidate, "PreviousCandidate" },
// { Qt::Key_Mode_switch, "Mode_switch" },
// { Qt::Key_Kanji, "Kanji" },
// { Qt::Key_Muhenkan, "Muhenkan" },
// { Qt::Key_Henkan, "Henkan" },
// { Qt::Key_Romaji, "Romaji" },
// { Qt::Key_Hiragana, "Hiragana" },
// { Qt::Key_Katakana, "Katakana" },
// { Qt::Key_Hiragana_Katakana, "Hiragana_Katakana" },
// { Qt::Key_Zenkaku, "Zenkaku" },
// { Qt::Key_Hankaku, "Hankaku" },
// { Qt::Key_Zenkaku_Hankaku, "Zenkaku_Hankaku" },
// { Qt::Key_Touroku, "Touroku" },
// { Qt::Key_Massyo, "Massyo" },
// { Qt::Key_Kana_Lock, "Kana_Lock" },
// { Qt::Key_Kana_Shift, "Kana_Shift" },
// { Qt::Key_Eisu_Shift, "Eisu_Shift" },
// { Qt::Key_Eisu_toggle, "Eisu_toggle" },
// { Qt::Key_Hangul, "Hangul" },
// { Qt::Key_Hangul_Start, "Hangul_Start" },
// { Qt::Key_Hangul_End, "Hangul_End" },
// { Qt::Key_Hangul_Hanja, "Hangul_Hanja" },
// { Qt::Key_Hangul_Jamo, "Hangul_Jamo" },
// { Qt::Key_Hangul_Romaja, "Hangul_Romaja" },
// { Qt::Key_Hangul_Jeonja, "Hangul_Jeonja" },
// { Qt::Key_Hangul_Banja, "Hangul_Banja" },
// { Qt::Key_Hangul_PreHanja, "Hangul_PreHanja" },
// { Qt::Key_Hangul_PostHanja, "Hangul_PostHanja" },
// { Qt::Key_Hangul_Special, "Hangul_Special" },
// { Qt::Key_Dead_Grave, "Dead_Grave" },
// { Qt::Key_Dead_Acute, "Dead_Acute" },
// { Qt::Key_Dead_Circumflex, "Dead_Circumflex" },
// { Qt::Key_Dead_Tilde, "Dead_Tilde" },
// { Qt::Key_Dead_Macron, "Dead_Macron" },
// { Qt::Key_Dead_Breve, "Dead_Breve" },
// { Qt::Key_Dead_Abovedot, "Dead_Abovedot" },
// { Qt::Key_Dead_Diaeresis, "Dead_Diaeresis" },
// { Qt::Key_Dead_Abovering, "Dead_Abovering" },
// { Qt::Key_Dead_Doubleacute, "Dead_Doubleacute" },
// { Qt::Key_Dead_Caron, "Dead_Caron" },
// { Qt::Key_Dead_Cedilla, "Dead_Cedilla" },
// { Qt::Key_Dead_Ogonek, "Dead_Ogonek" },
// { Qt::Key_Dead_Iota, "Dead_Iota" },
// { Qt::Key_Dead_Voiced_Sound, "Dead_Voiced_Sound" },
// { Qt::Key_Dead_Semivoiced_Sound, "Dead_Semivoiced_Sound" },
// { Qt::Key_Dead_Belowdot, "Dead_Belowdot" },
// { Qt::Key_Dead_Hook, "Dead_Hook" },
// { Qt::Key_Dead_Horn, "Dead_Horn" },
    { Qt::Key_Back, QStringLiteral("Back") },
    { Qt::Key_Forward, QStringLiteral("Forward") },
    { Qt::Key_Stop, QStringLiteral("Stop") },
    { Qt::Key_Refresh, QStringLiteral("Refresh") },
    { Qt::Key_VolumeDown, QStringLiteral("VolumeDown") },
    { Qt::Key_VolumeMute, QStringLiteral("VolumeMute") },
    { Qt::Key_VolumeUp, QStringLiteral("VolumeUp") },
    { Qt::Key_BassBoost, QStringLiteral("BassBoost") },
    { Qt::Key_BassUp, QStringLiteral("BassUp") },
    { Qt::Key_BassDown, QStringLiteral("BassDown") },
    { Qt::Key_TrebleUp, QStringLiteral("TrebleUp") },
    { Qt::Key_TrebleDown, QStringLiteral("TrebleDown") },
    { Qt::Key_MediaPlay, QStringLiteral("MediaPlay") },
    { Qt::Key_MediaStop, QStringLiteral("MediaStop") },
// { Qt::Key_MediaPrev, "MediaPrev" },
    { Qt::Key_MediaNext, QStringLiteral("MediaNext") },
    { Qt::Key_MediaRecord, QStringLiteral("MediaRecord") },
    { Qt::Key_HomePage, QStringLiteral("HomePage") },
    { Qt::Key_Favorites, QStringLiteral("Favorites") },
    { Qt::Key_Search, QStringLiteral("Search") },
    { Qt::Key_Standby, QStringLiteral("Standby") },
    { Qt::Key_OpenUrl, QStringLiteral("OpenUrl") },
    { Qt::Key_LaunchMail, QStringLiteral("LaunchMail") },
    { Qt::Key_LaunchMedia, QStringLiteral("LaunchMedia") },
    { Qt::Key_Launch0, QStringLiteral("Launch0") },
    { Qt::Key_Launch1, QStringLiteral("Launch1") },
    { Qt::Key_Launch2, QStringLiteral("Launch2") },
    { Qt::Key_Launch3, QStringLiteral("Launch3") },
    { Qt::Key_Launch4, QStringLiteral("Launch4") },
    { Qt::Key_Launch5, QStringLiteral("Launch5") },
    { Qt::Key_Launch6, QStringLiteral("Launch6") },
    { Qt::Key_Launch7, QStringLiteral("Launch7") },
    { Qt::Key_Launch8, QStringLiteral("Launch8") },
    { Qt::Key_Launch9, QStringLiteral("Launch9") },
    { Qt::Key_LaunchA, QStringLiteral("LaunchA") },
    { Qt::Key_LaunchB, QStringLiteral("LaunchB") },
    { Qt::Key_LaunchC, QStringLiteral("LaunchC") },
    { Qt::Key_LaunchD, QStringLiteral("LaunchD") },
    { Qt::Key_LaunchE, QStringLiteral("LaunchE") },
    { Qt::Key_LaunchF, QStringLiteral("LaunchF") },
    { Qt::Key_MediaLast, QStringLiteral("MediaLast") },
    { Qt::Key_unknown, QStringLiteral("unknown") }
// { Qt::Key_Call, "Call" },
// { Qt::Key_Context1, "Context1" },
// { Qt::Key_Context2, "Context2" },
// { Qt::Key_Context3, "Context3" },
// { Qt::Key_Context4, "Context4" },
// { Qt::Key_Flip, "Flip" },
// { Qt::Key_Hangup, "Hangup" },
// { Qt::Key_No, "No" },
// { Qt::Key_Select, "Select" },
// { Qt::Key_Yes, "Yes" },
// { Qt::Key_Execute, "Execute" },
// { Qt::Key_Printer, "Printer" },
// { Qt::Key_Play, "Play" },
// { Qt::Key_Sleep, "Sleep" },
// { Qt::Key_Zoom, "Zoom" },
// { Qt::Key_Cancel, "Cancel"}
};


KeycodeLineEdit::KeycodeLineEdit(QWidget *parent)
    :   KLineEdit(parent)
{
}


int KeycodeLineEdit::key()
{
    return m_key;
}


Qt::KeyboardModifiers KeycodeLineEdit::modifiers()
{
    return m_modifiers;
}


void KeycodeLineEdit::setKeyModifiers(int key, Qt::KeyboardModifiers modifiers)
{
    m_key = key;
    m_modifiers = modifiers;

    if (m_key) {
        setText(keyString(m_key, m_modifiers));
    } else {
        clear();
    }
}


void KeycodeLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) {
        setKeyModifiers(0, Qt::NoModifier);
        e->accept();
    } else if (!findQtText(e->key()).isEmpty()) {
        setKeyModifiers(e->key(), e->modifiers());
        e->accept();
    } else {
        e->ignore();
    }
}


QString KeycodeLineEdit::findQtText(int qtKey)
{
    QString string;

    for (unsigned i = 0 ; i < sizeof(keyCodes) / sizeof(struct KEYCODE) ; ++i) {
        if (keyCodes[i].qtKey == qtKey) {
            string = keyCodes[i].qtText;
            break;
        }
    }

    return string;
}


QString KeycodeLineEdit::keyString(int qtKey, Qt::KeyboardModifiers modifiers)
{
    QString string;

    if (modifiers & Qt::ShiftModifier) {
        string += QLatin1String("SHIFT+");
    }

    if (modifiers & Qt::ControlModifier) {
        string += QStringLiteral("CTRL+");
    }

    if (modifiers & Qt::AltModifier) {
        string += QStringLiteral("ALT+");
    }

    if (modifiers & Qt::MetaModifier) {
        string += QStringLiteral("META+");
    }

    return string += findQtText(qtKey);
}

#include "moc_KeycodeLineEdit.cpp"
