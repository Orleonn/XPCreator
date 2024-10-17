#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_REDUCE_TRADE_COST 2

static int _trade_cost = DEFAULT_REDUCE_TRADE_COST;

static void _on_change_cost(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_trade_cost = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.1f%%", (static_cast<float>(_trade_cost) / 10.0F)));
}

class TradeCostPropCustomizeDialog final : public wxDialog
{
public:
	TradeCostPropCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change trade cost", wxDefaultPosition, { 600, 110 })
	{
		_trade_cost = DEFAULT_REDUCE_TRADE_COST;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_REDUCE_TRADE_COST, 1, 100, { 15, 30 }, { 500, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_cost);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.1f%%", (static_cast<float>(_trade_cost) / 10.0F)));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};



XP_DECLARE_SKILL_PROPERTY(TradeCostProp, L"Reduce trade cost");

wxString TradeCostProp::GetDescription() const
{
	return { L"Reduce trade cost at 0.2% by default per each skill level" };
}

wxDialog* TradeCostProp::GetCustomizeDialog() const
{
	return new TradeCostPropCustomizeDialog(XPMainWindow::Get());
}

std::wstring TradeCostProp::GetInGameLevelDescription(int desired_level) const
{
	const float val = (static_cast<float>(_trade_cost) / 10.0F * static_cast<float>(desired_level));
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] prices at traders reduced by %.1f%%",
		(val > 100.0F ? 100.0F : val));
}

std::vector<std::pair<std::wstring, std::wstring>>
TradeCostProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring TradeCostProp::GetUpdateFunctionName() const
{
	return { L"on_trade_cost_prop_update" };
}

std::wstring TradeCostProp::GetScriptImplementation() const
{
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\TradeCost.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read TradeCost.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), static_cast<float>(_trade_cost) / 1000.0F);
}
