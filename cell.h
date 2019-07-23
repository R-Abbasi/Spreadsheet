#ifndef CELL_H
#define CELL_H

#include <QTableWidgetItem>

class Cell : public QTableWidgetItem
{
public:
    Cell();

    QTableWidgetItem* clone() const;
    void setData(int, const QVariant&);
    QVariant data(int) const;
    void setFormula(const QString&);
    QString formula() const;
    void setDirty();

private:
    QVariant value() const;
    QVariant evalExpression(const QString&, int&) const;
    QVariant evalTerm(const QString&, int&) const;
    QVariant evalFactor(const QString&, int&) const;

    mutable QVariant cachedValue;
    mutable bool cacheIsDirty;
};

#endif
