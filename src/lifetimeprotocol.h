#ifndef LIFETIMEPROTOCOL_H
#define LIFETIMEPROTOCOL_H

#include <QDialog>
#include "LifetimeMainWindow.h"
#include "ui_lifetimeprotocolview.h"

class LifetimeMainWindow;

/**
Handles all jobs for viewing and editing of the protocol.

    @author Christoph Maciejewski <chrmac@gmx.de>
*/
class LifetimeProtocol : public QDialog
{
    Q_OBJECT
public:
    LifetimeProtocol(LifetimeMainWindow *parent);
    ~LifetimeProtocol();
    bool writeToFile();
    bool openFile(QString path);

private:
    LifetimeMainWindow* parentGui;
    Ui::LifetimeProtocolDialog *ui;
    bool isEmpty;
    bool addRow(QString content, int row);
    void clearTable();
    void calculateRow(int row);

private slots:
    void tableCellChanged(int row, int column);
    void calcOvertime();
    void removeSelectedRows();
    bool saveChanges(QString outputFile);
    void openProtocolFile();
    void saveProtocolAs();
    void addEmptyRow();
    void saveChanges();
    bool showImportDialog();
    bool showExportDialog();
    void okClicked();

protected:
    bool isLoading;
    QString fileToLoad;
    void closeEvent(QCloseEvent *event);

private:
    bool isRowValid(int row);

};

#endif
