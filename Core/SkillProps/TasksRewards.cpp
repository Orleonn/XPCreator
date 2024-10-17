#include "XPPropsContent.hpp"
#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"



#define DEFAULT_TASKS_REWARDS 2

static int _tasks_rewards = DEFAULT_TASKS_REWARDS;

static void _on_change_reward(wxCommandEvent& event)
{
	XPTrackBar* const ctrl = static_cast<XPTrackBar*>(event.GetEventObject());
	_tasks_rewards = ctrl->GetValue();
	ctrl->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%d%%", _tasks_rewards));
}

class TaskRewardsCustomizeDialog final : public wxDialog
{
public:
	TaskRewardsCustomizeDialog(wxWindow* parent)
		: wxDialog(parent, wxID_ANY, L"Change money rewards", wxDefaultPosition, { 200, 110 })
	{
		_tasks_rewards = DEFAULT_TASKS_REWARDS;
		XPTrackBar* trackBar = new XPTrackBar(this, wxID_ANY,
			L"Value per skill level", DEFAULT_TASKS_REWARDS, 1, 25, { 15, 30 }, { 130, 30 });
		trackBar->Bind(wxEVT_SLIDER, &_on_change_reward);
		trackBar->GetXPSliderCounter()->SetLabelText(XPSprintf(L"%d%%", _tasks_rewards));

		const wxPoint parentPos = parent->GetPosition();
		const wxSize parentSize = parent->GetSize();
		const wxSize mySize = wxDialog::GetSize();
		wxDialog::SetPosition({ parentPos.x + parentSize.x / 2 - mySize.x / 2, parentPos.y + parentSize.y / 2 - mySize.y / 2 });
	}
};


XP_DECLARE_SKILL_PROPERTY(TaskRewardsProp, L"Increase tasks money rewards");

wxString TaskRewardsProp::GetDescription() const
{
	return { L"Increase tasks money rewards at 2% by default per each skill level." };
}

wxDialog* TaskRewardsProp::GetCustomizeDialog() const
{
	return new TaskRewardsCustomizeDialog(XPMainWindow::Get());
}

std::wstring TaskRewardsProp::GetInGameLevelDescription(int desired_level) const
{
	return XPSprintf(L"%%c[d_blue] • %%c[d_cyan] increase money rewards for tasks by %d%%", (_tasks_rewards * desired_level));
}

std::vector<std::pair<std::wstring, std::wstring>>
TaskRewardsProp::GetRegisterCallbacks() const
{
	return {};
}

std::wstring TaskRewardsProp::GetUpdateFunctionName() const
{
	return { L"on_tasks_rewards_prop_update" };
}

std::wstring TaskRewardsProp::GetScriptImplementation() const
{
	const std::wstring content = XPReadFromFile(XPGetPropsDir() + L"\\TasksRewards.txt");
	if (content.empty())
	{
		XPShowErrorBox(L"Can't read TasksRewards.txt!");
		std::exit(1);
	}
	return XPSprintf(content.c_str(), (static_cast<float>(_tasks_rewards) / 100.0F));
}
