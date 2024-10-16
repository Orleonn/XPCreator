#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_JUMP_POWER 5

static int _jump_power = DEFAULT_JUMP_POWER;

static void _on_change_power(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_jump_power = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%d%%", _jump_power));
}

class JumpPowerCustomizeDialog final : public wxDialog
{
public:
	JumpPowerCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change jump power", wxDefaultPosition, { 200, 110 })
	{
		_jump_power = DEFAULT_JUMP_POWER;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_JUMP_POWER, 1, 25, { 15, 30 }, { 130, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_power);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%d%%", _jump_power));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};



XP_DECLARE_SKILL_PROPERTY(JumpPowerProp, L"Increase jumping power");

wxString JumpPowerProp::GetDescription() const
{
	return { L"Increase jumping power at 5% by default per each skill level." };
}

wxDialog* JumpPowerProp::GetCustomizeDialog() const
{
	return new JumpPowerCustomizeDialog(XPMainWindow::Get());
}

std::wstring JumpPowerProp::GetInGameLevelDescription(int desired_level) const
{
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] increases your jumping power by %d%%", (_jump_power * desired_level));
}

std::vector<std::pair<std::wstring, std::wstring>>
JumpPowerProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring JumpPowerProp::GetUpdateFunctionName() const
{
	return { L"on_jump_power_prop_update" };
}

std::wstring JumpPowerProp::GetScriptImplementation() const
{
	const wxString id = XPMainWindow::Get()->GetSkillID();
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\JumpPower.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read JumpPower.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), id.wc_str(), id.wc_str(), (static_cast<float>(_jump_power) / 100.0F));
}
