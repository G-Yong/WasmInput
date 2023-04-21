#ifndef ZYHTMLUTIL_H
#define ZYHTMLUTIL_H

#include <QObject>

class ZyHtmlUtil : public QObject
{
    Q_OBJECT
public:
    explicit ZyHtmlUtil(QObject *parent = nullptr);

    Q_INVOKABLE static int showTextInput(QObject* item, QString currentText, int x, int y, int width, int height);

    Q_INVOKABLE static int readLocalFile(quintptr ptr);

signals:


};

#endif // ZYHTMLUTIL_H
