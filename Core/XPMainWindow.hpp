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

    XPMainWindow(const XPMainWindow&) = delete;
    XPMainWindow& operator=(const XPMainWindow&) = delete;

    void OnSkill_ID_Changed(wxKeyEvent& event);
    void OnSkillChanged(wxKeyEvent& event);
    void OnLoadIcon(wxCommandEvent& event);
    void OnSelectSkillProp(wxCommandEvent& event);
    void OnAddSkillProp(const wxString& item);
    void OnRemoveSkillProp(const wxString& item);
    void OnCreateSkill(wxCommandEvent& event);

    void ReadyCheck();

private:
    XPTextBar* XPID, *XPName, *XPDesc;
    XPTrackBar* XPMaxLevel, *XPReqLevel, *XPEachLevel, *XPReqPrestige;
    wxStaticText* XPIconText;
    XPSkillIcon* XPIconPreview;
    wxString XPIconFilePath;
    wxButton* XPIconBtn, *XPCreateBtn;
    XPSkillPropsBar* XPSkillList;
};
