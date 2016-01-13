#ifndef PETRI_H
#define PETRI_H

#include <QtCore>

using namespace std;

enum Relation{
    DIFFER,
    CAUSE,
    CAUSED,
    PARALLEL
};

class Petri
{
private:
    QList<QStringList> _sequences;
    QSet<QString> _Tl;
    QSet<QString> _Ti;
    QSet<QString> _To;
    QMap<QString, QVector<QSet<QString> > > _Yl;
    QList<QVector<QString> > _Fl;

    QMap<QString, QMap<QString, Relation> > _footprints;
    QList<QVector<QSet<QString> > > _Xl;

    void initFootprints();
    void calculateFootprints();

    bool areConnected(QString first, QString second);
    QVector<QSet<QString> > makeSetOfDiff(QString key, QSet<QString> set);
    bool isInclude(QSet<QString> first, QSet<QString> second);

public:
    Petri(QList<QStringList> sequences);

    void listTransitions();
    void enumerateXl();
    void reduceXl();
    void enumerateFl();

    QSet<QString> getTransitions();
    QMap<QString, QVector<QSet<QString> > > getPlaces();
    QList<QVector<QString> > getArcs();

    void printSequences();
    void printTransitions();
    void printStarters();
    void printEnders();
    void printFootprints();
    void printPlaces();
    void printArcs();
};

#endif // PETRI_H
