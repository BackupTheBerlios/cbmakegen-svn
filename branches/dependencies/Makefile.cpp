#include <sdk.h>
#include <wx/textfile.h>
#include "Makefile.hpp"
#include <globals.h>
#include <messagemanager.h>
#include <compiler.h>
#include <compilerfactory.h>
#include <macrosmanager.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( cbMGArrayOfRules );
WX_DEFINE_OBJARRAY( cbMGSortFilesArray ); // keep our own copy, to sort it by file weight (priority)

static wxString sHeader = _T(
                              "# A simple makefile generator by KiSoft, 2007. mailto: kisoft@rambler.ru"
                          );

const wxString cmdbefore = _T( "commandsbeforebuild" );
const wxString cmdafter  = _T( "commandsafterbuild" );
const wxString cmdphony  = _T( ".PHONY" );
const wxString objs      = _T("$(OBJS)");
const wxString oobjs     = _T("$$(OBJS)");
const wxString cmdclean  = _T("clean");

cbMGMakefile::cbMGMakefile( cbProject* ppProject, const wxString& pFileName, bool pOverwrite,bool pSilence)
        : m_CommandPrefix( _T( '\t' ) )
        , m_CommandPrefixRepeatCnt( 1 )
        , m_pProj( ppProject )
        , m_Filename( pFileName )
        , m_Path( _T("") )
        , m_IsSilence( pSilence )
        , m_Overwrite( pOverwrite )
{
    //ctor
    m_Path = m_pProj->GetBasePath();
}

cbMGMakefile::~cbMGMakefile()
{
    //dtor
}

void cbMGMakefile::SetCommandPrefix( const wxChar& p_CommandPrefix )
{
    m_CommandPrefix = p_CommandPrefix;
}

void cbMGMakefile::SetCommandPrefixRepeatCnt( long p_CommandPrefixRepeatCnt )
{
    m_CommandPrefixRepeatCnt = p_CommandPrefixRepeatCnt;
}

int cbMGSortProjectFilesByWeight( ProjectFile*** p_ppOne, ProjectFile*** p_ppTwo )
{
    return ( *( *p_ppOne ) )->weight - ( *( *p_ppTwo ) )->weight;
}

cbMGSortFilesArray cbMGMakefile::GetProjectFilesSortedByWeight( ProjectBuildTarget* p_pTarget, bool p_Compile, bool p_Link )
{
    cbMGSortFilesArray files;
    for ( int i = 0; i < m_pProj->GetFilesCount(); ++i )
    {
        ProjectFile* pf = m_pProj->GetFile( i );
        // require compile
        if ( p_Compile && !pf->compile )
            continue;
        // require link
        if ( p_Link && !pf->link )
            continue;
        // if the file does not belong in this target (if we have a target), skip it
        if ( p_pTarget && ( pf->buildTargets.Index( p_pTarget->GetTitle() ) == wxNOT_FOUND ) )
            continue;
        files.Add( pf );
    }
    files.Sort( cbMGSortProjectFilesByWeight );
    return files;
}

bool cbMGMakefile::reLoadDependecies(const wxString &p_DepsFileName,ProjectBuildTarget *p_pTarget,Compiler* p_pCompiler)
{
    m_Deps.Clear();
    if ( !wxFileExists( p_DepsFileName ) ) return true;

    wxString l_Buf;
    wxString l_VarName;
    wxTextFile l_DepFile;
    bool IsSkipThisFile = true;
    if (l_DepFile.Open( p_DepsFileName, wxConvFile ))
    {
        for ( unsigned long i = 0; i < l_DepFile.GetLineCount(); i++ )
        {
            l_Buf = l_DepFile[i];
            l_Buf.Trim(true);
            // Wrong! Don't uncomment it! Being deleted '\t' symbol! l_Buf.Trim(false);
            if ( l_Buf.IsEmpty() )
            {
                l_VarName.Clear();
                IsSkipThisFile = true;
                continue;
            }
            if ( _T('#') == l_Buf[0] ) continue;
            if ( _T('\t') == l_Buf[0] )
            {
                if ( !IsSkipThisFile )
                {
                    if ( _T('"') == l_Buf[1] )
                    {
                        wxString l_TmpName = l_Buf.AfterFirst( _T('\t') );
                        if (!l_TmpName.IsEmpty() && l_TmpName.GetChar(0) == _T('"') && l_TmpName.Last() == _T('"'))
                            l_TmpName = l_TmpName.Mid(1, l_TmpName.Length() - 2);
                        QuoteStringIfNeeded( l_TmpName );
                        m_Deps.AppendValue( l_VarName, l_TmpName );
                    }
                }
            }
            else
            {
                l_VarName = l_Buf.AfterFirst(_T(' '));
                bool IsSource = l_VarName.Matches( _T("source:*") );
                if ( IsSource )
                {
                    l_VarName = l_VarName.AfterFirst( _T(':') );
                }
                /*
                 * You would MUST found source file and get his filename from project
                 * !!! .depend file content lowcase filenames
                 */
                wxFileName l_DepFileName = l_VarName;
                ProjectFile *pf = m_pProj->GetFileByFilename( l_DepFileName.GetFullPath(), l_DepFileName.IsRelative(), false );
                if ( pf )
                {
                    const pfDetails& pfd = pf->GetFileDetails( p_pTarget );
                    if ( p_pCompiler->GetSwitches().UseFullSourcePaths )
                    {
                        l_VarName = UnixFilename( pfd.source_file_absolute_native );
                    }
                    else
                    {
                        l_VarName = pfd.source_file;
                    }
                    QuoteStringIfNeeded( l_VarName );
                    IsSkipThisFile = false;
                }
                else
                {
                    IsSkipThisFile = true;
                }
            }
        }
    }
    /* FIXME (kisoftcb#1#): Next code for debug only!
    wxTextFile l_DebFile;
    l_DebFile.Create( _T("D:/DepsFile.log") );
    m_Deps.SaveAllVars( l_DebFile );
    l_DebFile.Write();
    l_DebFile.Close(); */
}

bool cbMGMakefile::getDependencies(ProjectBuildTarget *p_pTarget,Compiler* p_pCompiler)
{
    wxFileName l_DepsFilename(m_pProj->GetFilename());
    l_DepsFilename.SetExt(_T("depend"));
    /* l_DepsFilename content filename for <project>.depend */
    return reLoadDependecies(l_DepsFilename.GetFullPath(),p_pTarget,p_pCompiler);
}

bool cbMGMakefile::SaveMakefile()
{
    bool l_Ret = false;
    wxString l_FullFilename = m_Path + m_Filename;

    if ( m_Overwrite && wxFileExists( l_FullFilename ) ) wxRemoveFile( l_FullFilename );

    wxTextFile l_File;
    if ( !l_File.Create( l_FullFilename ) )
    {
        wxString lMsg = _( "File " ) + l_FullFilename + _(" is exists!\nOverwrite?" );
        if (wxID_YES == cbMessageBox(lMsg, _("Warning"), wxICON_EXCLAMATION | wxYES_NO, (wxWindow *)Manager::Get()->GetAppWindow()))
        {
            wxRemoveFile( l_FullFilename );
        }
        else
        {
            return l_Ret;
        }
    }
    // for active target only!
    wxString l_ActiveTargetName = m_pProj->GetActiveBuildTarget();
    ProjectBuildTarget* l_pTarget = m_pProj->GetBuildTarget( l_ActiveTargetName );
    if ( !l_pTarget )
    {
        wxString l_Msg = _( "Can't found an active target!\n"
                            "C::B MakefileGen could not complete the operation." );
        cbMessageBox( l_Msg, _( "Error" ), wxICON_ERROR | wxOK, (wxWindow *)Manager::Get()->GetAppWindow() );
        Manager::Get()->GetMessageManager()->DebugLog( l_Msg );
        return l_Ret;
    }

    const wxString& l_CompilerId = l_pTarget->GetCompilerID();
    Compiler* l_pCompiler = CompilerFactory::GetCompiler( l_CompilerId );

    getDependencies( l_pTarget, l_pCompiler );

    m_Variables.AddVariable(_T("CPP"),l_pCompiler->GetPrograms().CPP);
    m_Variables.AddVariable(_T("CC"),l_pCompiler->GetPrograms().C);
    m_Variables.AddVariable(_T("LD"),l_pCompiler->GetPrograms().LD);
    m_Variables.AddVariable(_T("LIB"),l_pCompiler->GetPrograms().LIB);
    m_Variables.AddVariable(_T("WINDRES"),l_pCompiler->GetPrograms().WINDRES);

    const wxArrayString& l_CommandsBeforeBuild = l_pTarget->GetCommandsBeforeBuild();
    const wxArrayString& l_CommandsAfterBuild = l_pTarget->GetCommandsAfterBuild();

    wxString l_TargetFileName = l_pTarget->GetOutputFilename();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(l_TargetFileName, l_pTarget);
    wxFileName l_OutFileName = UnixFilename(l_TargetFileName);
    cbMGRule l_Rule;
    if ( 0 == l_ActiveTargetName.CmpNoCase( _T( "default" ) ) )
    {
        l_Rule.SetTarget( _T( "all" ) );
    }
    else
    {
        l_Rule.SetTarget( l_ActiveTargetName );
    }
    wxString l_Pre;
    if ( l_CommandsBeforeBuild.GetCount() > 0 )
    {
        l_Pre = cmdbefore + _T(" ");
    }
    l_Pre += l_OutFileName.GetFullPath();
    if ( l_CommandsAfterBuild.GetCount() > 0 )
    {
        l_Pre += _T(" ") + cmdafter;
    }

    l_Rule.SetPrerequisites( l_Pre );
    m_Rules.Add( l_Rule );

    if ( l_CommandsBeforeBuild.GetCount() > 0 )
    {
        l_Rule.Clear();
        l_Rule.SetTarget( cmdphony );
        l_Rule.SetPrerequisites( cmdbefore );
        m_Rules.Add( l_Rule );
        l_Rule.Clear();
        l_Rule.SetTarget( cmdbefore );
        for ( unsigned long idx = 0; idx < l_CommandsBeforeBuild.GetCount(); idx ++ )
        {
            l_Rule.AddCommand( l_CommandsBeforeBuild[ idx ] );
        }
        m_Rules.Add( l_Rule );
    }
    if ( l_CommandsAfterBuild.GetCount() > 0 )
    {
        l_Rule.Clear();
        l_Rule.SetTarget( cmdphony );
        l_Rule.SetPrerequisites( cmdafter );
        m_Rules.Add( l_Rule );
        l_Rule.Clear();
        l_Rule.SetTarget( cmdafter );
        for ( unsigned long idx = 0; idx < l_CommandsAfterBuild.GetCount(); idx ++ )
        {
            l_Rule.AddCommand( l_CommandsAfterBuild[ idx ] );
        }
        m_Rules.Add( l_Rule );
    }
    l_Rule.Clear();
    l_Rule.SetTarget( l_OutFileName.GetFullPath() );
    l_Rule.SetPrerequisites( objs );

    wxString kind_of_output = _T( "unknown" );
    CommandType ct = ctInvalid; // get rid of compiler warning
    switch ( l_pTarget->GetTargetType() )
    {
    case ttConsoleOnly:
        ct = ctLinkConsoleExeCmd;
        kind_of_output = _T( "console executable" );
        break;

    case ttExecutable:
        ct = ctLinkExeCmd;
        kind_of_output = _T( "executable" );
        break;

    case ttDynamicLib:
        ct = ctLinkDynamicCmd;
        kind_of_output = _T( "dynamic library" );
        break;

    case ttStaticLib:
        ct = ctLinkStaticCmd;
        kind_of_output = _T( "static library" );
        break;

    case ttNative:
        ct = ctLinkNativeCmd;
        kind_of_output = _T( "native" );
        break;
    case ttCommandsOnly:
        ct = ctLinkConsoleExeCmd;
        kind_of_output = _T( "commands only" );
        break;

//      case ttCommandsOnly:
//          // add target post-build commands
//          ret.Clear();
//          AppendArray(GetPostBuildCommands(target), ret);
//          return ret;
//          break;
        break;
    }
    /*if(ttCommandsOnly == lTarget->GetTargetType())
    {
      GetPostBuildCommands(lTarget);
    }
    else*/
    {
        l_Rule.AddCommand( _T( "echo Building " ) + kind_of_output + _T( " " ) + l_OutFileName.GetFullPath() );
        wxString l_LinkerCmd = l_pCompiler->GetCommand( ct );
        l_pCompiler->GenerateCommandLine( l_LinkerCmd,
                                          l_pTarget,
                                          NULL,
                                          l_OutFileName.GetFullPath(),
                                          oobjs,
                                          wxEmptyString,
                                          wxEmptyString );
        l_Rule.AddCommand( l_LinkerCmd );
    }
    m_Rules.Add( l_Rule );

    // Form rules
    wxString l_Tmp;

    unsigned long ii;

    wxString macro;
    wxString file;
    wxString object;
    wxString FlatObject;
    wxString deps;

    cbMGSortFilesArray files = GetProjectFilesSortedByWeight(l_pTarget,true,false);
    unsigned long lnb_files = files.GetCount();
    for ( ii = 0; ii < lnb_files; ii++ )
    {
        l_Rule.Clear();
        ProjectFile* pf = files[ ii ];
        const pfDetails& pfd = pf->GetFileDetails( l_pTarget );
        wxString l_Object = ( l_pCompiler->GetSwitches().UseFlatObjects )?pfd.object_file_flat:pfd.object_file;
        wxString l_CompilerCmd;
        // lookup file's type
        FileType ft = FileTypeOf( pf->relativeFilename );
        bool isResource = ft == ftResource;
        bool isHeader = ft == ftHeader;
        if ( !isHeader || l_pCompiler->GetSwitches().supportsPCH )
        {
            pfCustomBuild& pcfb = pf->customBuild[l_pCompiler->GetID()];
            l_CompilerCmd = pcfb.useCustomBuildCommand
                            ? pcfb.buildCommand
                            : l_pCompiler->GetCommand( isResource ? ctCompileResourceCmd : ctCompileObjectCmd );
            wxString l_SourceFile;
            if ( l_pCompiler->GetSwitches().UseFullSourcePaths )
            {
                l_SourceFile = UnixFilename( pfd.source_file_absolute_native );
                // for resource files, use short from if path because if windres bug with spaces-in-paths
                if ( isResource )
                {
                    l_SourceFile = pf->file.GetShortPath();
                }
            }
            else
            {
                l_SourceFile = pfd.source_file;
            }
            QuoteStringIfNeeded( l_SourceFile );
            l_pCompiler->GenerateCommandLine( l_CompilerCmd,
                                              l_pTarget,
                                              pf,
                                              l_SourceFile,
                                              l_Object,
                                              pfd.object_file_flat,
                                              pfd.dep_file );
            m_Variables.AppendValue( _T( "OBJS" ), l_Object );
            l_Rule.SetTarget( l_Object );
            l_Rule.SetPrerequisites( l_SourceFile );
            l_Rule.AddCommand( _T( "echo Compiling: " ) + l_SourceFile );
            l_Rule.AddCommand( l_CompilerCmd );
            m_Rules.Add( l_Rule );
        }
    }
    for ( unsigned long i=0; i < m_Deps.Count(); i++ )
    {
        l_Rule.Clear();
        l_Rule.SetTarget( m_Deps.GetVarName( i ) );
        l_Rule.SetPrerequisites( m_Deps.GetVariable( i ) );
        m_Rules.Add( l_Rule );
    }

    l_Rule.Clear();
    l_Rule.SetTarget( cmdphony );
    l_Rule.SetPrerequisites( cmdclean );
    m_Rules.Add( l_Rule );
    l_Rule.Clear();
    l_Rule.SetTarget( cmdclean );
    l_Rule.AddCommand( _T( "echo Delete $(OBJS) " ) + l_OutFileName.GetFullPath() );
#ifdef __WXMSW__
    l_Rule.AddCommand( _T( "del /f $(OBJS) " ) + l_OutFileName.GetFullPath() );
#else
    l_Rule.AddCommand( _T( "rm -f $(OBJS) " ) + l_OutFileName.GetFullPath() );
#endif
    m_Rules.Add( l_Rule );

    // save header
    l_File.AddLine( sHeader );
    l_File.AddLine( wxEmptyString );
    m_Variables.SaveAllVars( l_File );
    SaveAllRules( l_File );

    l_File.Write();
    l_File.Close();
    l_Ret = true;
    return l_Ret;
}

bool cbMGMakefile::SaveAllRules( wxTextFile& pFile )
{
    bool lRet = false;

    size_t num = m_Rules.GetCount();
    unsigned long i;
    unsigned long k;

    for ( i = 0; i < num; i++ )
    {
        cbMGRule& lRule = m_Rules.Item( i );
        pFile.AddLine( lRule.GetTarget() + _T( ": " ) + lRule.GetPrerequisites() );
        for ( k = 0; k < lRule.GetCommands().GetCount(); k++ )
        {
            if ( m_IsSilence )
            {
                pFile.AddLine( GetCommandPrefix() + _T( "@" ) + lRule.GetCommands()[ k ] );
            }
            else
            {
                pFile.AddLine( GetCommandPrefix() + lRule.GetCommands()[ k ] );
            }
        }
        pFile.AddLine( wxEmptyString );
    }
    lRet = true;
    return lRet;
}

