#pragma once

#include "XPDefs.hpp"



#pragma warning(push)
#pragma warning(disable : 26495)

class XPBaseSkillProperty;

struct XPSkillInfo
{
	std::wstring create_path, icon_path, id, name, descr;
	int max_level, req_level, each_level, req_prestige;
	std::vector<XPBaseSkillProperty*> props;
};

extern bool XPCreate(const XPSkillInfo& info);

#pragma warning(pop)
