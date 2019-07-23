#include "zeelamo.h"
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSplashScreen* splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/splash.jpg"));
    splash->show();

    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("Setting up the main window..."),
                        topRight, Qt::white);

    Zeelamo* myZl = new Zeelamo;
     if(argc > 1)
        myZl -> loadFile(argv[1]);

     splash->showMessage(QObject::tr("Loading modules..."),
     topRight, Qt::white);

    myZl -> show();
    splash -> finish(myZl);
    delete splash;

    return app.exec();
}
