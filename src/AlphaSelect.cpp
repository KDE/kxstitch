/***************************************************************************
 *   Copyright (C) 2003 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "AlphaSelect.h"

#include <QMouseEvent>

#include <KDebug>


AlphaSelect::AlphaSelect(QLabel *parent)
	:	QLabel(parent)
{
	resize(parent->size());
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setPixmap(*(parent->pixmap()));
	setCursor(QCursor(Qt::CrossCursor));
}


void AlphaSelect::mouseReleaseEvent(QMouseEvent *event)
{
	emit clicked(event->pos());
}
