#pragma once

#include <QtCore/qglobal.h>

#if defined(LNPLAINTEXT_LIBRARY)
  #define LNPLAINTEXT_SHARED_EXPORT Q_DECL_EXPORT
#else
  #define LNPLAINTEXT_SHARED_EXPORT Q_DECL_IMPORT
#endif

