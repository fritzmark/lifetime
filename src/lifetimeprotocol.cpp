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

#include <QFile>
#include <QTextStream>
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include "lifetimeprotocol.h"
#include "lifetimecore.h"
#include "lifetimeimportexportdialog.h"

LifetimeProtocol::LifetimeProtocol(LifetimeMainWindow *parent)
 : QDialog(parent)
{
    this->parentGui = parent;
    this->isLoading = true;
    this->fileToLoad = "";
    this->isEmpty = true;
    this->setWindowTitle(tr("Protocol"));

    Ui::LifetimeProtocolDialog* newUi = new Ui::LifetimeProtocolDialog();
    newUi->setupUi(this);

    this->ui = newUi;
    //this->ui.tableWidget->setCellWidget(0,0,new QTimeEdit());

    QObject::connect(this->ui->tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(tableCellChanged(int, int)));
    QObject::connect(this->ui->pushButton_reload, SIGNAL(clicked()), this, SLOT(calcOvertime()));
    QObject::connect(this->ui->pushButton_delete, SIGNAL(clicked()), this, SLOT(removeSelectedRows()));
    QObject::connect(this->ui->pushButton_OK, SIGNAL(clicked()), this, SLOT(okClicked()));
    QObject::connect(this->ui->pushButton_Cancel, SIGNAL(clicked()), this, SLOT(hide()));
    QObject::connect(this->ui->pushButton_open, SIGNAL(clicked()), this, SLOT(openProtocolFile()));
    QObject::connect(this->ui->pushButton_insert, SIGNAL(clicked()), this, SLOT(addEmptyRow()));
    QObject::connect(this->ui->pushButton_saveas, SIGNAL(clicked()), this, SLOT(saveProtocolAs()));
    QObject::connect(this->ui->pushButton_import, SIGNAL(clicked()), this, SLOT(showImportDialog()));
    QObject::connect(this->ui->pushButton_export, SIGNAL(clicked()), this, SLOT(showExportDialog()));


    this->ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}


LifetimeProtocol::~LifetimeProtocol()
{
}

void LifetimeProtocol::closeEvent(QCloseEvent *event)
{
    event->ignore();
    qDebug("close");
    this->hide();
}


/*!
    \fn LifetimeProtocol::writeToFile()
 */
bool LifetimeProtocol::writeToFile()
{
    LifetimeConfig *conf = this->parentGui->getTheConfig();
    LifetimeCore *core = this->parentGui->getTheCore();

    QFile file(conf->protocolFile);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug("Can't open File!");
        return false;
    }

    QTextStream out(&file);
    QChar delim = conf->protocolSeparator;


    out << core->startTime->toString("dd.MM.yy (ddd)") << delim \
            << core->startTime->toString("hh:mm:ss") << delim \
            << QDateTime::currentDateTime().toString("hh:mm:ss") << delim \
            << core->sumOfPauses/60 << delim \
            << conf->getWorkPerDay(core->startTime->date().dayOfWeek()-1) << delim \
            << core->getStatusString(true) << delim \
            << conf->jobDescription \
            << "\n";

    file.close();

    return true;
}


/*!
    \fn LifetimeProtocol::openFile(QString path)
 */
bool LifetimeProtocol::openFile(QString path)
{
    this->isLoading = true;
    this->fileToLoad = path;

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("Can't open File!");
        this->isLoading = false;
        this->isEmpty = true;
        return false;
    }

    this->clearTable();
    int row = 0;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(!this->addRow(line, row)) {
            QString msg = tr("This file is not a valid protocol file!\n Found syntax error in row ");
            msg += QString::number(row + 1);
            QMessageBox::warning( this, tr("Invalid format"),
                                      msg);
            this->isLoading = false;
            this->isEmpty = true;
            this->setWindowTitle(tr("Incorrect format!"));
            return false;
        }
        row++;
    }

    this->ui->tableWidget->resizeColumnsToContents();
    this->isLoading = false;
    this->isEmpty = false;
    this->setWindowTitle(this->fileToLoad);
    this->calcOvertime();
    return true;
}


/*!
    \fn LifetimeProtocol::addRow(QString row)
 */
bool LifetimeProtocol::addRow(QString content, int row)
{
    QStringList list = content.split(this->parentGui->getTheConfig()->protocolSeparator);

    if(list.size() != 7) {
        return false;
    }

    this->isLoading = true;

    this->ui->tableWidget->insertRow(row);
    QString cell = "";

    for (int i = 0; i < list.size(); ++i) {
        cell = list.at(i);

        if(cell.isNull())
            cell = "";

        cell = cell.simplified();

        QTableWidgetItem *newItem = new QTableWidgetItem(cell);
        if(i == 5) {
            newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            if(cell[0] == '-')
                newItem->setForeground ( QBrush ( Qt::red ) );
            else
                newItem->setForeground ( QBrush ( Qt::darkGreen ) );
        }
        this->ui->tableWidget->setItem(row, i, newItem);
    }

    this->isLoading = false;
    return true;
}


/*!
    \fn LifetimeProtocol::clearTable()
 */
void LifetimeProtocol::clearTable()
{
    this->isLoading = true;

    int rows = this->ui->tableWidget->rowCount();
    qDebug() << "Rows:" << rows;
    for(int i = 0; i < rows; i++) {
        this->ui->tableWidget->removeRow(0);
    }

    this->isLoading = false;
}


/*!
    \fn LifetimeProtocol::calculateRow(int row)
 */
void LifetimeProtocol::calculateRow(int row)
{
    if(this->ui->tableWidget->rowCount() - 1 < row)
        return;

    if(!this->isRowValid(row))
        return;

    QTime come = QTime::fromString(this->ui->tableWidget->item(row, 1)->text(), "h:mm:ss");
    QTime gone = QTime::fromString(this->ui->tableWidget->item(row, 2)->text(), "h:mm:ss");

    long brutto = come.secsTo(gone);

    if(brutto < 0)
        brutto = 24 * 3600 + brutto;

    double netto = brutto - (this->ui->tableWidget->item(row, 3)->text().toLong())*60;
    double overtime = netto - (this->ui->tableWidget->item(row, 4)->text().toDouble())*3600;
    QChar sign = '+';

    if(overtime < 0) {
        overtime *= -1;
        sign = '-';
    }

    QString out = sign + LifetimeCore::decimalToTime(overtime/3600).toString("hh:mm:ss");
    this->ui->tableWidget->item(row, 5)->setText(out);

    if(sign == '-')
        this->ui->tableWidget->item(row, 5)->setForeground ( QBrush ( Qt::red ) );
    else
        this->ui->tableWidget->item(row, 5)->setForeground ( QBrush ( Qt::darkGreen ) );
}


/*!
    \fn LifetimeProtocol::TableCellChanged(int row, int column)
 */
void LifetimeProtocol::tableCellChanged(int row, int column)
{
    if(!this->isLoading) {
        this->calculateRow(row);
        qDebug() << "CellChanged: " << row << "/" << column;

        // Check, if the delimiter char is used
        QString tmpTxt = this->ui->tableWidget->item(row, column)->text();
        QChar tmpChar = this->parentGui->getTheConfig()->protocolSeparator;

        if(tmpTxt.contains(tmpChar)) {
            tmpTxt.replace(tmpChar, QChar((tmpChar.digitValue())+1));
            this->ui->tableWidget->item(row, column)->setText(tmpTxt);
        }

        this->calcOvertime();
    }
}


/*!
    \fn LifetimeProtocol::calcOvertime()
 */
void LifetimeProtocol::calcOvertime()
{
    if(this->isEmpty)
        return;

    int rows = this->ui->tableWidget->rowCount();
    double overtime = 0;
    QString curr = "";
    int mult = 1;
    QChar sign = '+';

    for(int i = 0; i < rows; i++) {
        if(this->ui->tableWidget->item(i, 5) != NULL)
            curr = this->ui->tableWidget->item(i, 5)->text();

        if(curr[0] == '-')
            mult = -1;
        else
            mult = 1;

        curr.remove(0, 1);
        overtime += mult * LifetimeCore::timeToDecimal(QTime::fromString(curr, "h:mm:ss"), false);
    }

    if(overtime < 0)
        sign = '-';

    qDebug() << "Overtime: " << overtime;
    this->ui->lineEdit_overtime->setText(LifetimeCore::decimalToTimeString(overtime));
}


/*!
    \fn LifetimeProtocol::removeSelectedRows()
 */
void LifetimeProtocol::removeSelectedRows()
{
    QList<QTableWidgetSelectionRange> list = this->ui->tableWidget->selectedRanges();
    // This variables are not used.
    // I just kicked them out. Dogo
    //int top, bottom;

    while(list.size() > 0) {
        this->ui->tableWidget->removeRow(list.at(0).topRow());
	//This is a little bit tricky. After deleting a selected row
	//the indexes of the remaining selected rows have to be rebuild.
	list = this->ui->tableWidget->selectedRanges();
	
    }
}


/*!
    \fn LifetimeProtocol::saveChanges()
 */
bool LifetimeProtocol::saveChanges(QString outputFile)
{
    if(this->isEmpty)
        return false;

    LifetimeConfig *conf = this->parentGui->getTheConfig();
    QFile file(outputFile);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug("Can't open File!");
        return false;
    }

    QTextStream out(&file);
    QChar delim = conf->protocolSeparator;
    QString tmp;

    int rows = this->ui->tableWidget->rowCount();

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j <= 5; j++) {
            if(this->ui->tableWidget->item(i, j) != NULL)
                out << this->ui->tableWidget->item(i, j)->text() << delim;
            else
                out << "" << delim;
        }

        if(this->ui->tableWidget->item(i, 6) != NULL)
            out << this->ui->tableWidget->item(i, 6)->text();

        out << '\n';
    }

    file.close();

    return true;
}


/*!
    \fn LifetimeProtocol::openProtocolFile()
 */
void LifetimeProtocol::openProtocolFile()
{
    LifetimeProtocol* lp = new LifetimeProtocol(this->parentGui);

    QString file = QFileDialog::getOpenFileName(this, tr("Select Protocol File"), this->fileToLoad);

    if(!file.isNull()) {
        lp->openFile(file);
        lp->show();
    }
}


/*!
    \fn LifetimeProtocol::addEmptyRow()
 */
void LifetimeProtocol::addEmptyRow()
{
    QList<QTableWidgetSelectionRange> list = this->ui->tableWidget->selectedRanges();

    if(list.size() > 0) {
        int row = list.at(0).topRow() + 1;
        qDebug() << "Adding row at " << row;
        QChar delim = this->parentGui->getTheConfig()->protocolSeparator;
        QString s = "-|00:00:00|00:00:00|-|-|-|-";
        s.replace(QString("|"), QString(delim));
        this->addRow(s, row);
    }
}


/*!
    \fn LifetimeProtocol::saveProtocolAs()
 */
void LifetimeProtocol::saveProtocolAs()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Select output file"), this->fileToLoad);

    if(!file.isNull()) {
        this->saveChanges(file);
    }
}


/*!
    \fn LifetimeProtocol::isRowValid(int row)
 */
bool LifetimeProtocol::isRowValid(int row)
{
    for(int i = 0; i <= 6; i++) {
        if(this->ui->tableWidget->item(row, i) == NULL)
            return false;
    }

    return true;
}


/*!
    \fn LifetimeProtocol::saveChanges()
 */
void LifetimeProtocol::saveChanges()
{
    this->saveChanges(this->fileToLoad);
}


/*!
    \fn LifetimeProtocol::okClicked()
 */
void LifetimeProtocol::okClicked()
{
    // Workaround for the Win32 bug: [ 1692956 ] Opening protocol from system tray
    this->saveChanges();
    this->hide();
}


/*!
    \fn LifetimeProtocol::showExportDialog()
 */
bool LifetimeProtocol::showExportDialog()
{
    LifetimeImportExportDialog* exportDialog =
            new LifetimeImportExportDialog(this->parentGui);
    exportDialog->showExport(this->fileToLoad);
    exportDialog->deleteLater();
    return false;
}


/*!
    \fn LifetimeProtocol::showImportDialog()
 */
bool LifetimeProtocol::showImportDialog()
{
    LifetimeImportExportDialog* exportDialog =
            new LifetimeImportExportDialog(this->parentGui);
    exportDialog->showImport();
    exportDialog->deleteLater();
    return false;
}
