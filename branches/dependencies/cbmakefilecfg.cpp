#include "cbmakefilecfg.h"

//(*InternalHeaders(cbmakefilecfg)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

#include <configmanager.h>

//(*IdInit(cbmakefilecfg)
const long cbmakefilecfg::ID_STATICTEXT1 = wxNewId();
const long cbmakefilecfg::ID_TEXTCTRL1 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX1 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(cbmakefilecfg,cbConfigurationPanel)
    //(*EventTable(cbmakefilecfg)
    //*)
END_EVENT_TABLE()

cbmakefilecfg::cbmakefilecfg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(cbmakefilecfg)
    Create(parent,id,wxDefaultPosition,wxSize(572,82),wxTAB_TRAVERSAL,_T("wxPanel"));
    m_pStaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Filename :"),wxPoint(1,6),wxDefaultSize,0,_T("ID_STATICTEXT1"));
    m_pTCFilename = new wxTextCtrl(this,ID_TEXTCTRL1,_T("Makefile.gen"),wxPoint(62,3),wxSize(510,21),0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
    m_pCBOverwrite = new wxCheckBox(this,ID_CHECKBOX1,_("Overwrite if exists"),wxPoint(63,28),wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
    m_pCBOverwrite->SetValue(false);
    m_pCBSilence = new wxCheckBox(this,ID_CHECKBOX2,_("Add \'@\' prefix for commands"),wxPoint(64,47),wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX2"));
    m_pCBSilence->SetValue(false);
    //*)
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cbMakefileGen"));
    m_pTCFilename->SetValue(cfg->Read(_T("/filename"),_T("Makefile")));
    m_pCBOverwrite->SetValue(cfg->ReadBool(_T("/overwrite"),true));
    m_pCBSilence->SetValue(cfg->ReadBool(_T("/silence"),true));
}

cbmakefilecfg::~cbmakefilecfg()
{
    //(*Destroy(cbmakefilecfg)
    //*)
}

void cbmakefilecfg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cbMakefileGen"));
    cfg->Write(_T("/filename"),(const wxString &)m_pTCFilename->GetValue());
    cfg->Write(_T("/overwrite"),(bool)m_pCBOverwrite->GetValue());
    cfg->Write(_T("/silence"),(bool)m_pCBSilence->GetValue());
}
