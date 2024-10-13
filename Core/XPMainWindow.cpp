#include "XPMainWindow.hpp"
#include "XPTrackBar.hpp"
#include "XPTextBar.hpp"
#include "XPSkillSelector.hpp"
#include "XPSkillIcon.hpp"

#include "resource.h"

#include <wx/wfstream.h>
#include <wx/hyperlink.h>



static inline void XPShowErrorBox(const wxString& message)
{
	wxMessageBox(message, L"Error", wxOK | wxICON_ERROR | wxCENTRE);
}

static const wxSize _default_wnd_size(1100, 640);
static const wxString _load_icon_button_tooltip(
	L"Optional\nUpload a skill icon in DDS format. Recommended resolution is 256x256 pixels.");

enum XPIDs
{
	XP_ID_SKILL_ID,
	XP_ID_SKILL_CHANGE,
	XP_ID_LOAD_ICON_BTN,
	XP_ID_CREATE_SKILL_BTN,
	XP_ID_SKILL_LIST,
	XP_ID_ABOUT_BTN
};



static void _about_xp_creator(wxCommandEvent& event)
{
	wxMessageBox(
		wxString(L"Experience Framework Skill Creator ver. ") + wxString(XPCREATOR_VERSION) + wxString(L" by Orleon"),
		L"About", wxOK | wxICON_INFORMATION | wxCENTRE);
}

void XPMainWindow::OnSkill_ID_Changed(wxKeyEvent& event)
{
	const wxChar ch = event.GetUnicodeKey();
	if (ch < 127 && (wxIsalnum(ch) || ch == '_'))
	{
		this->ReadyCheck();
		return;
	}

	wxTextCtrl* const ctrl = static_cast<wxTextCtrl*>(event.GetEventObject());
	if (ctrl->IsEmpty())
	{
		this->ReadyCheck();
		return;
	}

	wxString str = ctrl->GetValue();
	const int pos = ctrl->GetInsertionPoint() - 1;
	str.erase(pos, 1);
	ctrl->SetValue(str);
	ctrl->SetInsertionPoint(pos);
	this->ReadyCheck();
}

void XPMainWindow::OnSkillChanged(wxKeyEvent& event)
{
	this->ReadyCheck();
}

void XPMainWindow::OnLoadIcon(wxCommandEvent& event)
{
	// Clear previous icon information
	if (!bXPIsLoadIconMode)
	{
		XPIconBtn->SetLabelText(L"Select Icon");
		XPIconBtn->SetToolTip(_load_icon_button_tooltip);
		XPIconText->SetLabelText(wxEmptyString);
		XPIconText->UnsetToolTip();
		XPIconFileName.Clear();
		XPIconFilePath.Clear();
		XPIconPreview->Show(false);
		bXPIsLoadIconMode = true;
		return;
	}

	wxFileDialog openFileDialog(this,
		L"Open Icon",
		wxEmptyString, wxEmptyString,
		L"DDS files (*.dds)|*dds", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_NO_FOLLOW);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	const wxString fPath = openFileDialog.GetPath();
	const wxString fName = openFileDialog.GetFilename();
	uint32_t headerSize = 0, iconHeight = 0, iconWidth = 0;

	{
		wxFileInputStream file(fPath);
		if (!file.IsOk())
		{
			XPShowErrorBox(L"Can't open file!");
			return;
		}

		char fileCode[128]{};
		file.Read(fileCode, 128);

		headerSize = *reinterpret_cast<uint32_t*>(fileCode + 4);
		iconHeight = *reinterpret_cast<uint32_t*>(fileCode + 12);
		iconWidth = *reinterpret_cast<uint32_t*>(fileCode + 16);

		if (memcmp(fileCode, "DDS ", 4) != 0 || headerSize != 124)
		{
			XPShowErrorBox(L"Invalid DDS format!");
			return;
		}
	}

	if (CreateTempIcon(fPath, fName))
	{
		if (XPIconPreview->SetImage())
			XPIconPreview->Show(true);
	}

	XPIconFileName = fName;
	XPIconFilePath = fPath;
	XPIconText->SetLabelText(XPIconFileName + XPSprintf(L"\n(%ux%u)", iconWidth, iconHeight));
	XPIconText->SetToolTip(XPIconFilePath);
	XPIconBtn->SetLabelText(L"Unselect Icon");
	XPIconBtn->UnsetToolTip();
	bXPIsLoadIconMode = false;
}

void XPMainWindow::OnSelectSkillProp(wxCommandEvent& event)
{
	if (XPSkillList->GetSelection() == wxNOT_FOUND) return;
}

void XPMainWindow::OnChangeSkillProp(const wxString& item)
{
	this->ReadyCheck();
}

void XPMainWindow::ReadyCheck()
{
	const bool result = (
		!XPID->IsEmpty() && !XPName->IsEmpty() && !XPDesc->IsEmpty() && !XPSkillList->GetXPSelectedList()->IsEmpty());
	XPCreateBtn->Enable(result);
}

XPMainWindow* XPMainWindow::Get()
{
	static XPMainWindow* instance = new XPMainWindow();
	return instance;
}

XPMainWindow::XPMainWindow()
	: wxFrame(nullptr, wxID_ANY, L"XPCreator", wxDefaultPosition, _default_wnd_size,
		wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN),
	bXPIsLoadIconMode(true)
{
	wxIcon icon;
	icon.CreateFromHICON(LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1)));
	this->SetIcon(icon);

	this->SetMinClientSize(_default_wnd_size);
	
	XPhbox = new wxBoxSizer(wxHORIZONTAL);

	wxPanel* panelSkillInfo = new wxPanel(this, wxID_ANY);

	wxButton* aboutButton = new wxButton(panelSkillInfo, XP_ID_ABOUT_BTN,
		L"About", { _default_wnd_size.x - 110, 10 });
	aboutButton->Bind(wxEVT_BUTTON, &_about_xp_creator, XP_ID_ABOUT_BTN);

	wxHyperlinkCtrl* moddbLink = new wxHyperlinkCtrl(panelSkillInfo, wxID_ANY,
		L"ModDB", L"https://www.moddb.com/mods/stalker-anomaly/addons/experience-framework",
		{ _default_wnd_size.x - 95, 40 }, { 70, 20 }, wxHL_ALIGN_CENTRE);
	moddbLink->SetFont(moddbLink->GetFont().Scale(1.2F));

	wxHyperlinkCtrl* githubLink = new wxHyperlinkCtrl(panelSkillInfo, wxID_ANY,
		L"GitHub", L"https://github.com/",
		{ _default_wnd_size.x - 95, 65 }, { 70, 20 }, wxHL_ALIGN_CENTRE);
	githubLink->SetFont(moddbLink->GetFont());

	XPID = new XPTextBar(panelSkillInfo, XP_ID_SKILL_ID,
		L"Skill ID", wxEmptyString, { 15, 35 }, { 200, 20 });
	XPID->Bind(wxEVT_AFTER_CHAR, &XPMainWindow::OnSkill_ID_Changed, this, XP_ID_SKILL_ID);
	XPID->SetToolTip(L"Allowed characters: Aa-Zz, 0-9 or _");

	XPName = new XPTextBar(panelSkillInfo, XP_ID_SKILL_CHANGE,
		L"Skill Name", wxEmptyString, { 15, 90 }, { 200, 20 });
	XPName->Bind(wxEVT_AFTER_CHAR, &XPMainWindow::OnSkillChanged, this, XP_ID_SKILL_CHANGE);
	XPName->SetToolTip(L"In-game skill display name.");

	XPMaxLevel = new XPTrackBar(panelSkillInfo, wxID_ANY,
		L"Max. Skill Level", 1, 1, 100, {15, 140}, { 200, 20 });
	XPMaxLevel->SetToolTip(L"This setting specifies the maximum number of skill levels.");

	XPReqLevel = new XPTrackBar(panelSkillInfo, wxID_ANY,
		L"Min. Actor Level", 1, 1, 100, { 15, 190 }, { 200, 20 });
	XPReqLevel->SetToolTip(L"This setting sets the minimum required actor level to unlock the skill.");

	XPEachLevel = new XPTrackBar(panelSkillInfo, wxID_ANY,
		L"Each Actor Level", 0, 0, 50, { 15, 240 }, { 200, 20 });
	XPEachLevel->SetToolTip(L"This setting increases the required skill level as it is upgraded.");

	XPReqPrestige = new XPTrackBar(panelSkillInfo, wxID_ANY,
		L"Min. Actor Prestige", 0, 0, 10, { 15, 290 }, { 200, 20 });
	XPReqPrestige->SetToolTip(L"This setting specifies the actor's prestige level requirement to unlock the skill.");

	XPDesc = new XPTextBar(panelSkillInfo, XP_ID_SKILL_CHANGE,
		L"Skill Description", wxEmptyString, { 15, 350 }, { 300, 150 },
		wxTE_RICH | wxTE_MULTILINE | wxTE_PROCESS_TAB);
	XPDesc->Bind(wxEVT_AFTER_CHAR, &XPMainWindow::OnSkillChanged, this, XP_ID_SKILL_CHANGE);

	XPIconBtn = new wxButton(panelSkillInfo, XP_ID_LOAD_ICON_BTN,
		L"Select Icon", { 550, 20 }, { 100, 50 });
	XPIconBtn->Bind(wxEVT_BUTTON, &XPMainWindow::OnLoadIcon, this, XP_ID_LOAD_ICON_BTN);
	XPIconBtn->SetToolTip(_load_icon_button_tooltip);

	XPIconText = new wxStaticText(panelSkillInfo, wxID_ANY,
		wxEmptyString, { 500, 70 }, { 200, 40 }, wxALIGN_CENTRE_HORIZONTAL);
	XPIconText->SetFont(XPIconText->GetFont().Scale(1.2F));
	XPIconText->SetForegroundColour({ *wxBLUE });

	XPIconPreview = new XPSkillIcon(panelSkillInfo, wxID_ANY,
		{ 472, 115 }, { 256, 256 });
	XPIconPreview->Show(false);
	XPIconPreview->SetBackgroundColour({ *wxRED });

	XPSkillList = new XPSkillPropsBar(panelSkillInfo, XP_ID_SKILL_LIST,
		{ 380, 435 });

	XPSkillList->Append(L"asdasdasd");

	XPSkillList->Bind(wxEVT_CHOICE, &XPMainWindow::OnSelectSkillProp, this, XP_ID_SKILL_LIST);
	XPSkillList->SetXPAddItemCallback(&XPMainWindow::OnChangeSkillProp, this);
	XPSkillList->SetXPRemoveItemCallback(&XPMainWindow::OnChangeSkillProp, this);

	XPCreateBtn = new wxButton(panelSkillInfo, XP_ID_CREATE_SKILL_BTN,
		L"Create Skill", { 115, 520 }, { 100, 75 });
	XPCreateBtn->Enable(false);

	//wxButton* aboutButton = new wxButton(panel, XP_ID_ABOUT_BTN, "About");
	//wxButton* createSkillButton = new wxButton(panel2, XP_ID_CREATE_SKILL_BTN, "Create", {820, 23});

	XPhbox->Add(panelSkillInfo, 1, wxEXPAND);
	this->SetSizer(XPhbox);

	//this->Bind(wxEVT_BUTTON, &_about_xp_creator, XP_ID_ABOUT_BTN);

	this->Centre();
}
