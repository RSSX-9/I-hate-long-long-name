#include <QApplication>
#include <QIcon>

#include "ui/mainwindow.h"
#include "ui/uistyle.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    UiStyle::applyApplicationStyle(app);
    app.setWindowIcon(QIcon(QStringLiteral(":/ui/app_icon.svg")));

    MainWindow window;
    window.resize(1220, 760);
    window.show();
    return app.exec();
}
