﻿#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#include <QWidget>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "ZyHtmlUtil.h"
#include "mylineedit.h"

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QTGLESSTREAM_DISPLAY")) {
        qputenv("QT_QPA_EGLFS_PHYSICAL_WIDTH", QByteArray("213"));
        qputenv("QT_QPA_EGLFS_PHYSICAL_HEIGHT", QByteArray("120"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    }

    qmlRegisterType<ZyHtmlUtil>("ZyCppItems", 1, 0, "ZyHtmlUtil");

    QApplication app(argc, argv);

    // 设置全局字体，用于解决Qt for WebAssembly中文显示异常
    int fontId = QFontDatabase::addApplicationFont(":/font/Alibaba-PuHuiTi-Light.otf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    qDebug() << "fontfamilies:" << fontFamilies;
    if (fontFamilies.size() > 0)
    {
        QFont font;
        font.setFamily(fontFamilies[0]);//设置全局字体
        app.setFont(font);
    }


//#define QML // 注释此处来切换
#ifdef QML
    // 运行qml

    // 注册
    qmlRegisterSingletonType<ZyHtmlUtil>("ZyCppItems", 1, 0, "ZyHtmlUtil",[](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        ZyHtmlUtil *util = new ZyHtmlUtil();
        return util;
    });

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
#else
    // 运行widget
    QWidget widget;
    widget.resize(640, 480);

    for(int i = 0; i < 10; i++)
    {
        MyLineEdit *mLineEdit = new MyLineEdit(&widget);
        mLineEdit->move(10, 10 + (mLineEdit->height() + 10) * i);
    }

    widget.show();
#endif

    QNetworkAccessManager netMan;
    // 测试跨域请求
    {
        QNetworkRequest req;
        req.setAttribute(QNetworkRequest::CookieLoadControlAttribute, true);
        req.setAttribute(QNetworkRequest::CookieSaveControlAttribute, true);

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        // Qt6.5可以使用这个解决cookie问题
        req.setAttribute(QNetworkRequest::UseCredentialsAttribute, true);
#endif


//        // 启用withCredentials选项,解决跨域cookie的问题，好像不行
//        req.setRawHeader("Access-Control-Allow-Credentials", "true");

        // 要解决跨域的问题，最好还是服务器端添加这个：
//        Access-Control-Allow-Origin: *
//        Access-Control-Allow-Credentials: true

        QString urlStr;
//        urlStr = "https://api.github.com/users/github";  // 其响应中包含Access-Control-Allow-Origin: *标头
        urlStr = "http://www.jsontest.com";  // 其响应中包含Access-Control-Allow-Origin: *标头
//        urlStr = "http://www.baidu.com"; // 无法跨域

        req.setUrl(QUrl(urlStr));

        auto reply = netMan.get(req);
        QObject::connect(reply, &QNetworkReply::finished, [=](){
//            qDebug() << "network reply:" << reply->readAll();
            qDebug() << "header count:" << reply->rawHeaderList().length() << reply->rawHeaderList();
            qDebug() << "cookie header:" << reply->header(QNetworkRequest::CookieHeader);
            qDebug() << "set cookie header:" << reply->header(QNetworkRequest::SetCookieHeader);
            qDebug() << reply->rawHeaderPairs();
            reply->deleteLater();
        });
    }


    return app.exec();
}
