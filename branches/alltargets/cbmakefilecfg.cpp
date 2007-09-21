#include "cbmakefilecfg.h"

//(*InternalHeaders(cbmakefilecfg)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

#include <configmanager.h>

//(*IdInit(cbmakefilecfg)
const long cbmakefilecfg::ID_STATICTEXT1 = wxNewId();
const long cbmakefilecfg::ID_TEXTCTRL1 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX1 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX2 = wxNewId();
const long cbmakefilecfg::ID_CHECKBOX3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(cbmakefilecfg,cbConfigurationPanel)
    //(*EventTable(cbmakefilecfg)
    //*)
END_EVENT_TABLE()

cbmakefilecfg::cbmakefilecfg(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(cbmakefilecfg)
    Create(parent, id, wxDefaultPosition, wxSize(587,81), wxTAB_TRAVERSAL, _T("id"));
    m_pStaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Filename :"), wxPoint(4,8), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    m_pTCFilename = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(88,6), wxSize(481,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_pCBOverwrite = new wxCheckBox(this, ID_CHECKBOX1, _("Overwrite if exists"), wxPoint(88,28), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_pCBOverwrite->SetValue(false);
    m_pCBSilence = new wxCheckBox(this, ID_CHECKBOX2, _("Add \'@\' prefix for commands"), wxPoint(88,48), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_pCBSilence->SetValue(false);
    m_pCBAllTargets = new wxCheckBox(this, ID_CHECKBOX3, _("Generate for all targets (else being used active target only)"), wxPoint(88,68), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    m_pCBAllTargets->SetValue(false);
    //*)
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("cbMakefileGen"));
    m_pTCFilename->SetValue(cfg->Read(_T("/filename"),_T("Makefile")));
    m_pCBOverwrite->SetValue(cfg->ReadBool(_T("/overwrite"),true));
    m_pCBSilence->SetValue(cfg->ReadBool(_T("/silence"),true));
    m_pCBAllTargets->SetValue(cfg->ReadBool(_T("/alltargets"),false));
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
    cfg->Write(_T("/alltargets"),(bool)m_pCBAllTargets->GetValue());
}
