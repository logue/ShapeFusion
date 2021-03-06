/*
 * This file is part of ShapeFusion (Copyright 2000 Tito Dal Canton)
 *
 * ShapeFusion is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ShapeFusion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ShapeFusion; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/wfstream.h"

#include "../ShapeFusionApp.h"
#include "../ShapeFusionMenus.h"
#include "../DefaultNames.h"
#include "SoundsView.h"

BEGIN_EVENT_TABLE(SoundsView, wxView)
	EVT_LISTBOX(SOUND_CLASS_LIST, SoundsView::SoundClassChanged)
	EVT_TEXT(SOUND_CLASS_ID_FIELD, SoundsView::SoundClassIdChanged)
	EVT_RADIOBOX(SOUND_VOLUME_RADIO_BUTTON, SoundsView::VolumeButtonChanged)
	EVT_CHOICE(SOUND_CHANCE_MENU, SoundsView::ChanceMenuChanged)
	EVT_CHECKBOX(SOUND_FLAGS_RESTART, SoundsView::FlagsChanged)
	EVT_CHECKBOX(SOUND_FLAGS_ABORT, SoundsView::FlagsChanged)
	EVT_CHECKBOX(SOUND_FLAGS_RESIST, SoundsView::FlagsChanged)
	EVT_CHECKBOX(SOUND_FLAGS_CHANGE, SoundsView::FlagsChanged)
	EVT_CHECKBOX(SOUND_FLAGS_OBSTRUCTED, SoundsView::FlagsChanged)
	EVT_CHECKBOX(SOUND_FLAGS_MOBSTRUCTED, SoundsView::FlagsChanged)
	EVT_CHECKBOX(SOUND_FLAGS_AMBIENT, SoundsView::FlagsChanged)
	EVT_LISTBOX(SOUND_EIGHT_BIT_PERMUTATIONS_LIST, SoundsView::SoundPermutationSelected)
	EVT_LISTBOX(SOUND_SIXTEEN_BIT_PERMUTATIONS_LIST, SoundsView::SoundPermutationSelected)
	EVT_LISTBOX_DCLICK(SOUND_EIGHT_BIT_PERMUTATIONS_LIST, SoundsView::SoundPermutationDoubleClicked)
	EVT_LISTBOX_DCLICK(SOUND_SIXTEEN_BIT_PERMUTATIONS_LIST, SoundsView::SoundPermutationDoubleClicked)
	EVT_MENU(EDIT_MENU_DELETE, SoundsView::MenuDelete)
	EVT_MENU(SOUNDS_MENU_ADDCLASS, SoundsView::MenuAddSoundClass)
	EVT_MENU(SOUNDS_MENU_EXPORT, SoundsView::MenuExportSound)
	EVT_MENU(SOUNDS_MENU_IMPORT, SoundsView::MenuImportSound)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SoundsView, wxView)

SoundsView::SoundsView(): mSoundClass(wxNOT_FOUND), mSoundSource(wxNOT_FOUND), mSoundPermutation(wxNOT_FOUND)
{
	frame = NULL;
	menubar = NULL;
	payload = NULL;
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool SoundsView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
	// Make Traslatable
	wxLocale::AddCatalogLookupPathPrefix("lang");

	wxLocale locale;
	// Add Japanese support.
	locale.Init( wxLANGUAGE_JAPANESE, wxLOCALE_DONT_LOAD_DEFAULT );
	// ShapeFusion Dictionary
	locale.AddCatalog("messages");
	// WxWidget Standard Dictionary
	locale.AddCatalog("wxstd");
	
	wxString frameTitle = _("ShapeFusion : Sounds : ");

	frameTitle.Append(doc->GetFilename());
	
    frame = wxGetApp().CreateChildFrame(doc, this, frameTitle, wxPoint(0, 0), wxSize(600, 400), wxDEFAULT_FRAME_STYLE);// & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX));
	
	payload = (SoundsDocument*)doc;
	
	menubar = frame->GetMenuBar();	
	CreateSoundsMenu(menubar);
	
	// Because we can always add sound classes
	menubar->Enable(SOUNDS_MENU_ADDCLASS, true);

	wxString volume_labels[] = { _("Soft"), _("Medium"), _("Loud") };
	wxString chances_labels[] = { wxT("100%"), wxT("90%"), wxT("80%"), wxT("70%"), wxT("60%"), wxT("50%"), wxT("40%"), wxT("30%"), wxT("20%"), wxT("10%") };

	main_panel = new wxPanel(frame);
	main_panel->Show();
	
	sound_class_text = new wxStaticText(main_panel, wxID_ANY, _("Sound classes: "));
	sound_class_id_text = new wxStaticText(main_panel, wxID_ANY, _("Class ID: "));
	sound_class_id_field = new wxTextCtrl(main_panel, SOUND_CLASS_ID_FIELD, wxT(""));
	
	sound_class_number_text = new wxStaticText(main_panel, wxID_ANY, _("Class number: "));
	sound_class_number_field = new wxStaticText(main_panel, SOUND_CLASS_NUMBER_FIELD, wxT(""));
	
	sound_class_list = new wxListBox(main_panel, (wxWindowID)SOUND_CLASS_LIST);
	
	sound_flag_restart_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_RESTART, _("Cannot be restarted"));
	sound_flag_abort_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_ABORT, _("Does not self-abort"));
	sound_flag_resist_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_RESIST, _("Resists pitch changes"));
	sound_flag_change_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_CHANGE, _("Can't change pitch"));
	sound_flag_obstructed_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_OBSTRUCTED, _("Can't be obstructed"));
	sound_flag_mobstructed_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_MOBSTRUCTED, _("Can't be media obstructed"));
	sound_flag_ambient_checkbox = new wxCheckBox(main_panel, SOUND_FLAGS_AMBIENT, _("Is ambient"));
	
	sound_volume_radio_button = new wxRadioBox(main_panel, SOUND_VOLUME_RADIO_BUTTON, _("Volume"), wxDefaultPosition, wxDefaultSize, 3, volume_labels, 3, wxRA_SPECIFY_COLS);
	
	sound_chance_text = new wxStaticText(main_panel, wxID_ANY, _("Chance: "));
	sound_chance_menu = new wxChoice(main_panel, SOUND_CHANCE_MENU, wxDefaultPosition, wxDefaultSize, 10, chances_labels);
	
	sound_low_pitch_text = new wxStaticText(main_panel, wxID_ANY, _("Low pitch: "));
	sound_low_pitch_field = new wxTextCtrl(main_panel, SOUND_LOW_PITCH_FIELD);
	sound_high_pitch_text = new wxStaticText(main_panel, wxID_ANY, _("High pitch: "));
	sound_high_pitch_field = new wxTextCtrl(main_panel, SOUND_HIGH_PITCH_FIELD);
	
	sound_eight_bit_text = new wxStaticText(main_panel, wxID_ANY, _("8-bit sounds:"));
	sound_eight_bit_list = new wxListBox(main_panel, (wxWindowID)SOUND_EIGHT_BIT_PERMUTATIONS_LIST);
	
	sound_sixteen_bit_text = new wxStaticText(main_panel, wxID_ANY, _("16-bit sounds: "));
	sound_sixteen_bit_list = new wxListBox(main_panel, (wxWindowID)SOUND_SIXTEEN_BIT_PERMUTATIONS_LIST);
	
	sound_remap_check_box = new wxCheckBox(main_panel, SOUND_REMAP_CHECK_BOX, _("Remap 8-bit"));
	
	frame_sizer = new wxBoxSizer(wxHORIZONTAL);
	sound_class_sizer = new wxBoxSizer(wxVERTICAL);
	sound_class_header_sizer = new wxFlexGridSizer(2, 2, 0, 0);
	sound_editor_sizer = new wxBoxSizer(wxVERTICAL);
	sound_flags_sizer = new wxStaticBoxSizer(wxVERTICAL, main_panel, wxT("Flags"));
	sound_menus_sizer = new wxFlexGridSizer(2, 3, 0, 0);
	sound_permutation_sizer = new wxBoxSizer(wxHORIZONTAL);
	sound_eight_bit_sizer = new wxBoxSizer(wxVERTICAL);
	sound_sixteen_bit_sizer = new wxBoxSizer(wxVERTICAL);
	
	sound_class_header_sizer->Add(sound_class_id_text, 0, wxALIGN_CENTER_VERTICAL, 0);
	sound_class_header_sizer->Add(sound_class_id_field, 0, 0, 0);
	sound_class_header_sizer->Add(sound_class_number_text, 0, wxALIGN_CENTER_VERTICAL, 0);
	sound_class_header_sizer->Add(sound_class_number_field, 0, 0, 0);
	
	sound_class_sizer->Add(sound_class_text, 0, 0, 0);
	sound_class_sizer->Add(sound_class_header_sizer, 0, 0, 0);
	sound_class_sizer->Add(sound_class_list, 1, wxEXPAND, 0);
	
	sound_flags_sizer->Add(sound_flag_restart_checkbox, 0, 0, 0);
	sound_flags_sizer->Add(sound_flag_abort_checkbox, 0, 0, 0);
	sound_flags_sizer->Add(sound_flag_resist_checkbox, 0, 0, 0);
	sound_flags_sizer->Add(sound_flag_change_checkbox, 0, 0, 0);
	sound_flags_sizer->Add(sound_flag_obstructed_checkbox, 0, 0, 0);
	sound_flags_sizer->Add(sound_flag_mobstructed_checkbox, 0, 0, 0);
	sound_flags_sizer->Add(sound_flag_ambient_checkbox, 0, 0, 0);
	
	sound_menus_sizer->Add(sound_chance_text, 0, wxALIGN_CENTER_VERTICAL, 0);
	sound_menus_sizer->Add(sound_chance_menu, 0, 0, 0);
	sound_menus_sizer->Add(sound_low_pitch_text, 0, wxALIGN_CENTER_VERTICAL, 0);
	sound_menus_sizer->Add(sound_low_pitch_field, 0, 0, 0);
	sound_menus_sizer->Add(sound_high_pitch_text, 0, wxALIGN_CENTER_VERTICAL, 0);
	sound_menus_sizer->Add(sound_high_pitch_field, 0, 0, 0);
	
	sound_eight_bit_sizer->Add(sound_eight_bit_text, 0, 0, 0);
	sound_eight_bit_sizer->Add(sound_eight_bit_list, 1, wxEXPAND | wxRIGHT, 5);
	sound_sixteen_bit_sizer->Add(sound_sixteen_bit_text, 0, 0, 0);
	sound_sixteen_bit_sizer->Add(sound_sixteen_bit_list, 1, wxEXPAND, 0);
	sound_sixteen_bit_sizer->Add(sound_remap_check_box, 0, 0, 0);
	
	sound_permutation_sizer->Add(sound_eight_bit_sizer, 1, wxEXPAND, 0);
	sound_permutation_sizer->Add(sound_sixteen_bit_sizer, 1, wxEXPAND, 0);
	
	sound_editor_sizer->Add(sound_flags_sizer, 0, 0, 0);
	sound_editor_sizer->AddSpacer(1);
	sound_editor_sizer->Add(sound_volume_radio_button, 0, 0, 0);
	sound_editor_sizer->AddSpacer(1);
	sound_editor_sizer->Add(sound_menus_sizer, 0, 0, 0);
	sound_editor_sizer->AddSpacer(5);
	sound_editor_sizer->Add(sound_permutation_sizer, 1, wxEXPAND, 0);
	
	frame_sizer->Add(sound_class_sizer, 0, wxEXPAND | wxALL, 5);
	frame_sizer->AddSpacer(5);
	frame_sizer->Add(sound_editor_sizer, 0, wxEXPAND | wxALL, 5);
	
	main_panel->SetSizer(frame_sizer);
	frame_sizer->Layout();
	frame_sizer->SetSizeHints(frame);
	
	frame->Show(true);
#ifdef __X__
	// X seems to require a forced resize
	int x, y;
	frame->GetSize(&x, &y);
	frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif
	
	return true;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void SoundsView::OnDraw(wxDC *dc)
{
}

void SoundsView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
	bool	gequals = true;

	sound_class_list->Clear();
	for (unsigned int i = 0; i < payload->GetSoundCount(); i++) {
		sound_class_list->Append(GetName(wxT("sound"), i));
		// We check if there is a difference between 8-bit and 16-bit
		// SoundsDefinitions
		SoundsDefinition	*def8 = payload->Get8BitSoundDefinition(i),
							*def16 = payload->Get16BitSoundDefinition(i);
		bool				equals = false;

		if (def8 != NULL && def16 != NULL)
			equals = def8->HaveSameAttributesAs(*def16);
		else if (def8 == NULL && def16 == NULL)
			equals = true;

		if (!equals)
			wxLogDebug(_("Sound source different at %d"), i);
		gequals = gequals && equals;
	}
	if (!gequals) {
		// FIXME : Update this when we have a "complete" editor...
		wxMessageDialog msg(frame,
						_("It seems 8-bit and 16-bit versions of some of this Sound file "
						"sounds have differences. This editor will replace 16-bit sounds "
						"flags with those from 8-bit sounds, to ensure consistency. "
						"If you really need to be able to change 16-bit flags independently, "
						"please file a feature request."),
						_("Warning"), wxOK | wxICON_WARNING);
						
		msg.ShowModal();
	}
	Update();
}

void SoundsView::Update(void)
{
	// We disable our menuitems, in case selection is invalid 
	menubar->Enable(SOUNDS_MENU_IMPORT, false);
	menubar->Enable(SOUNDS_MENU_EXPORT, false);
	menubar->Enable(EDIT_MENU_DELETE, false);
	
	if (sound_class_list->GetCount() == 0) {
		// There is no sound class
		// We cannot have a selection
		mSoundPermutation = wxNOT_FOUND;
		mSoundSource = wxNOT_FOUND;

	} else {
		// We have a sound class
		
		// Make sure we always have something selected
		mSoundClass = sound_class_list->GetSelection();
		if (mSoundClass == wxNOT_FOUND) {
			wxLogDebug(wxT("[SoundsView] There is no sound selected. Selecting first item..."));
			sound_class_list->SetSelection(0);
			mSoundClass = sound_class_list->GetSelection();
		}
		
		// We build the permutations listbox
		sound_eight_bit_list->Clear();
		sound_sixteen_bit_list->Clear();
		
		SoundsDefinition *def = payload->Get8BitSoundDefinition(mSoundClass);
		if (def) {
			for (unsigned int i = 0; i < def->GetPermutationCount(); i++) {
				sound_eight_bit_list->Append(wxString::Format(wxT("%d"), def->GetPermutation(i)->Size()));
			}
		}
		
		def = payload->Get16BitSoundDefinition(mSoundClass);
		if (def) {
			for (unsigned int i = 0; i < def->GetPermutationCount(); i++) {
				sound_sixteen_bit_list->Append(wxString::Format(wxT("%d"), def->GetPermutation(i)->Size()));
			}
		}
		
		// As soon as we have a sound class selected, we can
		// - import a sound into it
		// - delete it
		menubar->Enable(SOUNDS_MENU_IMPORT, true);
		menubar->Enable(EDIT_MENU_DELETE, true);
		
		if (payload->Get8BitSoundDefinition(mSoundClass)->GetPermutationCount() != 0) {
			// There is 8-bit sounds, we select first
			mSoundSource = 0;
			mSoundPermutation = 0;
			sound_eight_bit_list->SetSelection(0);
			// We deselect 16-bit list
			sound_sixteen_bit_list->SetSelection(wxNOT_FOUND);
		} else {
			// There is no 8-bit sounds
			if (payload->Get16BitSoundDefinition(mSoundClass)->GetPermutationCount() != 0) {
				// We have 16-bit sounds, we select this one...
				mSoundSource = 1;
				mSoundPermutation = 0;
				sound_sixteen_bit_list->SetSelection(0);
				// We deselect 8-bit list
				sound_eight_bit_list->SetSelection(wxNOT_FOUND);
			} else {
				// There is neither 8-bit nor 16-bit sounds, don't select anything
				sound_eight_bit_list->SetSelection(wxNOT_FOUND);
				sound_sixteen_bit_list->SetSelection(wxNOT_FOUND);
				mSoundSource = wxNOT_FOUND;
				mSoundPermutation = wxNOT_FOUND;
			}
		}
		
		// We enable this, our selection is valid...
		menubar->Enable(SOUNDS_MENU_EXPORT, true);
		
		def = payload->Get8BitSoundDefinition(mSoundClass);
				
		sound_class_number_field->SetLabel(wxString::Format(wxT("%d"), mSoundClass));
		sound_class_id_field->ChangeValue(wxString::Format(wxT("%d"), def->GetSoundCode()));
		
		sound_volume_radio_button->SetSelection(def->GetBehaviorIndex());
		sound_chance_menu->SetSelection(def->GetChance());
		
		sound_flag_restart_checkbox->SetValue(def->IsNotRestartable());
		sound_flag_abort_checkbox->SetValue(def->IsNotSelfAbortable());
		sound_flag_resist_checkbox->SetValue(def->IsPitchChangeResistant());
		sound_flag_change_checkbox->SetValue(def->IsNotPitchChangeable());
		sound_flag_obstructed_checkbox->SetValue(def->IsNotObstructed());
		sound_flag_mobstructed_checkbox->SetValue(def->IsNotMediaObstructed());
		sound_flag_ambient_checkbox->SetValue(def->IsAmbient());
		sound_low_pitch_field->ChangeValue(wxString::Format(wxT("%g"), def->GetLowPitch()));
		sound_high_pitch_field->ChangeValue(wxString::Format(wxT("%g"), def->GetHighPitch()));
	}
}

// Clean up windows used for displaying the view.
bool SoundsView::OnClose(bool deleteWindow)
{
	if (!GetDocument()->Close())
		return false;
	SetFrame((wxFrame *) NULL);
	Activate(false);
	if (deleteWindow) {
		delete frame;
		return true;
	}
	return true;
}

void SoundsView::SoundClassChanged(wxCommandEvent &e)
{
	Update();
}

void SoundsView::SoundClassIdChanged(wxCommandEvent& e)
{
	long v;
	if (e.GetString().ToLong(&v))
	{
		SoundsDefinition* def = payload->Get8BitSoundDefinition(mSoundClass);
		def->SetSoundCode(v);
		
		def = payload->Get16BitSoundDefinition(mSoundClass);
		def->SetSoundCode(v);
	}
	GetDocument()->Modify(true);
}

void SoundsView::VolumeButtonChanged(wxCommandEvent &e)
{
	SoundsDefinition *def = payload->Get8BitSoundDefinition(mSoundClass);
	def->SetBehaviorIndex(sound_volume_radio_button->GetSelection());
	
	def = payload->Get16BitSoundDefinition(mSoundClass);
	def->SetBehaviorIndex(sound_volume_radio_button->GetSelection());
	GetDocument()->Modify(true);
}

void SoundsView::ChanceMenuChanged(wxCommandEvent &e)
{
	SoundsDefinition *def = payload->Get8BitSoundDefinition(mSoundClass);
	def->SetChance(sound_chance_menu->GetSelection());
	
	def = payload->Get16BitSoundDefinition(mSoundClass);
	def->SetChance(sound_chance_menu->GetSelection());
	GetDocument()->Modify(true);
}

void SoundsView::FlagsChanged(wxCommandEvent &e)
{
	SoundsDefinition *def8 = payload->Get8BitSoundDefinition(mSoundClass);
	SoundsDefinition *def16 = payload->Get16BitSoundDefinition(mSoundClass);
	switch (e.GetId()) {
		case SOUND_FLAGS_RESTART:
			def8->SetNotRestartable(e.IsChecked());
			def16->SetNotRestartable(e.IsChecked());
			break;
		case SOUND_FLAGS_ABORT:
			def8->SetNotSelfAbortable(e.IsChecked());
			def16->SetNotSelfAbortable(e.IsChecked());
			break;
		case SOUND_FLAGS_RESIST:
			def8->SetPitchChangeResistant(e.IsChecked());
			def16->SetPitchChangeResistant(e.IsChecked());
			break;
		case SOUND_FLAGS_CHANGE:
			def8->SetNotPitchChangeable(e.IsChecked());
			def16->SetNotPitchChangeable(e.IsChecked());
			break;
		case SOUND_FLAGS_OBSTRUCTED:
			def8->SetNotObstructed(e.IsChecked());
			def16->SetNotObstructed(e.IsChecked());
			break;
		case SOUND_FLAGS_MOBSTRUCTED:
			def8->SetNotMediaObstructed(e.IsChecked());
			def16->SetNotMediaObstructed(e.IsChecked());
			break;
		case SOUND_FLAGS_AMBIENT:
			def8->SetAmbient(e.IsChecked());
			def16->SetAmbient(e.IsChecked());
			break;
		default:
			wxLogDebug(wxT("Invalid control id in FlagsChanged"));
			break;
	}
	GetDocument()->Modify(true);
}

void SoundsView::LowPitchValueChanged(wxScrollEvent &e)
{
	long int l;

	sound_low_pitch_field->GetValue().ToLong(&l);
	
	SoundsDefinition *def = payload->Get8BitSoundDefinition(mSoundClass);
	def->SetLowPitch(l);
	
	def = payload->Get16BitSoundDefinition(mSoundClass);
	def->SetLowPitch(l);
	GetDocument()->Modify(true);
}

void SoundsView::HighPitchValueChanged(wxScrollEvent &e)
{
	long int l;

	sound_high_pitch_field->GetValue().ToLong(&l);
	
	SoundsDefinition *def = payload->Get8BitSoundDefinition(mSoundClass);
	def->SetHighPitch(l);
	
	def = payload->Get16BitSoundDefinition(mSoundClass);
	def->SetHighPitch(l);
	GetDocument()->Modify(true);
}

void SoundsView::MenuDelete(wxCommandEvent &e)
{
	wxWindow *win = sound_class_list->FindFocus();

	switch (win->GetId()) {
		case SOUND_CLASS_LIST:
			wxLogDebug(wxT("Delete Sound Class"));
			break;
		case SOUND_EIGHT_BIT_PERMUTATIONS_LIST:
		case SOUND_SIXTEEN_BIT_PERMUTATIONS_LIST:
		{
			if (mSoundClass == wxNOT_FOUND || mSoundSource == wxNOT_FOUND || mSoundPermutation == wxNOT_FOUND) {
				return;
			}
			
			SoundsDefinition* def = payload->GetSoundDefinition(mSoundSource, mSoundClass);
			def->DeletePermutation(mSoundPermutation);
			Update();
			GetDocument()->Modify(true);
			break;
		}
		default:
			break;
	}
}

void SoundsView::MenuAddSoundClass(wxCommandEvent &e)
{
	wxLogDebug(wxT("Adding an item"));
	payload->AddSoundDefinition();
	
	// We add the new Sound class item by hand
	sound_class_list->Append(wxString::Format(wxT("Sound %d"), sound_class_list->GetCount()));

	GetDocument()->Modify(true);
	
	Update();
}

void SoundsView::MenuImportSound(wxCommandEvent &e)
{
	wxWindow* w = wxWindow::FindFocus();
	SoundsDefinition* definition = 0;
	if (w == static_cast<wxWindow*>(sound_sixteen_bit_list)) {
		definition = payload->Get16BitSoundDefinition(mSoundClass);
	} else if (w == static_cast<wxWindow*>(sound_eight_bit_list)) {
		definition = payload->Get8BitSoundDefinition(mSoundClass);
	} else {
		wxMessageDialog msg(frame, _("Sorry, you need to select a sound class and 8-bit or 16-bit to import a sound"), _("Error: No selection"), wxOK | wxICON_EXCLAMATION);
		msg.ShowModal();
		return;
	}

	if (definition->GetPermutationCount() >= MAXIMUM_PERMUTATIONS_PER_SOUND) {
		wxMessageDialog msg(frame, _("There are already five permutations for this sound"), _("Error: permutation limit reached"), wxOK | wxICON_EXCLAMATION);
		msg.ShowModal();
		return;
	}
	
	wxFileDialog dlg(frame, _("Choose a sound file to add"), wxT(""), wxT(""), _("Common sound files (AIFF, WAV)|*.aif;*.wav"), wxFD_OPEN);
	if (dlg.ShowModal() == wxID_OK) {
		if (definition->NewPermutation(dlg.GetPath()) == NULL) {
			wxMessageDialog msg(frame, _("Error importing sound"), _("Error"), wxOK | wxICON_EXCLAMATION);
			msg.ShowModal();
			return;
		}
	}

	GetDocument()->Modify(true);

	Update();
}

void SoundsView::MenuExportSound(wxCommandEvent &e)
{
	if (mSoundClass == wxNOT_FOUND || mSoundSource == wxNOT_FOUND || mSoundPermutation == wxNOT_FOUND) {
		wxMessageDialog msg(frame, _("Sorry, you need to select a sound class and a permutation to export a sound"), _("Error : No selection"), wxOK | wxICON_EXCLAMATION);
		msg.ShowModal();
		return;
	}
	
	wxFileDialog dlg(frame, _("Choose a file name :"), wxT(""), wxString::Format(wxT("Sound %d-%d.wav"), mSoundClass, mSoundPermutation), _("WAV files (*.wav)|*.wav|AIFF files (*.aif)|*.aif"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (dlg.ShowModal() == wxID_OK) {
		SoundsDefinition	*def = payload->GetSoundDefinition(mSoundSource, mSoundClass);
		AppleSoundHeader	*sound = def->GetPermutation(mSoundPermutation);
		bool				result = false;

		switch (dlg.GetFilterIndex()) {
			case 0: // Selected *.wav
				result = sound->SaveToWave(dlg.GetPath());
				break;
			case 1: // Selected *.aif
				result = sound->SaveToAiff(dlg.GetPath());
				break;
			default:
				break;
		}

		if (!result)
			wxLogDebug(wxT("[SoundsView] Error exporting sound"));
	}
}

void SoundsView::SoundPermutationSelected(wxCommandEvent &e)
{
	// We unselect the other permutation field
	if (e.GetId() == SOUND_EIGHT_BIT_PERMUTATIONS_LIST) {
		wxLogDebug(wxT("Selected 8-bit"));
		sound_sixteen_bit_list->SetSelection(wxNOT_FOUND);
		mSoundSource = 0;
		mSoundPermutation = sound_eight_bit_list->GetSelection();
	} else if (e.GetId() == SOUND_SIXTEEN_BIT_PERMUTATIONS_LIST) {
		wxLogDebug(wxT("Selected 16-bit"));
		sound_eight_bit_list->SetSelection(wxNOT_FOUND);
		mSoundSource = 1;
		mSoundPermutation = sound_sixteen_bit_list->GetSelection();
	}
}

void SoundsView::SoundPermutationDoubleClicked(wxCommandEvent &e)
{
	SoundsDefinition *def = payload->GetSoundDefinition(mSoundSource, mSoundClass);
	def->GetPermutation(mSoundPermutation)->PlaySound();
}

