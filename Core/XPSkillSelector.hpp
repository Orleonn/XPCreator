#pragma once

#include "XPDefs.hpp"

#define XP_SKILL_PROPS_BAR_WIDTH 300
#define XP_SKILL_PROPS_BAR_HEIGHT 200
#define XP_SKILL_PROPS_CAP L"Skill properties"

class XPSkillPropsBar : public wxChoice
{
public:
	XPSkillPropsBar(wxWindow* parent, wxWindowID id, const wxPoint& pos)
		: wxChoice(parent, id,
			{ pos.x, pos.y + XP_SKILL_PROPS_BAR_HEIGHT - 75 },
			{ XP_SKILL_PROPS_BAR_WIDTH, 75 },
			0, nullptr, wxCB_SORT),
		mXPCallbackAdd(nullptr, nullptr), mXPCallbackRemove(nullptr, nullptr)
	{
		mSkillDesc = new wxTextCtrl(parent, wxID_ANY, wxEmptyString, pos,
			{ XP_SKILL_PROPS_BAR_WIDTH, XP_SKILL_PROPS_BAR_HEIGHT - 80 },
			wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH);

		const wxSize defaultButtonSize(40, 25);

		mXPBtnAdd = new wxButton(parent, wxID_ANY, L">>",
			{ pos.x + XP_SKILL_PROPS_BAR_WIDTH + 5, pos.y + 27 },
			defaultButtonSize);
		mXPBtnAdd->Enable(false);
		mXPBtnAdd->Bind(wxEVT_BUTTON, &XPSkillPropsBar::XPOnButtonAddItem, this);

		const wxPoint btnAddPos = mXPBtnAdd->GetPosition();
		const wxSize btnAddSize = mXPBtnAdd->GetSize();

		mXPBtnRemove = new wxButton(parent, wxID_ANY, L"<<",
			{ btnAddPos.x, btnAddPos.y + btnAddSize.y + 5 },
			defaultButtonSize);
		mXPBtnRemove->Enable(false);
		mXPBtnRemove->Bind(wxEVT_BUTTON, &XPSkillPropsBar::XPOnButtonRemoveItem, this);

		mXPSelectedList = new wxListBox(parent, wxID_ANY,
			{ btnAddPos.x + btnAddSize.x + 5, pos.y },
			mSkillDesc->GetSize(),
			0, nullptr,
			wxLB_EXTENDED | wxLB_SORT);
		mXPSelectedList->Bind(wxEVT_LISTBOX, &XPSkillPropsBar::XPOnSelectRightItem, this);

		mXPPropsCap = new wxStaticText(parent, wxID_ANY,
			XP_SKILL_PROPS_CAP, { pos.x, pos.y - 40 },
			{ mXPSelectedList->GetPosition().x + mXPSelectedList->GetSize().x - pos.x, 40 },
			wxALIGN_CENTRE_HORIZONTAL);
		mXPPropsCap->SetFont(mXPPropsCap->GetFont().Scale(1.75F));
	}

	virtual bool ProcessEvent(wxEvent& event) override
	{
		if (event.GetId() == this->GetId() && event.GetEventType() == wxEVT_CHOICE)
		{
			this->XPOnSelectLeftItem();
		}
		return wxChoice::ProcessEvent(event);
	}

	wxTextCtrl* GetXPDescriptionControl() const { return mSkillDesc; }
	wxListBox* GetXPSelectedList() const { return mXPSelectedList; }

	using XPCAllbackProc = void(wxWindow::*)(const wxString&);

	template <class _Member>
	void SetXPAddItemCallback(void(_Member::* func)(const wxString&), _Member* handle)
	{
		mXPCallbackAdd.first = static_cast<wxWindow*>(handle);
		mXPCallbackAdd.second = static_cast<XPCAllbackProc>(func);
	}

	template <class _Member>
	void SetXPRemoveItemCallback(void(_Member::* func)(const wxString&), _Member* handle)
	{
		mXPCallbackRemove.first = static_cast<wxWindow*>(handle);
		mXPCallbackRemove.second = static_cast<XPCAllbackProc>(func);
	}

private:
	void XPUpdateCaptionName()
	{
		const unsigned int count = mXPSelectedList->GetCount();
		if (count == 0)
			mXPPropsCap->SetLabelText(XP_SKILL_PROPS_CAP);
		else
			mXPPropsCap->SetLabelText(XPSprintf(L"%s x%u", XP_SKILL_PROPS_CAP, count));
	}

	void XPOnSelectLeftItem()
	{
		mXPBtnAdd->Enable(this->GetCurrentSelection() != wxNOT_FOUND);
	}

	void XPOnSelectRightItem(wxCommandEvent& event)
	{
		wxArrayInt arr;
		mXPSelectedList->GetSelections(arr);
		mXPBtnRemove->Enable(!arr.empty());
	}

	void XPOnButtonAddItem(wxCommandEvent& event)
	{
		mXPBtnAdd->Enable(false);
		const int id = this->GetCurrentSelection();
		if (id == wxNOT_FOUND) return;

		const wxString selected = this->GetString(id);
		this->SetSelection(wxNOT_FOUND);
		this->Delete(id);
		mXPSelectedList->Append(selected);
		mSkillDesc->Clear();
		XPUpdateCaptionName();
		if (mXPCallbackAdd.first && mXPCallbackAdd.second)
		{
			(mXPCallbackAdd.first->*mXPCallbackAdd.second)(selected);
		}
	}

	void XPOnButtonRemoveItem(wxCommandEvent& event)
	{
		mXPBtnRemove->Enable(false);
		wxArrayInt arr;
		mXPSelectedList->GetSelections(arr);
		if (arr.empty()) return;
		
		wxString item;
		for (const auto& id : arr)
		{
			item = mXPSelectedList->GetString(id);
			this->AppendString(item);
			mXPSelectedList->Delete(id);
			if (mXPCallbackRemove.first && mXPCallbackRemove.second)
			{
				(mXPCallbackRemove.first->*mXPCallbackRemove.second)(item);
			}
		}
		XPUpdateCaptionName();
	}

private:
	std::pair<wxWindow*, XPCAllbackProc> mXPCallbackAdd, mXPCallbackRemove;
	wxTextCtrl* mSkillDesc;
	wxListBox* mXPSelectedList;
	wxButton* mXPBtnAdd, *mXPBtnRemove;
	wxStaticText* mXPPropsCap;
};
