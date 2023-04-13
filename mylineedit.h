#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QObject>
#include <QLineEdit>
#include <QMouseEvent>

#include "ZyHtmlUtil.h"

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget *parent = nullptr);

signals:

protected:
    void mouseMoveEvent(QMouseEvent *e);

};

#endif // MYLINEEDIT_H
