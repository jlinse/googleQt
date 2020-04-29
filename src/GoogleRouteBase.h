#pragma once
#include "GoogleExportMacro.h"
namespace googleQt{

    class Endpoint;

    class GOOGLEQT_DLLSPEC GoogleRouteBase
    {        
    public:
        GoogleRouteBase(Endpoint* ep):m_end_point(ep){}

    protected:
        Endpoint* m_end_point;
    };

}
