//
// Created by linqi on 16-5-31.
//

#ifndef WDR_WDRPROCESS_H
#define WDR_WDRPROCESS_H


#include "include/global.h"
#include "include/dataType.h"
#include "include/regConfig.h"
#include "include/fileIO.h"
#include "include/fileParser.h"
#include "include/gauseFilter.h"
#include "log.h"
#include "include/toneMap.h"
#include "include/nonlinearCurve.h"

INT32 clearGlobalConfig();

INT32 frameProc();

INT32 wdrProcess();

INT32 simpleProcess();

#endif //WDR_WDRPROCESS_H
