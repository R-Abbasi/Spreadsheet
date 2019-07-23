#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    FindDialog(QWidget* parent = nullptr);

signals:
    void findNext(const QString&, Qt::CaseSensitivity);
    void findPrevious(const QString&, Qt::CaseSensitivity);

private slots:
    void findClicked();
    void enableFindButton(const QString&);

private:
    QLabel* label;
    QLineEdit* lineEdit;
    QCheckBox* caseCheckBox;
    QCheckBox* backwardCheckBox;
    QPushButton* findButton;
    QPushButton* closeButton;
};

#endif // FINDDIALOG_H

