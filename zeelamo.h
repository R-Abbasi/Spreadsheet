#ifndef ZEELAMO_H
#define ZEELAMO_H

#include <QMainWindow>

class QAction;
class QLabel;
class FindDialog;
class Spreadsheet;

class Zeelamo : public QMainWindow
{
    Q_OBJECT

public:
    Zeelamo(QWidget *parent = nullptr);
    ~Zeelamo();

    bool loadFile(const QString&);

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void find();
    void goToCell();
    void sort();
    void about();
    void openRecentFile();
    void updateStatusBar();
    void spreadsheetModified();
    void remove_File();

private:
    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool saveFile(const QString&);
    void setCurrentFile(const QString&);
    void updateRecentFileActions();
    QString strippedName(const QString&);

    Spreadsheet* spreadsheet;
    FindDialog* findDialog;
    QLabel* locationLabel;
    QLabel* formulaLabel;
    static QStringList recentFiles;
    QString curFile;
    static QSet<QString> ss;

    enum { MaxRecentFiles = 5 };
    QAction* recentFileActions[MaxRecentFiles];
    QAction* separatorAction;


    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* selectSubMenu;
    QMenu* toolsMenu;
    QMenu* optionsMenu;
    QMenu* helpMenu;

    QToolBar* fileToolBar;
    QToolBar* editToolBar;

    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QAction* showGridAction;
    QAction* aboutQtAction;
    QAction* cutAction;
    QAction* copyAction;
    QAction* pasteAction;
    QAction* deleteAction;
    QAction* selectRowAction;
    QAction* selectColumnAction;
    QAction* selectAllAction;
    QAction* findAction;
    QAction* goToCellAction;
    QAction* recalculateAction;
    QAction* sortAction;
    QAction* autoRecalcAction;
    QAction* aboutAction;
    QAction* sumAction;
    QAction* aveAction;
    QAction* countAction;
    QAction* maxAction;
    QAction* minAction;
    QAction* closeAction;
    QAction* exitAction;
};

#endif // ZEELAMO_H
