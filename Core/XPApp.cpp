
#include "XPDefs.hpp"
#include "XPMainWindow.hpp"
#include "XPSkillIcon.hpp"



class XPApplication final : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit() override;
};

bool XPApplication::OnInit()
{
	wxInitAllImageHandlers();
	XPMainWindow* window = XPMainWindow::Get();
	window->Show(true);
	return true;
}

int XPApplication::OnExit()
{
	::RemoveTempIcon();
	return wxApp::OnExit();
}

wxIMPLEMENT_APP(XPApplication);
