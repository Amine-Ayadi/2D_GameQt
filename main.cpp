#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <cstdlib>
#include <QThread>

void terminateHandler() {
    std::cerr << "terminate called" << std::endl;
    // Additional debugging or logging can be added here
    std::abort();  // Terminate the program
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::set_terminate(terminateHandler);

    QThread::currentThread()->setObjectName("Main Thread");
    MainWindow w;
    w.show();
    return a.exec();
}
