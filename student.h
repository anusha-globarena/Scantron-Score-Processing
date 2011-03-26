/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#ifndef STUDENT_H
#define STUDENT_H

#include "defines.h"

#include<QString>
#include<QStringList>

class Student {
 public:
    QString name;
    QString EID;
    qint16 version;
    qint16 responses[MAX_QUESTIONS];
    float score;

    Student( const QString &_name, const QString &_EID, qint16 _version );
    bool insertResponse( int question, QString response );
};

#endif
