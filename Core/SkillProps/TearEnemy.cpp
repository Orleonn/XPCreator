#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_TEAR_ENEMY 2

static int _tear_enemy = DEFAULT_TEAR_ENEMY;

static void _on_change_chance(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_tear_enemy = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.2f%%", (static_cast<float>(_tear_enemy) / 100.0F)));
}

class TearEnemyPropCustomizeDialog final : public wxDialog
{
public:
	TearEnemyPropCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change chance", wxDefaultPosition, { 600, 110 })
	{
		_tear_enemy = DEFAULT_TEAR_ENEMY;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_TEAR_ENEMY, 1, 500, { 15, 30 }, { 500, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_chance);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%.2f%%", (static_cast<float>(_tear_enemy) / 100.0F)));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};


XP_DECLARE_SKILL_PROPERTY(TearEnemyProp, L"Chance to tear enemy");

wxString TearEnemyProp::GetDescription() const
{
	return { L"Chance to tear enemy at 0.02% by default per each skill level." };
}

wxDialog* TearEnemyProp::GetCustomizeDialog() const
{
	return new TearEnemyPropCustomizeDialog(XPMainWindow::Get());
}

std::wstring TearEnemyProp::GetInGameLevelDescription(int desired_level) const
{
	const float val = (static_cast<float>(_tear_enemy) / 100.0F * desired_level);
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] chance to tear enemy %.2f%%",
		(val > 100.0F ? 100.0F : val));
}

std::vector<std::pair<std::wstring, std::wstring>>
TearEnemyProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring TearEnemyProp::GetUpdateFunctionName() const
{
	return { L"on_tear_enemy_prop_update" };
}

std::wstring TearEnemyProp::GetScriptImplementation() const
{
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\TearEnemy.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read TearEnemy.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(),
		XPMainWindow::Get()->GetSkillID().wc_str(), static_cast<float>(_tear_enemy) / 10000.0F);
}
