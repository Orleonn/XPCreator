#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define XP_DEFAULT_MOVESPEED_POWER 5

static int _speed_power = XP_DEFAULT_MOVESPEED_POWER;

static void _on_change_power(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_speed_power = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%d%%", _speed_power));
}

class MoveSpeedPowerCustomizeDialog final : public wxDialog
{
public:
	MoveSpeedPowerCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change speed power", wxDefaultPosition, { 200, 110 })
	{
		_speed_power = XP_DEFAULT_MOVESPEED_POWER;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", XP_DEFAULT_MOVESPEED_POWER, 1, 25, { 15, 30 }, { 130, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_power);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%d%%", _speed_power));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};



XP_DECLARE_SKILL_PROPERTY(MoveSpeedPowerProp, L"Increase movement speed");

wxString MoveSpeedPowerProp::GetDescription() const
{
	return { L"Increase movement speed at 5% by default per each skill level." };
}

wxDialog* MoveSpeedPowerProp::GetCustomizeDialog() const
{
	return new MoveSpeedPowerCustomizeDialog(XPMainWindow::Get());
}

std::wstring MoveSpeedPowerProp::GetInGameLevelDescription(int desired_level) const
{
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] increases your movement speed by %d%%", (_speed_power * desired_level));
}

std::vector<std::pair<std::wstring, std::wstring>>
MoveSpeedPowerProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring MoveSpeedPowerProp::GetUpdateFunctionName() const
{
	return { L"on_movespeed_prop_update" };
}

std::wstring MoveSpeedPowerProp::GetScriptImplementation() const
{
	const wxString id = XPMainWindow::Get()->GetSkillID();
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\MoveSpeedPower.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read MoveSpeedPower.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), id.wc_str(), id.wc_str(), (static_cast<float>(_speed_power) / 100.0F));
}
