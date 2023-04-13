#include "mylineedit.h"
#include <QDebug>

MyLineEdit::MyLineEdit(QWidget *parent) : QLineEdit(parent)
{

#ifdef Q_OS_WASM
    setReadOnly(true);
    setMouseTracking(true);
#endif

}

void MyLineEdit::mouseMoveEvent(QMouseEvent *e)
{

#ifdef Q_OS_WASM
    Q_UNUSED(e);
    auto pos = mapToGlobal(QPoint(0, 0));
    ZyHtmlUtil::showTextInput(this, this->text(), pos.x(), pos.y(), this->width(), this->height());
#else
    QLineEdit::mouseMoveEvent(e);
#endif

}
