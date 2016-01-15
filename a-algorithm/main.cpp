#include <QtCore>
#include "petri.h"

void loadCsv(QList<QStringList > * sequences, QString fileName){
   sequences->clear();
   QFile file(fileName);
   if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
       while (!file.atEnd()){
           QString line = file.readLine();
            sequences->append(line.trimmed().split(QChar(','), QString::SkipEmptyParts));
       }
       file.close();
   }
}

int main()
{
    QList<QStringList> sequences;
    loadCsv(&sequences, "../data/data.csv");
    Petri P(sequences);
    // Make Tl, Ti and To
    P.listTransitions();
    //Enumerate Xl places
    P.enumerateXl();
    //Reduce Xl to get Yl
    P.reduceXl();
    //List arcs to get Fl
    P.enumerateFl();

    //Print
    P.printSequences();
    P.printTransitions();
    P.printFootprints();
    P.printPlaces();
    P.printArcs();
    return 0;
}


