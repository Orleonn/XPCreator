#include "XPSkillIcon.hpp"

#include <filesystem>

static const std::wstring _png_temp_path(XPGetBinaryDir() + std::wstring(L"\\tmp_icon.png"));

bool CreateTempIcon(const wxString& filePath, const wxString& fileName)
{
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);

	const wxString currPath(XPGetBinaryDir());
	
	wxString cmdLine = XPSprintf(L"texconv.exe \"%s\" -m 1 -ft png -f R8G8B8A8_UNORM -o %s", filePath.wc_str(), currPath.wx_str());

	if (!CreateProcessW(nullptr, cmdLine.wchar_str(),
		nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, currPath, &si, &pi))
	{
		return false;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	std::wstring tmpFile((currPath + L"\\" + fileName).c_str().AsWChar());
	tmpFile[tmpFile.size() - 3] = L'p';
	tmpFile[tmpFile.size() - 2] = L'n';
	tmpFile[tmpFile.size() - 1] = L'g';

	if (!std::filesystem::exists(tmpFile))
		return false;

	std::filesystem::rename(tmpFile, _png_temp_path);
	return true;
}

void RemoveTempIcon()
{
	if (std::filesystem::exists(_png_temp_path))
		std::filesystem::remove(_png_temp_path);
}



bool XPSkillIcon::SetImage()
{
	const bool result = XPImage.LoadFile(_png_temp_path, wxBITMAP_TYPE_PNG);
	XPWidth = -1;
	XPHeight = -1;
	return result;
}

void XPSkillIcon::paintEvent(wxPaintEvent& evt)
{
	if (this->IsShown() && XPImage.IsOk())
	{
		wxPaintDC dc(this);
		render(dc);
	}
}

void XPSkillIcon::paintNow()
{
	if (this->IsShown() && XPImage.IsOk())
	{
		wxClientDC dc(this);
		render(dc);
	}
}

void XPSkillIcon::render(wxDC& dc)
{
	int neww, newh;
	dc.GetSize(&neww, &newh);

	if (neww != XPWidth || newh != XPHeight)
	{
		XPResized = wxBitmap(XPImage.Scale(neww, newh));
		XPWidth = neww;
		XPHeight = newh;
		dc.DrawBitmap(XPResized, 0, 0, false);
	}
	else
	{
		dc.DrawBitmap(XPResized, 0, 0, false);
	}
}

void XPSkillIcon::OnSize(wxSizeEvent& event)
{
	this->Refresh();
	event.Skip();
}


BEGIN_EVENT_TABLE(XPSkillIcon, wxWindow)

EVT_PAINT(XPSkillIcon::paintEvent)
EVT_SIZE(XPSkillIcon::OnSize)

END_EVENT_TABLE()
