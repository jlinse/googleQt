#pragma once

#if defined GOOGLEQT_DLL
 #define GOOGLEQT_DLLSPEC Q_DECL_EXPORT
#else
 #define GOOGLEQT_DLLSPEC Q_DECL_IMPORT
#endif
