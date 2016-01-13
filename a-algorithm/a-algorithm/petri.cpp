#include "petri.h"
#include <iostream>

Petri::Petri(QList<QStringList> sequences)
{
    _sequences = sequences;
}

void Petri::listTransitions(){
    _Tl.clear();
    _Ti.clear();
    _To.clear();
    for(QList<QStringList>::iterator it = _sequences.begin(); it != _sequences.end(); it++){
        for(QStringList::iterator sit = it->begin(); sit != it->end(); sit++){
            _Tl.insert(*sit);
        }
        _Ti.insert(it->first());
        _To.insert(it->last());
    }
}

/**
 * @brief Petri::init
 * Init the footprints matrix
 */
void Petri::initFootprints(){
    _footprints.clear();
    QMap<QString, Relation> tmp;
    QList<QString> list = _Tl.toList();
    for(int i = 0; i < list.size(); i++)
    {
        tmp.clear();
        for(int j = 0; j < list.size(); j++){
            tmp.insert(list[j], DIFFER);
        }
        _footprints.insert(list[i], tmp);
    }
}

void Petri::calculateFootprints(){
    initFootprints();
    //calcultate
    for(QList<QStringList>::iterator it = _sequences.begin(); it != _sequences.end(); it++){
        for(int i = 1; i < it->size(); i++){
            switch(_footprints[it->at(i-1)][it->at(i)]){
            case DIFFER:
                _footprints[it->at(i-1)][it->at(i)] = CAUSE;
                _footprints[it->at(i)][it->at(i-1)] = CAUSED;
                break;
             case CAUSED:
                _footprints[it->at(i-1)][it->at(i)] = PARALLEL;
                _footprints[it->at(i)][it->at(i-1)] = PARALLEL;
                break;
            default:
                break;
            }
        }
    }
}

bool Petri::areConnected(QString first, QString second){
    if(_footprints[first][second]!=DIFFER) return true;
    return false;
}

QVector<QSet<QString> > Petri::makeSetOfDiff(QString key, QSet<QString> set){
    QVector<QSet<QString> > r;
    if(set.empty()) return r;
    QSet<QString> copy;
    for(QSet<QString>::iterator it = set.begin(); it != set.end(); it++){
        for(QSet<QString>::iterator sit = it + 1; sit != set.end(); sit++){
            if(areConnected(*it, *sit)){
                copy = set;
                copy.remove(*it);
                r = makeSetOfDiff(key, copy);
                copy.insert(*it);
                copy.remove(*sit);
                r += makeSetOfDiff(key, copy);
                return r;
            }
        }
    }
    r.append(set);
    return r;
}

void Petri::enumerateXl(){
    if(_footprints.empty()) calculateFootprints();
    QSet<QString> inputs, outputs, keySet;
    QVector<QSet<QString> > r, tmp(2);
    for(QMap<QString, QMap<QString, Relation> >::iterator it = _footprints.begin(); it != _footprints.end(); it++){
        for(QMap<QString, Relation>::iterator itt = it.value().begin(); itt != it.value().end(); itt++){
            if(itt.value() == CAUSE) outputs.insert(itt.key());
            if(itt.value() == CAUSED) inputs.insert(itt.key());
        }
        keySet.clear();
        keySet.insert(it.key());
        for(int i = 0; i < 2; i++){
            r = makeSetOfDiff(it.key(), i%2==0?outputs:inputs);
            for(QVector<QSet<QString> >::iterator itt = r.begin(); itt != r.end(); itt++){
                tmp[i%2] = keySet;
                tmp[(i+1)%2] = *itt;
                _Xl.append(tmp);
            }
        }
        outputs.clear();
        inputs.clear();
    }
}

bool Petri::isInclude(QSet<QString> first, QSet<QString> second){
    //first into second ?
    if(first.size() == 0 && second.size() == 0) return true;
    for(QSet<QString>::iterator it = first.begin(); it != first.end(); it++){
        if(!second.contains(*it)){
            return false;
        }
    }
    return true;
}

void Petri::reduceXl(){
    QSet<int> toRemove;
    for(int i = 0; i < _Xl.size()-1; i++){
        for(int j = i+1; j < _Xl.size(); j++){
            if(isInclude(_Xl.at(i).first(), _Xl.at(j).first()) && isInclude(_Xl.at(i).back(), _Xl.at(j).back())) toRemove.insert(i);
            else if(isInclude(_Xl.at(j).first(), _Xl.at(i).first()) && isInclude(_Xl.at(j).back(), _Xl.at(i).back())) toRemove.insert(j);
        }
    }
    for(int i = 0; i < _Xl.size(); i++){
        if(toRemove.contains(i)) continue;
        _Yl.insert("P" + QString::number(_Yl.size() + 1), _Xl.at(i));
    }
}

void Petri::enumerateFl(){
    QVector<QString> tmp(2);
    _Fl.clear();
    for(QSet<QString>::iterator it = _Ti.begin(); it != _Ti.end(); it++){
        tmp[0] = "P0";
        tmp[1] = *it;
        _Fl.append(tmp);
    }
    for(QSet<QString>::iterator it = _Tl.begin(); it != _Tl.end(); it++){
        for(QMap<QString, QVector<QSet<QString> > >::iterator itt = _Yl.begin(); itt != _Yl.end(); itt++){
            if(itt.value().first().contains(*it)){ //add arc transition to Place
                tmp[0] = *it;
                tmp[1] = itt.key();
                _Fl.append(tmp);
            }else if(itt.value().last().contains(*it)){ //add arc Place to transition
                tmp[0] = itt.key();
                tmp[1] = *it;
                _Fl.append(tmp);
            }
        }
    }
    for(QSet<QString>::iterator it = _To.begin(); it != _To.end(); it++){
        tmp[0] = *it;
        tmp[1] = "P" + QString::number(_Yl.size() + 1);
        _Fl.append(tmp);
    }
}

QSet<QString> Petri::getTransitions(){
    return _Tl;
}

QMap<QString, QVector<QSet<QString> > > Petri::getPlaces(){
    return _Yl;
}

QList<QVector<QString> > Petri::getArcs(){
    return _Fl;
}

void Petri::printSequences(){
    cout << "List of sequences : " << endl;
    for(QList<QStringList>::iterator it = _sequences.begin(); it!=_sequences.end(); it++){
        for(QStringList::iterator sit = it->begin(); sit != it->end(); sit++){
            cout << sit->toStdString() << " | ";
        }
        cout << endl;
    }
    cout << endl;
}

void Petri::printTransitions(){
    cout << endl << "List of activities (transitions) : " << endl;
    for(QSet<QString>::iterator it = _Tl.begin(); it != _Tl.end(); it++){
        cout << it->toStdString() << ", ";
    }
    cout << endl << endl;
}

void Petri::printStarters(){
    cout << endl << "List of starters : " << endl;
    for(QSet<QString>::iterator it = _Ti.begin(); it != _Ti.end(); it++){
        cout << it->toStdString() << ", ";
    }
    cout << endl << endl;
}

void Petri::printEnders(){
    cout << endl << "List of enders : " << endl;
    for(QSet<QString>::iterator it = _To.begin(); it != _To.end(); it++){
        cout << it->toStdString() << ", ";
    }
    cout << endl;
}

void Petri::printFootprints(){
    cout << endl << "--|  ";
    for(QMap<QString, QMap<QString, Relation> >::iterator it = _footprints.begin(); it != _footprints.end(); it++){
        cout << " " << it.key().toStdString() << " | ";
    }
    cout << endl;
    for(QMap<QString, QMap<QString, Relation> >::iterator it = _footprints.begin(); it != _footprints.end(); it++){
       for(QMap<QString, Relation>::iterator itt = it.value().begin(); itt != it.value().end(); itt++){
           switch(itt.value()){
            case DIFFER:
               cout << "  # |";
                break;
           case PARALLEL:
               cout << " // |";
                break;
           case CAUSE:
               cout << " -> |";
                break;
           case CAUSED:
               cout << " <- |";
                break;
           default:
               break;
           }
       }
       cout << endl;
    }
    cout << endl << endl;
}

void Petri::printPlaces(){
    cout << ends << "List of places : " << endl;
    for(QMap<QString, QVector<QSet<QString> > >::iterator it = _Yl.begin(); it != _Yl.end(); it++){
        cout << it.key().toStdString() << " : ";
        for(QVector<QSet<QString> >::iterator itt = it.value().begin(); itt != it.value().end(); itt++){
            cout << "{ ";
            for(QSet<QString>::iterator ittt = itt->begin(); ittt != itt->end(); ittt++){
                cout << ittt->toStdString() << " ";
            }
            cout << "}";
        }
        cout << endl;
    }
}

void Petri::printArcs(){
    cout << endl << "List of arcs :" << endl;
    for(QList<QVector<QString> >::iterator it = _Fl.begin(); it != _Fl.end(); it++){
        cout << it->first().toStdString() << " -> " << it->last().toStdString() << endl;
    }
}
