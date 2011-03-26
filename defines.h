/*
 * Copyright (c) 2010 Akarsh Simha <akarshsimha@gmail.com>
 * License: GPL v2.05
 */

#ifndef DEFINES_H
#define DEFINES_H

#ifndef MAX_QUESTIONS
#define MAX_QUESTIONS 30
#endif

#ifndef MAX_RESPONSES
#define MAX_RESPONSES 10
#endif

#if (MAX_RESPONSES > 15)
#error "MAX_RESPONSES larger than 15. Not fair. Increase the size of the response-binning integer if required."
#endif

#endif

