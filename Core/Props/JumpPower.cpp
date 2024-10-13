#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"

#define DEFAULT_JUMP_POWER 5

static int _jump_power = DEFAULT_JUMP_POWER;

static void _on_change_power(wxCommandEvent& event)
{
	_jump_power = static_cast<XPTrackBar*>(event.GetEventObject())->GetValue();
}

class JumpPowerCustomizeDialog final : public wxDialog
{
public:
	JumpPowerCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change jump power", wxDefaultPosition, { 170, 110 })
	{
		_jump_power = DEFAULT_JUMP_POWER;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value (%)", DEFAULT_JUMP_POWER, 1, 25, { 15, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_power);

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

wxString JumpPowerProp::GetGameImplementation() const
{
	return {};
}

std::vector<wxString> JumpPowerProp::GetInGameLevelDescriptions(int total_levels) const
{
	std::vector<wxString> vec;
	vec.reserve(total_levels);
	for (int i = 0; i < total_levels; ++i)
	{
		//
	}
	return vec;
}
