#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);

    Q_INVOKABLE int showTextInput(QObject* item, QString currentText, int x, int y, int width, int height);

    int setInputText(QString text);

signals:

private:
    QObject *mTextInput;

};

#endif // MANAGER_H
