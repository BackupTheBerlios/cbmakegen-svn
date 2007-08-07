#ifndef __MAKEFILE_HPP
#define __MAKEFILE_HPP

#include <wx/hashmap.h>
#include <cbproject.h>
#include "Rule.hpp"
#include "Variable.hpp"

class cbProject;

#include <wx/arrimpl.cpp>

WX_DECLARE_OBJARRAY(cbMGRule,cbMGArrayOfRules);
WX_DECLARE_OBJARRAY(ProjectFile*, cbMGSortFilesArray); // keep our own copy, to sort it by file weight (priority)

class cbMGMakefile
{
    cbMGArrayOfRules m_Rules;
    cbMGVariable m_Variables;
    wxChar       m_CommandPrefix;
    long         m_CommandPrefixRepeatCnt;
    cbProject    *m_pProj;
    wxString     m_Filename;
    wxString     m_Path;
    bool         m_IsSilence;
    bool         m_Overwrite;
public:
    cbMGMakefile(cbProject* ppProj, const wxString& pFileName,bool pOverwrite,bool pSilence);
    virtual ~cbMGMakefile();

    void SetCommandPrefix( const wxChar& pCommandPrefix );
    wxString GetCommandPrefix() const
    {
        wxString lRes;
        return lRes.Pad( m_CommandPrefixRepeatCnt, m_CommandPrefix );
    }
    void SetCommandPrefixRepeatCnt( long pCommandPrefixRepeatCnt );
    long GetCommandPrefixRepeatCnt() const
    {
        return m_CommandPrefixRepeatCnt;
    }
    bool SaveMakefile();
    void SetIsSilence( bool pIsSilence )
    {
        m_IsSilence = pIsSilence;
    }
protected:
    bool SaveAllRules( wxTextFile& pFile );
    cbMGSortFilesArray GetProjectFilesSortedByWeight(ProjectBuildTarget* ppTarget, bool pCompile, bool pLink);
private:
};

#endif

