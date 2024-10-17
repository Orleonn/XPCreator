#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_CARRY_WEIGHT 2

static int _carry_weight = DEFAULT_CARRY_WEIGHT;

static void _on_change_wight(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_carry_weight = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%dkg", _carry_weight));
}

class CarryWeightCustomizeDialog final : public wxDialog
{
public:
	CarryWeightCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change weight", wxDefaultPosition, { 215, 110 })
	{
		_carry_weight = DEFAULT_CARRY_WEIGHT;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_CARRY_WEIGHT, 1, 50, { 15, 30 }, { 130, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_wight);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%dkg", _carry_weight));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};



XP_DECLARE_SKILL_PROPERTY(CarryWeightProp, L"Increase carry weight");

wxString CarryWeightProp::GetDescription() const
{
	return { L"Increase max carry weight at 2kg by default per each skill level." };
}

wxDialog* CarryWeightProp::GetCustomizeDialog() const
{
	return new CarryWeightCustomizeDialog(XPMainWindow::Get());
}

std::wstring CarryWeightProp::GetInGameLevelDescription(int desired_level) const
{
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] increases your max carry weight by %dkg", (_carry_weight * desired_level));
}

std::vector<std::pair<std::wstring, std::wstring>>
CarryWeightProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring CarryWeightProp::GetUpdateFunctionName() const
{
	return { L"on_carry_weight_prop_update" };
}

std::wstring CarryWeightProp::GetScriptImplementation() const
{
	const wxString id = XPMainWindow::Get()->GetSkillID();
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\CarryWeight.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read CarryWeight.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), id.wc_str(), id.wc_str(), _carry_weight);
}
