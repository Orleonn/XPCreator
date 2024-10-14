#pragma once

#include "XPDefs.hpp"



class XPBaseSkillProperty
{
public:
	XPBaseSkillProperty(const wchar_t* prop_name);
	virtual ~XPBaseSkillProperty() = default;

	XPBaseSkillProperty(const XPBaseSkillProperty&) = delete;
	XPBaseSkillProperty& operator=(const XPBaseSkillProperty&) = delete;

public:
	virtual wxString GetName() const = 0;
	virtual wxString GetDescription() const = 0;
	virtual wxDialog* GetCustomizeDialog() const = 0;
	virtual std::wstring GetInGameLevelDescription(int) const = 0;
	virtual std::vector<std::pair<std::wstring, std::wstring>> GetRegisterCallbacks() const = 0;
	virtual std::wstring GetUpdateFunctionName() const = 0;
	virtual std::wstring GetScriptImplementation() const = 0;
};

class XPProps final
{
public:
	static XPProps* Get();

	XPBaseSkillProperty* Find(const wxString& prop_name) const
	{
		const auto predicate = [&prop_name](XPBaseSkillProperty* const& prop) -> bool
			{
				return prop_name.compare(prop->GetName()) == 0;
			};

		const auto _End = mProps.end();
		const auto _Item = std::find_if(mProps.begin(), _End, predicate);
		return _Item != _End ? *_Item : nullptr;
	}

	template <class _Predicate>
	void ForEach(_Predicate _Pred) const
	{
		for (auto& item : mProps)
		{
			if (_Pred(item)) break;
		}
	}

private:
	friend class XPBaseSkillProperty;

	void RegisterProperty(XPBaseSkillProperty* prop, const wchar_t* prop_name);

	XPProps() = default;
	~XPProps() = default;

	XPProps(const XPProps&) = delete;
	XPProps& operator=(const XPProps&) = delete;

private:
	std::vector<XPBaseSkillProperty*> mProps;
};



#define XP_DECLARE_SKILL_PROPERTY(ClassName, PropertyName)											\
class ClassName final : public XPBaseSkillProperty													\
{																									\
public:																								\
	ClassName() : XPBaseSkillProperty(PropertyName) {}												\
	~ClassName() = default;																			\
																									\
	wxString GetName() const override { return { PropertyName }; }									\
	wxString GetDescription() const override;														\
	wxDialog* GetCustomizeDialog() const override;													\
	std::wstring GetInGameLevelDescription(int) const override;										\
	std::vector<std::pair<std::wstring, std::wstring>> GetRegisterCallbacks() const override;		\
	std::wstring GetUpdateFunctionName() const override;											\
	std::wstring GetScriptImplementation() const override;											\
};																									\
static ClassName __xp_static_property ## ClassName
