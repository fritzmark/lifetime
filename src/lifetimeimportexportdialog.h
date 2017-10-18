/***************************************************************************
 *   Copyright (C) 2007 by Christoph Maciejewski   *
 *   chrmac@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef LIFETIMEIMPORTEXPORTDIALOG_H
#define LIFETIMEIMPORTEXPORTDIALOG_H

#include <QDialog>
#include "ui_lifetimeimportexportdialog.h"
#include "LifetimeMainWindow.h"

#define MODE_IMPORT 0
#define MODE_EXPORT 1

/**
	@author Christoph Maciejewski <chrmac@gmx.de>
*/
class LifetimeImportExportDialog : public QDialog, protected Ui_ImportExportDialog
{
Q_OBJECT
public:
    LifetimeImportExportDialog(LifetimeMainWindow* _parent = 0);

    ~LifetimeImportExportDialog();
    void showImport();
    void showExport(QString _sourceFile = "");

private:
    LifetimeMainWindow* parentGUI;
    unsigned int mode;

private slots:
    void goClicked();
    void openFile();
    void saveFile();
};

#endif
