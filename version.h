/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#ifndef VERSION_H
#define VERSION_H

#include <QStringList>
#include <QtCore>

#include "defines.h"

class Version {
 public:
    Version( qint16 versionid, const QStringList &list );
    qint16 id;
    int Key[MAX_QUESTIONS];
    float Weightage[MAX_QUESTIONS];
};

#endif
