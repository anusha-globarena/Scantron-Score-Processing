/*
 * Copyright (c) 2011 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#ifndef VERSIONLIST_H
#define VERSIONLIST_H
#include "version.h"

class VersionList {

 public:
    bool readKeysFromFile( QFile &VersionFile );
    
    inline const QHash<int, Version *> &list() const { return m_VersionList; }
    
    inline int numQuestions() const { return m_qcount; }

 private:
    QHash<int, Version *> m_VersionList;
    int m_qcount;

};
#endif
