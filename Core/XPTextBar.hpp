#pragma once

#include "XPDefs.hpp"

class XPTextBar : public wxTextCtrl
{
public:
    XPTextBar(wxWindow* parent, wxWindowID id, const wxString& label,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator)
        : wxTextCtrl(parent, id, value, pos, size, style, validator)
    {
        const wxRect rc = wxTextCtrl::GetRect();
        m_MCaptionText = new wxStaticText(parent, wxID_ANY, label,
            { rc.x, rc.y - 25 }, { rc.width, 20 }, wxALIGN_CENTRE_HORIZONTAL);
        m_MCaptionText->SetFont(m_MCaptionText->GetFont().Scale(1.25F));
    }

    void SetToolTip(const wxString& tip)
    {
        m_MCaptionText->SetToolTip(tip);
    }

    void UnsetToolTip()
    {
        m_MCaptionText->UnsetToolTip();
    }

private:
    wxStaticText* m_MCaptionText;
};
