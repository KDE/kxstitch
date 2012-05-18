/********************************************************************************
 * Copyright (C) 2010 by Stephen Allewell                                       *
 * stephen@mirramar.adsl24.co.uk                                                *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


#include "PaperSizes.h"


static struct PaperSize
{
    QPrinter::PaperSize size;
    const char          *name;
    int                 width;
    int                 height;
} PaperSizeList[] =
{
    { QPrinter::A0,         "A0",       841,    1189 },
    { QPrinter::A1,         "A1",       594,    841  },
    { QPrinter::A2,         "A2",       420,    594  },
    { QPrinter::A3,         "A3",       297,    420  },
    { QPrinter::A4,         "A4",       210,    297  },
    { QPrinter::A5,         "A5",       148,    210  },
    { QPrinter::A6,         "A6",       105,    148  },
    { QPrinter::A7,         "A7",       74,     105  },
    { QPrinter::A8,         "A8",       52,     74   },
    { QPrinter::A9,         "A9",       37,     52   },
    { QPrinter::B0,         "B0",       1000,   1414 },
    { QPrinter::B1,         "B1",       707,    1000 },
    { QPrinter::B2,         "B2",       500,    707  },
    { QPrinter::B3,         "B3",       353,    500  },
    { QPrinter::B4,         "B4",       250,    353  },
    { QPrinter::B5,         "B5",       176,    250  },
    { QPrinter::B6,         "B6",       125,    176  },
    { QPrinter::B7,         "B7",       88,     125  },
    { QPrinter::B8,         "B8",       62,     88   },
    { QPrinter::B9,         "B9",       44,     62   },
    { QPrinter::B10 ,       "B10",      31,     44   },
    { QPrinter::C5E,        "C5E",      163,    229  },
    { QPrinter::Comm10E,    "Comm10E",  105,    241  },
    { QPrinter::DLE,        "DLE",      110,    220  },
    { QPrinter::Executive,  "Executive",191,    254  },
    { QPrinter::Folio,      "Folio",    210,    330  },
    { QPrinter::Ledger,     "Ledger",   432,    279  },
    { QPrinter::Legal,      "Legal",    216,    356  },
    { QPrinter::Letter,     "Letter",   216,    279  },
    { QPrinter::Tabloid,    "Tabloid",  279,    432  },
    { QPrinter::Custom,     "Unknown",  0,      0    }
};


QPrinter::PaperSize PaperSizes::size(const QString &name)
{
    int i = 0;
    while (PaperSizeList[i].size != QPrinter::Custom)
    {
        if (PaperSizeList[i].name == name)
            break;
        ++i;
    }
    return PaperSizeList[i].size; // will return QPrinter::Custom if name isn't found
}


QString PaperSizes::name(QPrinter::PaperSize size)
{
    int i = 0;
    while (PaperSizeList[i].size != QPrinter::Custom)
    {
        if (PaperSizeList[i].size == size)
            break;
        ++i;
    }
    return QString(PaperSizeList[i].name);
}


int PaperSizes::width(QPrinter::PaperSize size, QPrinter::Orientation orientation)
{
    int i = 0;
    while (PaperSizeList[i].size != QPrinter::Custom)
    {
        if (PaperSizeList[i].size == size)
            break;
        ++i;
    }
    return (orientation == QPrinter::Portrait)?PaperSizeList[i].width:PaperSizeList[i].height;
}


int PaperSizes::height(QPrinter::PaperSize size, QPrinter::Orientation orientation)
{
    int i = 0;
    while (PaperSizeList[i].size != QPrinter::Custom)
    {
        if (PaperSizeList[i].size == size)
            break;
        ++i;
    }
    return (orientation == QPrinter::Portrait)?PaperSizeList[i].height:PaperSizeList[i].width;
}
