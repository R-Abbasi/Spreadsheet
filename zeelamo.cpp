#include <QtWidgets>

#include "finddialog.h"
#include "gotocelldialog.h"
#include "zeelamo.h"
#include "sortdialog.h"
#include "spreadsheet.h"

QStringList Zeelamo::recentFiles;
QSet<QString> Zeelamo::ss;

Zeelamo::Zeelamo(QWidget *parent)
    : QMainWindow(parent)
{
    spreadsheet = new Spreadsheet;
    setCentralWidget(spreadsheet);

    createActions();
    createMenus();
    createContextMenu();
    createToolBars();
    createStatusBar(); // Finished (HERE)

    readSettings();

    findDialog = nullptr;

    setAttribute(Qt::WA_DeleteOnClose);  // delete the window when it's closed
    setWindowIcon(QIcon(":/images/Spreadsheet.ico"));
    setCurrentFile("");
}

//**********************************************

void Zeelamo::closeEvent(QCloseEvent *event)
{
    if(okToContinue())
    {
        writeSettings();
        remove_File();
        event->accept();
    }
    else event->ignore();
}

//***********************************************

void Zeelamo::newFile()
{
    Zeelamo* myZl = new Zeelamo;
    myZl->show();

    /*if (okToContinue()) {
        spreadsheet->clear();
        setCurrentFile("");
    }
    */
}

//****************************************

void Zeelamo::open()
{
    if(okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open Zeelamo"),".",
                                                        tr("Zeelamo files (*.sp)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

//*****************************************************

bool Zeelamo::save()
{
    if(curFile.isEmpty())
        return saveAs();
    else return saveFile(curFile);
}

//******************************************

bool Zeelamo::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Zeelamo"), ".",
                                                    tr("Zeelamo files (*.sp)"));

    if(fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

//*****************************************************

void Zeelamo::find()
{
    if(!findDialog)
    {
        findDialog = new FindDialog(this);
        connect(findDialog, &FindDialog::findNext, spreadsheet,
                &Spreadsheet::findNext);
        connect(findDialog, &FindDialog::findPrevious, spreadsheet,
                &Spreadsheet::findPrevious);
    }

    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

//*******************************************

void Zeelamo::goToCell()
{
    GoToCellDialog dialog(this);
    if (dialog.exec()) {
        QString str = dialog.lineEdit->text().toUpper();
        spreadsheet->setCurrentCell(str.mid(1).toInt() - 1,
                                    str[0].unicode() - 'A');
    }
}

//***************************************

void Zeelamo::sort()
{
    SortDialog dialog(this);
    QTableWidgetSelectionRange range = spreadsheet->selectedRange();
    dialog.setColumnRange('A' + range.leftColumn(),
                          'A' + range.rightColumn());

    if (dialog.exec())

    {
        SpreadsheetCompare compare;
        compare.keys[0] =
                dialog.primaryColumnCombo->currentIndex();
        compare.keys[1] =
                dialog.secondaryColumnCombo->currentIndex() - 1;
        compare.keys[2] =
                dialog.tertiaryColumnCombo->currentIndex() - 1;
        compare.ascending[0] =
                (dialog.primaryOrderCombo->currentIndex() == 0);
        compare.ascending[1] =
                (dialog.secondaryOrderCombo->currentIndex() == 0);
        compare.ascending[2] =
                (dialog.tertiaryOrderCombo->currentIndex() == 0);
        spreadsheet->sort(compare);
    }
}

//**************************************************

void Zeelamo::about()
{
    QMessageBox::about(this, tr("About Zeelamo"),
                       tr("<h2>Zeelamo Version 2</h2>"
                          "<p>Copyright &copy; 2019 Software Inc."
                          "<p>Zeelamo is a small spreadsheet application written by S.Rahmat Abbasi "
                          "(s.rabbasi@yahoo.com, Telegram: @Abbasi_Dev), that demonstrates QAction, QMainWindow, QMenuBar,"
                          "QStatusBar, QTableWidget, QToolBar, and many"
                          " Qt classes."));
}

//**********************************************************

void Zeelamo::openRecentFile()
{
    if(okToContinue()) {
        QAction* action = qobject_cast<QAction*>(sender());
        if(action)
            loadFile(action->data().toString());
    }
}

//*********************************

void Zeelamo::updateStatusBar()
{
    locationLabel->setText(spreadsheet -> currentLocation());
    formulaLabel->setText(spreadsheet -> currentFormula());
}

//****************************************

void Zeelamo::spreadsheetModified()
{
    setWindowModified(true);
    updateStatusBar();
}

//*****************************************************

void Zeelamo::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction -> setIcon(QIcon(":/images/new.png"));
    newAction -> setShortcut(QKeySequence::New);
    newAction -> setStatusTip(tr("Create a new spreadsheet file"));
    connect(newAction, &QAction::triggered, this, &Zeelamo::newFile);

    openAction = new QAction(tr("&Open..."), this);
    openAction -> setIcon(QIcon(":/images/open.png"));
    openAction -> setShortcut(QKeySequence::Open);
    openAction -> setStatusTip(tr("Open an existing spreadsheet file"));
    connect(openAction, &QAction::triggered, this, &Zeelamo::open);

    saveAction = new QAction(tr("&Save"), this);
    saveAction -> setIcon(QIcon(":/images/save.png"));
    saveAction -> setShortcut(QKeySequence::Save);
    saveAction -> setStatusTip(tr("Save the spreadsheet to disk"));
    connect(saveAction, &QAction::triggered, this, &Zeelamo::save);

    saveAsAction = new QAction(tr("Save &As..."),this);
    saveAsAction -> setStatusTip(tr("Save the spreadsheet"
                                    " under a new name"));
    connect(saveAsAction, &QAction::triggered, this, &Zeelamo::saveAs);

    for(int i=0; i<MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i] -> setVisible(false);
        connect(recentFileActions[i], &QAction::triggered,
                this, &Zeelamo::openRecentFile);
    }

    closeAction = new QAction(tr("&Close"), this);
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip(tr("Close this window"));
    connect(closeAction, &QAction::triggered, this, &Zeelamo::close);

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, &QAction::triggered,
            qApp, &QApplication::closeAllWindows);

    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setIcon(QIcon(":/images/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut the current selection's contents "
                               "to the clipboard"));
    connect(cutAction, &QAction::triggered, spreadsheet, &Spreadsheet::cut);

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":/images/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy the current selection's contents "
                                "to the clipboard"));
    connect(copyAction, &QAction::triggered, spreadsheet, &Spreadsheet::copy);

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction -> setIcon(QIcon(":/images/paste.png"));
    pasteAction -> setShortcut(QKeySequence::Paste);
    pasteAction -> setStatusTip(tr("Paste the clipboard's contents into "
                                   "the current selection"));
    connect(pasteAction, &QAction::triggered, spreadsheet, &Spreadsheet::cut);

    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip(tr("Delete the current selection's "
                                  "contents"));
    connect(deleteAction, &QAction::triggered, spreadsheet, &Spreadsheet::del);

    sumAction = new QAction(tr("&Sum"), this);
    sumAction->setStatusTip(tr("Sum the current selection's "
                               "contents"));
    connect(sumAction, &QAction::triggered, spreadsheet, &Spreadsheet::sum);

    aveAction = new QAction(tr("&Average"), this);
    aveAction->setStatusTip(tr("Average the current selection's "
                               "contents"));
    connect(aveAction, &QAction::triggered, spreadsheet, &Spreadsheet::ave);

    countAction = new QAction(tr("&Count"), this);
    countAction->setStatusTip(tr("Count the current selection's "
                                 "contents"));
    connect(countAction, &QAction::triggered, spreadsheet, &Spreadsheet::count);

    maxAction = new QAction(tr("&Max"), this);
    maxAction->setStatusTip(tr("Show the maximum number in the current "
                               "selection's contents"));
    connect(maxAction, &QAction::triggered, spreadsheet, &Spreadsheet::max);

    minAction = new QAction(tr("&Min"), this);
    minAction->setStatusTip(tr("Show the minimum number in the current "
                               "selection's contents"));
    connect(minAction, &QAction::triggered, spreadsheet, &Spreadsheet::min);

    selectRowAction = new QAction(tr("&Row"), this);
    selectRowAction->setStatusTip(tr("Select all the cells in the "
                                     "current row"));
    connect(selectRowAction, &QAction::triggered, spreadsheet,
            &Spreadsheet::selectCurrentRow);

    selectColumnAction = new QAction(tr("&Column"), this);
    selectColumnAction->setStatusTip(tr("Select all the cells in the "
                                        "current column"));
    connect(selectColumnAction, &QAction::triggered, spreadsheet,
            &Spreadsheet::selectCurrentColumn);

    selectAllAction = new QAction(tr("&All"), this);
    selectAllAction -> setShortcut(QKeySequence::SelectAll);
    selectAllAction -> setStatusTip(tr("Select all the cells in the"
                                       "spreadsheet"));
    connect(selectAllAction, &QAction::triggered, spreadsheet,
            &Spreadsheet::selectAll);

    findAction = new QAction(tr("&Find..."), this);
    findAction->setIcon(QIcon(":/images/find.png"));
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip(tr("Find a matching cell"));
    connect(findAction, &QAction::triggered, this, &Zeelamo::find);

    goToCellAction = new QAction(tr("&Go to Cell..."), this);
    goToCellAction->setIcon(QIcon(":/images/gotocell.png"));
    goToCellAction->setShortcut(tr("Ctrl+G"));
    goToCellAction->setStatusTip(tr("Go to the specified cell"));
    connect(goToCellAction, &QAction::triggered, this, &Zeelamo::goToCell);

    recalculateAction = new QAction(tr("&Recalculate"), this);
    recalculateAction->setShortcut(tr("F9"));
    recalculateAction->setStatusTip(tr("Recalculate all the "
                                       "Zeelamo's formulas"));
    connect(recalculateAction, &QAction::triggered, spreadsheet, &Spreadsheet::recalculate);

    sortAction = new QAction(tr("&Sort..."), this);
    sortAction->setStatusTip(tr("Sort the selected cells or all the "
                                "cells"));
    connect(sortAction, &QAction::triggered, this, &Zeelamo::sort);


    showGridAction = new QAction(tr("&Show Grid"), this);
    showGridAction -> setCheckable(true);
    showGridAction->setChecked(spreadsheet->showGrid());

    showGridAction->setStatusTip(tr("Show or hide the Zeelamo's"
                                    " grid"));
    connect(showGridAction, &QAction::triggered, spreadsheet, &Spreadsheet::setShowGrid);

    autoRecalcAction = new QAction(tr("&Auto-Recalculate"), this);
    autoRecalcAction->setCheckable(true);
    autoRecalcAction->setChecked(spreadsheet->autoRecalculate());

    autoRecalcAction->setStatusTip(tr("Switch auto-recalculation on or "
                                      "off"));
    connect(autoRecalcAction, &QAction::toggled, spreadsheet, &Spreadsheet::setAutoRecalculate);

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, &QAction::triggered, this, &Zeelamo::about);

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction -> setStatusTip(tr("Sow the Qt library's"
                                     " About box"));
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

//**************************************************************


void Zeelamo::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    separatorAction = fileMenu->addSeparator();
    for(int i=0; i<MaxRecentFiles; ++i)
        fileMenu -> addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addAction(sumAction);
    editMenu->addAction(aveAction);
    editMenu->addAction(countAction);
    editMenu->addAction(maxAction);
    editMenu->addAction(minAction);

    selectSubMenu = editMenu->addMenu(tr("&Select"));
    selectSubMenu->addAction(selectRowAction);
    selectSubMenu->addAction(selectColumnAction);
    selectSubMenu->addAction(selectAllAction);

    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addAction(goToCellAction);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(recalculateAction);
    toolsMenu->addAction(sortAction);

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(showGridAction);
    optionsMenu->addAction(autoRecalcAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

//*****************************************************

void Zeelamo::createContextMenu()
{
    spreadsheet->addAction(cutAction);
    spreadsheet->addAction(copyAction);
    spreadsheet->addAction(pasteAction);
    spreadsheet->addAction(sumAction);
    spreadsheet->addAction(aveAction);
    spreadsheet->addAction(countAction);
    spreadsheet->addAction(maxAction);
    spreadsheet->addAction(minAction);
    spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
    //spreadsheet->setCursor(Qt::IBeamCursor);
}

//********************************************

void Zeelamo::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();
    editToolBar->addAction(findAction);
    editToolBar->addAction(goToCellAction);
}

//**********************************************

void Zeelamo::createStatusBar()
{
    locationLabel = new QLabel(" W999 ");
    locationLabel -> setAlignment(Qt::AlignHCenter);
    locationLabel -> setMinimumSize(locationLabel->sizeHint());

    formulaLabel = new QLabel;
    formulaLabel -> setIndent(3);

    statusBar() -> addWidget(locationLabel);
    statusBar() -> addWidget(formulaLabel, 2);

    connect(spreadsheet, SIGNAL(currentCellChanged(int, int, int ,int))
            , this, SLOT(updateStatusBar()));
    connect(spreadsheet, SIGNAL(modified()),
            this, SLOT(spreadsheetModified()));
    updateStatusBar();
}

//******************************************

void Zeelamo::readSettings()
{
    QSettings settings("Abbasi Inc.", "Zeelamo");
    restoreGeometry(settings.value("geometry").toByteArray());
    recentFiles = settings.value("recentFiles").toStringList();

    foreach(QWidget* win, QApplication::topLevelWidgets())
        if(Zeelamo* mainWin = qobject_cast<Zeelamo*>(win))
            mainWin -> updateRecentFileActions();

    bool showGrid = settings.value("showGrid", true).toBool();
    showGridAction->setChecked(showGrid);
    bool autoRecalc = settings.value("autoRecalc", true).toBool();
    autoRecalcAction->setChecked(autoRecalc);
}

//*************************************************************

void Zeelamo::writeSettings()
{
    QSettings settings("Abbasi Inc.", "Zeelamo");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("showGrid", showGridAction->isChecked());
    settings.setValue("autoRecalc", autoRecalcAction->isChecked());
}

//***********************************************************

bool Zeelamo::okToContinue()
{
    if(isWindowModified())
    {
        int r = QMessageBox::warning(this, tr("Zeelamo"),
                                     tr("The document has been modified.\n"
                                        "Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::No
                                     | QMessageBox::Cancel);
        if(r == QMessageBox::Yes)
            return save();
        else if(r == QMessageBox::Cancel)
            return false;
    }
    return true;
}

//****************************************************

void Zeelamo::remove_File()
{
    ss.remove(strippedName(curFile));
}

//*****************************************************

bool Zeelamo::loadFile(const QString &fileName)
{
    if(ss.contains(strippedName(fileName)))
        return false;
    else ss.insert(strippedName(fileName));

    if (!spreadsheet->readFile(fileName)) {
        statusBar()->showMessage(tr("Loading canceled"), 3000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 3000);
    return true;
}

//*********************************************

bool Zeelamo::saveFile(const QString &fileName)
{
    if(!spreadsheet->writeFile(fileName))
    {
        statusBar()->showMessage(tr("Saving canceled"), 3000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 3000);
    return true;
}

//***************************************************

void Zeelamo::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);
    QString showName = tr("Untitled");

    if(!curFile.isEmpty()){
        showName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);

        foreach(QWidget* win, QApplication::topLevelWidgets())
            if(Zeelamo* mainWin = qobject_cast<Zeelamo*>(win))
                mainWin -> updateRecentFileActions();
    }

    setWindowTitle(tr("%1[*] - %2").arg(showName)
                   .arg(tr("Zeelamo")));
}

//*******************************************

void Zeelamo::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);

    while(i.hasNext())
        if(!QFile::exists(i.next()))
            i.remove();

    for(int j=0; j<MaxRecentFiles; ++j)
        if(j < recentFiles.count()) {
            QString text = tr("&%1 %2")
                    .arg(j+1)
                    .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }
        else  recentFileActions[j]->setVisible(false);

    separatorAction->setVisible(!recentFiles.isEmpty());
}

//***************************************

QString Zeelamo::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//**************************

Zeelamo::~Zeelamo()
{ }






