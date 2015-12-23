/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PaperSizes.h"


const struct PaperSize {
    QPrinter::PaperSize size;
    QString             name;
    int                 width;
    int                 height;
} PaperSizeList[] = {
    { QPrinter::A0,         QStringLiteral("A0"),       841,    1189 },
    { QPrinter::A1,         QStringLiteral("A1"),       594,    841  },
    { QPrinter::A2,         QStringLiteral("A2"),       420,    594  },
    { QPrinter::A3,         QStringLiteral("A3"),       297,    420  },
    { QPrinter::A4,         QStringLiteral("A4"),       210,    297  },
    { QPrinter::A5,         QStringLiteral("A5"),       148,    210  },
    { QPrinter::A6,         QStringLiteral("A6"),       105,    148  },
    { QPrinter::A7,         QStringLiteral("A7"),       74,     105  },
    { QPrinter::A8,         QStringLiteral("A8"),       52,     74   },
    { QPrinter::A9,         QStringLiteral("A9"),       37,     52   },
    { QPrinter::B0,         QStringLiteral("B0"),       1000,   1414 },
    { QPrinter::B1,         QStringLiteral("B1"),       707,    1000 },
    { QPrinter::B2,         QStringLiteral("B2"),       500,    707  },
    { QPrinter::B3,         QStringLiteral("B3"),       353,    500  },
    { QPrinter::B4,         QStringLiteral("B4"),       250,    353  },
    { QPrinter::B5,         QStringLiteral("B5"),       176,    250  },
    { QPrinter::B6,         QStringLiteral("B6"),       125,    176  },
    { QPrinter::B7,         QStringLiteral("B7"),       88,     125  },
    { QPrinter::B8,         QStringLiteral("B8"),       62,     88   },
    { QPrinter::B9,         QStringLiteral("B9"),       44,     62   },
    { QPrinter::B10 ,       QStringLiteral("B10"),      31,     44   },
    { QPrinter::C5E,        QStringLiteral("C5E"),      163,    229  },
    { QPrinter::Comm10E,    QStringLiteral("Comm10E"),  105,    241  },
    { QPrinter::DLE,        QStringLiteral("DLE"),      110,    220  },
    { QPrinter::Executive,  QStringLiteral("Executive"),191,    254  },
    { QPrinter::Folio,      QStringLiteral("Folio"),    210,    330  },
    { QPrinter::Ledger,     QStringLiteral("Ledger"),   432,    279  },
    { QPrinter::Legal,      QStringLiteral("Legal"),    216,    356  },
    { QPrinter::Letter,     QStringLiteral("Letter"),   216,    279  },
    { QPrinter::Tabloid,    QStringLiteral("Tabloid"),  279,    432  },
    { QPrinter::Custom,     QStringLiteral("Unknown"),  0,      0    }
};


QPrinter::PaperSize PaperSizes::size(const QString &name)
{
    int i = 0;

    while (PaperSizeList[i].size != QPrinter::Custom) {
        if (PaperSizeList[i].name == name) {
            break;
        }

        ++i;
    }

    return PaperSizeList[i].size; // will return QPrinter::Custom if name isn't found
}


QString PaperSizes::name(QPrinter::PaperSize size)
{
    int i = 0;

    while (PaperSizeList[i].size != QPrinter::Custom) {
        if (PaperSizeList[i].size == size) {
            break;
        }

        ++i;
    }

    return PaperSizeList[i].name;
}


int PaperSizes::width(QPrinter::PaperSize size, QPrinter::Orientation orientation)
{
    int i = 0;

    while (PaperSizeList[i].size != QPrinter::Custom) {
        if (PaperSizeList[i].size == size) {
            break;
        }

        ++i;
    }

    return (orientation == QPrinter::Portrait) ? PaperSizeList[i].width : PaperSizeList[i].height;
}


int PaperSizes::height(QPrinter::PaperSize size, QPrinter::Orientation orientation)
{
    int i = 0;

    while (PaperSizeList[i].size != QPrinter::Custom) {
        if (PaperSizeList[i].size == size) {
            break;
        }

        ++i;
    }

    return (orientation == QPrinter::Portrait) ? PaperSizeList[i].height : PaperSizeList[i].width;
}
