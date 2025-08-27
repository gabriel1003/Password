#include <QCoreApplication>
#include <presentation/presentation.h>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    Presentation show;
    show.run();
    return 0;
}