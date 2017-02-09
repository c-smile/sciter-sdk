// dllmain.h : Declaration of module class.

class CAxSciterModule : public ATL::CAtlDllModuleT< CAxSciterModule >
{
public :
	DECLARE_LIBID(LIBID_AxSciterLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AXSCITER, "{D2F98EB5-BD24-4A83-9044-DC5330A79665}")
};

extern class CAxSciterModule _AtlModule;
