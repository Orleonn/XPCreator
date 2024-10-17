#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_STAMINA_REGEN 1

static int _stamina_regen = DEFAULT_STAMINA_REGEN;

static void _on_change_power(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_stamina_regen = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.2f%%", (static_cast<float>(_stamina_regen) / 100.0F)));
}

class StaminaRegenPropCustomizeDialog final : public wxDialog
{
public:
	StaminaRegenPropCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change regeneration power", wxDefaultPosition, { 600, 110 })
	{
		_stamina_regen = DEFAULT_STAMINA_REGEN;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_STAMINA_REGEN, 1, 500, { 15, 30 }, { 500, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_power);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.2f%%", (static_cast<float>(_stamina_regen) / 100.0F)));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};


XP_DECLARE_SKILL_PROPERTY(StaminaRegenProp, L"Additional stamina regeneration");

wxString StaminaRegenProp::GetDescription() const
{
	return { L"Increase stamina regeneration at 0.01%/sec by default per each skill level" };
}

wxDialog* StaminaRegenProp::GetCustomizeDialog() const
{
	return new StaminaRegenPropCustomizeDialog(XPMainWindow::Get());
}

std::wstring StaminaRegenProp::GetInGameLevelDescription(int desired_level) const
{
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] increases your stamina regeneration by %.2f%%/sec",
		(static_cast<float>(_stamina_regen) / 100.0F * static_cast<float>(desired_level)));
}

std::vector<std::pair<std::wstring, std::wstring>>
StaminaRegenProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring StaminaRegenProp::GetUpdateFunctionName() const
{
	return { L"on_stamina_regen_prop_update" };
}

std::wstring StaminaRegenProp::GetScriptImplementation() const
{
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\StaminaRegen.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read StaminaRegen.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), static_cast<float>(_stamina_regen) / 10000.0F);
}
