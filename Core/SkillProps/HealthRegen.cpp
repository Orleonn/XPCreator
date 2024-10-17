#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_HEALTH_REGEN 1

static int _health_regen = DEFAULT_HEALTH_REGEN;

static void _on_change_power(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_health_regen = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.2f%%", (static_cast<float>(_health_regen) / 100.0F)));
}

class HealthRegenPropCustomizeDialog final : public wxDialog
{
public:
	HealthRegenPropCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change regeneration power", wxDefaultPosition, { 600, 110 })
	{
		_health_regen = DEFAULT_HEALTH_REGEN;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_HEALTH_REGEN, 1, 500, { 15, 30 }, { 500, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_power);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.2f%%", (static_cast<float>(_health_regen) / 100.0F)));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};


XP_DECLARE_SKILL_PROPERTY(HealthRegenProp, L"Additional health regeneration");

wxString HealthRegenProp::GetDescription() const
{
	return { L"Increase health regeneration at 0.01%/sec by default per each skill level" };
}

wxDialog* HealthRegenProp::GetCustomizeDialog() const
{
	return new HealthRegenPropCustomizeDialog(XPMainWindow::Get());
}

std::wstring HealthRegenProp::GetInGameLevelDescription(int desired_level) const
{
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] increases your health regeneration by %.2f%%/sec",
		(static_cast<float>(_health_regen) / 100.0F * desired_level));
}

std::vector<std::pair<std::wstring, std::wstring>>
HealthRegenProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring HealthRegenProp::GetUpdateFunctionName() const
{
	return { L"on_health_regen_prop_update" };
}

std::wstring HealthRegenProp::GetScriptImplementation() const
{
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\HealthRegen.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read HealthRegen.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), static_cast<float>(_health_regen) / 10000.0F);
}
