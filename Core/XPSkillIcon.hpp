#pragma once

#include "XPDefs.hpp"

extern bool CreateTempIcon(const wxString& filePath, const wxString& fileName);
extern void RemoveTempIcon();


class XPSkillIcon : public wxWindow
{
public:
	XPSkillIcon(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
		: wxWindow(parent, id, pos, size), XPImage(), XPResized(), XPWidth(-1), XPHeight(-1)
	{

	}

	bool SetImage();

	void paintEvent(wxPaintEvent& evt);
	void paintNow();

	void render(wxDC& dc);
	void OnSize(wxSizeEvent& event);

	DECLARE_EVENT_TABLE()

private:
	wxImage XPImage;
	wxBitmap XPResized;
	int XPWidth, XPHeight;
};