#pragma once

#include "XPDefs.hpp"

class XPTextBar;
class XPTrackBar;
class XPSkillIcon;
class XPSkillPropsBar;

class XPMainWindow final : public wxFrame
{
public:
    static XPMainWindow* Get();

private:
    XPMainWindow();

    void OnSkill_ID_Changed(wxKeyEvent& event);
    void OnSkillChanged(wxKeyEvent& event);
    void OnLoadIcon(wxCommandEvent& event);
    void OnSelectSkillProp(wxCommandEvent& event);
    void OnChangeSkillProp(const wxString& item);

    void ReadyCheck();

private:
    XPTextBar* XPID, *XPName, *XPDesc;
    XPTrackBar* XPMaxLevel, *XPReqLevel, *XPEachLevel, *XPReqPrestige;
    wxStaticText* XPIconText;
    XPSkillIcon* XPIconPreview;
    wxBoxSizer* XPhbox;
    wxString XPIconFileName, XPIconFilePath;
    wxButton* XPIconBtn, *XPCreateBtn;
    bool bXPIsLoadIconMode;
    XPSkillPropsBar* XPSkillList;
};
