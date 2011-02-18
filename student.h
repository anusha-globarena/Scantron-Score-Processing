/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#ifndef STUDENT_H
#define STUDENT_H

#ifndef MAX_QUESTIONS
#define MAX_QUESTIONS 30
#endif

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

class Version {
 public:
    Version( qint16 versionid, const QStringList &list );
    qint16 id;
    int Key[MAX_QUESTIONS];
    float Weightage[MAX_QUESTIONS];
};
#endif
