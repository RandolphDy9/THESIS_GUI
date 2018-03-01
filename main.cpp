#include "mainwindow.h"
#include <QApplication>

void addValues(int id, QString rn, QString ts, QString dt);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showFullScreen();
//    w.show();

//    qDebug() << "Start";

//    QSqlDatabase db;
//    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("C:/Users/Randolph D/thesis1/db/thesis1_db.sqlite");

//    if (!db.open()) {
//        qDebug() << "Problem opening database!";
//    }

//        QString query = "CREATE TABLE traffic ("
//                    "ID integer,"
//                    "road_name VARCHAR(50),"
//                    "traffic_status VARCHAR(30),"
//                    "date_time DATETIME);";
//        QSqlQuery qry;

//    if (!qry.exec(query)) {
//        qDebug() << "Error creating table!";
//    }

//    addValues(1, "Maguikay", "Moderate", "24-12-2017");
//    addValues(2, "Mabolo", "Low", "25-12-2017");

//    qDebug() << "Finish";

    return a.exec();
}


