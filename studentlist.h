/*
 * Copyright (c) 2011 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#ifndef STUDENTLIST_H
#define STUDENTLIST_H
#include <QHash>
#include <QString>
#include <QFile>

#include "student.h"

class StudentList {
 private:
    QHash<QString, Student *> m_StudentList;
 public:
    bool readResponsesFromFile( QFile &f );

    inline QHash<QString, Student *> *list() { return &m_StudentList; }
};

#endif
