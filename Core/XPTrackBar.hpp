#pragma once

#include "XPDefs.hpp"

class XPTrackBar : public wxSlider
{
public:
    XPTrackBar(wxWindow* parent,
        wxWindowID id,
        const wxString& label,
        int value,
        int minValue,
        int maxValue,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxSL_HORIZONTAL)
        : wxSlider(parent, id, value, minValue, maxValue, pos, size, style)
    {
        const wxRect rc = wxSlider::GetRect();
        m_MSliderCounter = new wxStaticText(parent, wxID_ANY, std::to_wstring(value),
            { rc.x + rc.width + 5, rc.y}, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
        m_MSliderCounter->SetFont(m_MSliderCounter->GetFont().Scale(1.2F));

        m_MSliderName = new wxStaticText(parent, wxID_ANY, label,
            { rc.x, rc.y - 20 }, { rc.width, 20 }, wxALIGN_CENTRE_HORIZONTAL);
        m_MSliderName->SetFont(m_MSliderName->GetFont().Scale(1.3F));
    }

    virtual ~XPTrackBar()
    {
        m_MSliderCounter->Destroy();
        m_MSliderName->Destroy();
    }

    void SetToolTip(const wxString& tip)
    {
        m_MSliderName->SetToolTip(tip);
    }

    void UnsetToolTip()
    {
        m_MSliderName->UnsetToolTip();
    }

    virtual bool ProcessEvent(wxEvent& event) override
    {
        if (event.GetId() == this->GetId() && event.GetEventType() == wxEVT_SLIDER)
        {
            this->OnSliderChanged(static_cast<wxCommandEvent&>(event));
        }
        return wxSlider::ProcessEvent(event);
    }

    wxStaticText* GetXPSliderCounter() const { return m_MSliderCounter; }

private:
    wxStaticText* m_MSliderCounter, *m_MSliderName;

    void OnSliderChanged(wxCommandEvent& event)
    {
        m_MSliderCounter->SetLabelText(std::to_wstring(event.GetInt()));
    }
};
