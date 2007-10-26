#ifndef CBMAKEFILEGENCONS_H
#define CBMAKEFILEGENCONS_H

//(*Headers(cbmakefilegencons)
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class cbmakefilegencons: public wxFrame
{
	public:

		cbmakefilegencons(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~cbmakefilegencons();

		//(*Declarations(cbmakefilegencons)
		wxBoxSizer* BoxSizer1;
		wxPanel* Panel1;
		wxBoxSizer* BoxSizer2;
		wxStaticText* StaticText1;
		wxButton* Button1;
		//*)

	protected:

		//(*Identifiers(cbmakefilegencons)
		static const long ID_PANEL1;
		static const long ID_STATICTEXT1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(cbmakefilegencons)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
