#include <sdk.h>
#include <wx/intl.h>
#include "Variable.hpp"


cbMGVariable::cbMGVariable()
{
    //ctor
}

cbMGVariable::~cbMGVariable()
{
    //dtor
}

void cbMGVariable::AddVariable(const wxString& pVarName,const wxString& pVarValue)
{
    long lIndex = GetVariableIndex( pVarName );
    if ( lIndex >=  0 )
    {
        m_VariableValue[ lIndex ] = pVarValue;
    }
    else
    {
        m_VariableName.Add(pVarName);
        m_VariableValue.Add(pVarValue);
    }
}

wxString cbMGVariable::GetVariable(const wxString& pVarName) const
{
    long lIndex = GetVariableIndex( pVarName );

    if ( lIndex >= 0 )
    {
        return m_VariableValue[ lIndex ];
    }
    else
    {
        return wxEmptyString;
    }
}

long cbMGVariable::GetVariableIndex(const wxString& pVarName) const
{
    unsigned long i;

    for ( i = 0; i < m_VariableName.GetCount(); i++ )
    {
        if ( m_VariableName[ i ] == pVarName )
        {
            return (long)i;
        }
    }
    return -1;
}

bool cbMGVariable::SaveAllVars( wxTextFile& pFile )
{
    unsigned long i;

    pFile.AddLine(_("# Project Variables start"));
    for ( i = 0; i < m_VariableName.GetCount(); i++ )
    {
        pFile.AddLine( m_VariableName[ i ] + _("=") + m_VariableValue[ i ] );
    }
    pFile.AddLine(_("# Project Variables end"));
    pFile.AddLine(wxEmptyString);
    return true;
}

void cbMGVariable::AppendValue(const wxString& pVarName,const wxString& pVarValue, const wxChar& pDelimiter)
{
    long lIndex = GetVariableIndex( pVarName );
    if ( lIndex >= 0 )
    {
        if ( m_VariableValue[ lIndex ].size() )
        {
            m_VariableValue[ lIndex ] += pDelimiter;
        }
        m_VariableValue[ lIndex ] += pVarValue;
    }
    else
    {
        AddVariable( pVarName, pVarValue );
    }
}

