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
#include <QMessageBox>
#include <QFileDialog>
#include "lifetimeimportexportdialog.h"


LifetimeImportExportDialog::LifetimeImportExportDialog(LifetimeMainWindow* _parent)
 : QDialog(_parent)
{
    this->setupUi(this);
    this->parentGUI = _parent;
    this->pushButton_go->setEnabled(false);

    connect(this->pushButton_go, SIGNAL(clicked()), this, SLOT(goClicked()));
    connect(this->pushButton_input, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(this->pushButton_output, SIGNAL(clicked()), this, SLOT(saveFile()));
}


LifetimeImportExportDialog::~LifetimeImportExportDialog()
{
}




/*!
    \fn LifetimeImportExportDialog::showExport()
 */
void LifetimeImportExportDialog::showExport(QString _sourceFile)
{
    this->mode = MODE_EXPORT;
    this->groupBox_settings->setTitle(tr("Export Settings"));
    this->setWindowTitle(tr("Export"));

    if(_sourceFile != "") {
        this->lineEdit_input->setText(_sourceFile);
    }

    this->exec();
}


/*!
    \fn LifetimeImportExportDialog::showImport()
 */
void LifetimeImportExportDialog::showImport()
{
    this->mode = MODE_IMPORT;
    this->groupBox_settings->setTitle(tr("Import Settings"));
    this->setWindowTitle(tr("Import"));

    this->exec();
}


/*!
    \fn LifetimeImportExportDialog::goClicked()
 */
void LifetimeImportExportDialog::goClicked()
{
    QString infile = this->lineEdit_input->text();
    QString outfile = this->lineEdit_output->text();

    // Check if infile is readable
    QFileInfo fi(infile);
    if(!fi.isReadable()) {
        QMessageBox::warning(this, tr("Can't read input file"), tr("Please select another input file."));
        return;
    }
    
    this->hide();
}


/*!
    \fn LifetimeImportExportDialog::openFile(int _type)
 */
void LifetimeImportExportDialog::openFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Protocol File"));

    if(!file.isNull()) {
        this->lineEdit_input->setText(file);
    }
}


/*!
    \fn LifetimeImportExportDialog::saveFile()
 */
void LifetimeImportExportDialog::saveFile()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Select output file"));

    if(!file.isNull()) {
        this->lineEdit_output->setText(file);
    }
}
