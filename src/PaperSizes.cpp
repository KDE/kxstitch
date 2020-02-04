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


const struct PageSize {
    QPageSize::PageSizeId   size;
    QString                 name;
    int                     width;
    int                     height;
} PageSizeList[] = {
    { QPageSize::A0,         QStringLiteral("A0"),       841,    1189 },
    { QPageSize::A1,         QStringLiteral("A1"),       594,    841  },
    { QPageSize::A2,         QStringLiteral("A2"),       420,    594  },
    { QPageSize::A3,         QStringLiteral("A3"),       297,    420  },
    { QPageSize::A4,         QStringLiteral("A4"),       210,    297  },
    { QPageSize::A5,         QStringLiteral("A5"),       148,    210  },
    { QPageSize::A6,         QStringLiteral("A6"),       105,    148  },
    { QPageSize::A7,         QStringLiteral("A7"),       74,     105  },
    { QPageSize::A8,         QStringLiteral("A8"),       52,     74   },
    { QPageSize::A9,         QStringLiteral("A9"),       37,     52   },
    { QPageSize::B0,         QStringLiteral("B0"),       1000,   1414 },
    { QPageSize::B1,         QStringLiteral("B1"),       707,    1000 },
    { QPageSize::B2,         QStringLiteral("B2"),       500,    707  },
    { QPageSize::B3,         QStringLiteral("B3"),       353,    500  },
    { QPageSize::B4,         QStringLiteral("B4"),       250,    353  },
    { QPageSize::B5,         QStringLiteral("B5"),       176,    250  },
    { QPageSize::B6,         QStringLiteral("B6"),       125,    176  },
    { QPageSize::B7,         QStringLiteral("B7"),       88,     125  },
    { QPageSize::B8,         QStringLiteral("B8"),       62,     88   },
    { QPageSize::B9,         QStringLiteral("B9"),       44,     62   },
    { QPageSize::B10 ,       QStringLiteral("B10"),      31,     44   },
    { QPageSize::C5E,        QStringLiteral("C5E"),      163,    229  },
    { QPageSize::Comm10E,    QStringLiteral("Comm10E"),  105,    241  },
    { QPageSize::DLE,        QStringLiteral("DLE"),      110,    220  },
    { QPageSize::Executive,  QStringLiteral("Executive"),191,    254  },
    { QPageSize::Folio,      QStringLiteral("Folio"),    210,    330  },
    { QPageSize::Ledger,     QStringLiteral("Ledger"),   432,    279  },
    { QPageSize::Legal,      QStringLiteral("Legal"),    216,    356  },
    { QPageSize::Letter,     QStringLiteral("Letter"),   216,    279  },
    { QPageSize::Tabloid,    QStringLiteral("Tabloid"),  279,    432  },
    { QPageSize::Custom,     QStringLiteral("Unknown"),  0,      0    }
};


QPageSize::PageSizeId PageSizes::size(const QString &name)
{
    int i = 0;

    while (PageSizeList[i].size != QPageSize::Custom) {
        if (PageSizeList[i].name == name) {
            break;
        }

        ++i;
    }

    return PageSizeList[i].size; // will return QPageSize::Custom if name isn't found
}


QString PageSizes::name(QPageSize::PageSizeId size)
{
    int i = 0;

    while (PageSizeList[i].size != QPageSize::Custom) {
        if (PageSizeList[i].size == size) {
            break;
        }

        ++i;
    }

    return PageSizeList[i].name;
}


int PageSizes::width(QPageSize::PageSizeId size, QPageLayout::Orientation orientation)
{
    int i = 0;

    while (PageSizeList[i].size != QPageSize::Custom) {
        if (PageSizeList[i].size == size) {
            break;
        }

        ++i;
    }

    return (orientation == QPageLayout::Portrait) ? PageSizeList[i].width : PageSizeList[i].height;
}


int PageSizes::height(QPageSize::PageSizeId size, QPageLayout::Orientation orientation)
{
    int i = 0;

    while (PageSizeList[i].size != QPageSize::Custom) {
        if (PageSizeList[i].size == size) {
            break;
        }

        ++i;
    }

    return (orientation == QPageLayout::Portrait) ? PageSizeList[i].height : PageSizeList[i].width;
}
