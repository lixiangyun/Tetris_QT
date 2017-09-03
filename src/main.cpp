#include <QtGui/QApplication>
#include "tetris.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    Tetris w;
    w.show();
    return a.exec();
}
