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

#include "ShapeFusionDocManager.h"

#include "BigEndianBuffer.h"

#include <wx/filename.h>
#include <wx/wfstream.h>

static wxDocTemplate* FindTemplate(wxList& templates, const wxString& description)
{
	for (unsigned int i = 0; i < templates.GetCount(); ++i) {
		wxDocTemplate* temp = reinterpret_cast<wxDocTemplate*>(templates.Item(i)->GetData());
		if (temp->GetDescription() == description) {
			return temp;
		}
	}

	return 0;
} 

wxDocTemplate* ShapeFusionDocManager::FindTemplateForPath(const wxString& path)
{
	// if we recognize the extension, assume it's correct
	wxFileName filename(path);
	wxString ext = filename.GetExt();
	
	if (ext == _T("sndA") || ext == _T("snd2")) {
		return ::FindTemplate(GetTemplates(), _("Sounds"));
	} else if (ext == _T("shpA") || ext == _T("shp2")) {
		return ::FindTemplate(GetTemplates(), _T("Shapes"));
	}

	wxFileInputStream stream(path);
	if (!stream.IsOk()) {
		return 0;
	}


	// check for sounds file
	{
		stream.SeekI(0, wxFromStart);

		unsigned char header[8];
		stream.Read(header, 8);

		BigEndianBuffer buffer(header, 4);
		
		unsigned long version = buffer.ReadULong();
		if ((version == 0 || version == 1) 
		    && strncmp(reinterpret_cast<const char*>(&header[4]), "snd2", 4) == 0) {
			return ::FindTemplate(GetTemplates(), _("Sounds"));
		}
	}

	stream.SeekI(0, wxFromEnd);
	long filesize = stream.TellI();
	stream.SeekI(0, wxFromStart);

	// check for shapes file
	{
		bool is_shapes = true;
		for (int i = 0; i < 32; ++i) {
			unsigned char header[32];
			stream.Read(header, 32);
			
			BigEndianBuffer buffer(header, 20);
			unsigned long status_flags = buffer.ReadULong();
			long offset = buffer.ReadLong();
			long length = buffer.ReadLong();
			long offset16 = buffer.ReadLong();
			long length16 = buffer.ReadLong();

			if (status_flags != 0
			    || (offset != -1 && (offset >= filesize || offset + length > filesize))
			    || (offset16 != -1 && (offset16 >= filesize || offset16 + length16 > filesize))) {
				is_shapes = false;
				break;
			}
		}

		if (is_shapes) {
			return ::FindTemplate(GetTemplates(), _("Shapes"));
		}
	}

	// check for physics
	{
		stream.SeekI(0, wxFromStart);
		unsigned char header[128];
		stream.Read(header, 128);
		
		BigEndianBuffer header_buffer(header, 128);
		int version = header_buffer.ReadShort();
		int data_version = header_buffer.ReadShort();
		if ((version == 0 || version == 1 || version == 2 || version == 4) && (data_version == 0 || data_version == 1 || data_version == 2)) {
			header_buffer.Position(72);
			long directory_offset = header_buffer.ReadLong();
			if (directory_offset >= filesize)
				return 0;

			unsigned char tag[4];
			stream.Read(tag, 4);
			if (strncmp(reinterpret_cast<const char*>(tag), "MNpx", 4) == 0) {
				return ::FindTemplate(GetTemplates(), _("Physics"));
			}
		}
	}
	
	return 0;	
}

wxDocTemplate* ShapeFusionDocManager::SelectDocumentPath(wxDocTemplate** templates, int noTemplates, wxString& path, long, bool)
{
	wxString pathTmp = wxFileSelector(_("Select a file"),
					    m_lastDirectory);

	wxDocTemplate* theTemplate = 0;

	if (!pathTmp.empty()) {
		if (!wxFileExists(pathTmp))
		{
			wxString msgTitle;
			if (!wxTheApp->GetAppName().empty()) {
				msgTitle = wxTheApp->GetAppName();
			} else {
				msgTitle = wxString(_("File error"));
			}

			(void)wxMessageBox(_("Sorry, could not open this file."), msgTitle, wxOK | wxICON_EXCLAMATION);
			
			path = wxEmptyString;
			return 0;
		}
		m_lastDirectory = wxPathOnly(pathTmp);
		
		path = pathTmp;

		theTemplate = FindTemplateForPath(path);
		
		if (!theTemplate)
		{
			(void) wxMessageBox(_("Sorry, the format for this file is unknown."), _("Open File"), wxOK | wxICON_EXCLAMATION);
		}
		
		return FindTemplateForPath(path);
	} else {
		path = wxEmptyString;
	}

	return theTemplate;
					    
}
