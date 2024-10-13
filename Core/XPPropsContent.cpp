#include "XPPropsContent.hpp"



XPProps* XPProps::Get()
{
	static XPProps instance;
	return &instance;
}

void XPProps::RegisterProperty(XPBaseSkillProperty* prop, const wchar_t* prop_name)
{
#ifdef _DEBUG
	if (this->Find(prop_name) != nullptr)
	{
		XPShowErrorBox(XPSprintf(L"\"%s\" property already exists!", prop_name));
		std::exit(1);
	}
#endif // _DEBUG

	mProps.push_back(prop);
}

XPBaseSkillProperty::XPBaseSkillProperty(const wchar_t* prop_name)
{
	XPProps::Get()->RegisterProperty(this, prop_name);
}
