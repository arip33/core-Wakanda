/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
#include "headers4d.h"
#include "VSolution.h"
#include "VProject.h"
#include "VProjectItem.h"
#include "VProjectItemBehaviour.h"
#include "VSolutionStartupParameters.h"
#include "VSourceControl.h"
#include "BreakPointsManager.h"
#include "VRIAUTIs.h"
#include "VRIASettingsKeys.h"
#include "VRIASettingsFile.h"
#include "VProjectSettings.h"
#include "VRIAServerConstants.h"

#if !RIA_SERVER
#include "VRIAStudioApplication.h"
#else
#include "VRIAServerApplication.h"
#include "VRIAServerTools.h"
#endif


USING_TOOLBOX_NAMESPACE


// ----------------------------------------------------------------------------
// Elements XML :
// ----------------------------------------------------------------------------
// Objet : Guidelines pour les fichiers XML du RIA Studio et RIA Server (d'après mail G.A)
// - tout en américain 
// - tout en camelCase (pour le camelCase on retiendra la variante consistant à mettre 
//   la première lettre en minuscule)
// - pas de '_'
// - pas d'acronyme ni d’abréviation ou uniquement les plus usuels (ok pour 'id', 
//   mais 'source' au lieu de 'src')
// - ne pas utiliser de nombre pour définir une valeur d’attribut mais les intitulés 
//   eux-même (ne pas utiliser '0', '1', '2'  mais 'bold', ' italic', ' underline' par exemple)
// - utiliser des noms et non des verbes ( 'NotModifiable' et non 'CannotModify')
// - éviter la forme négative ('enterable' et non 'CannotModify')
// - préférer des noms court pour un xml plus compact
// - préférer les attributs aux élements pour les données scalaires, pour un xml plus compact
// - choisir les valeurs par défaut les plus courantes et ne pas les produire dans le xml, 
//   pour un xml plus compact

const VString kXML_ELEMENT_UNDEFINED				= CVSTR("undefined");	// ne devrait pas arrive
const VString kXML_ELEMENT_SOLUTION					= CVSTR("solution");
const VString kXML_ELEMENT_SOLUTION_LINK			= CVSTR("solutionLink");
const VString kXML_ELEMENT_PROJECT					= CVSTR("project");
const VString kXML_ELEMENT_FILE						= CVSTR("file");
const VString kXML_ELEMENT_FOLDER					= CVSTR("folder");
const VString kXML_ELEMENT_SOLUTION_DESCRIPTION_FILE	= CVSTR("solutionDescriptionFile");
const VString kXML_ELEMENT_PROJECT_DESCRIPTION_FILE	= CVSTR("projectDescriptionFile");
const VString kXML_ELEMENT_ENTITY_MODEL				= CVSTR("entityModel");
const VString kXML_ELEMENT_TAG						= CVSTR("tag");
const VString kXML_ELEMENT_CONFIGURATION			= CVSTR("configuration");
const VString kXML_ELEMENT_SOLUTION_EXPLORER		= CVSTR("solutionExplorer");
const VString kXML_ELEMENT_LOCATOR					= CVSTR("locator");
const VString kXML_ELEMENT_EDITOR					= CVSTR("editor");
const VString kXML_ELEMENT_SOURCE_CONTROL			= CVSTR("sourceControl");
const VString kXML_ELEMENT_LAST_WINDOW_POS			= CVSTR("lastWindowPos");
const VString kXML_ELEMENT_TABS						= CVSTR("tabs");
const VString kXML_ELEMENT_PERMISSIONS				= CVSTR("permissions");
const VString kXML_ELEMENT_DIRECTORY				= CVSTR("directory");
const VString kXML_ELEMENT_GROUP					= CVSTR("group");
const VString kXML_ELEMENT_ID						= CVSTR("ID");
const VString kXML_ELEMENT_NAME						= CVSTR("name");

// ----------------------------------------------------------------------------
// Items de menus :
// ----------------------------------------------------------------------------
// les noms doivent correspondre a ceux utilises dans les Dial4D correspondants
const VString kPROJECT_ITEM_KIND				= CVSTR("ProjectItemKind");
const VString kPROJECT_ITEM_FULL_PATH			= CVSTR("ProjectItemFullPath");

// Menu contextuel Hierarchical List
const VString kITEM_MNU_CLOSE_SOLUTION			= CVSTR("close_solution");
const VString kITEM_MNU_EDIT_PROJECT_SETTINGS	= CVSTR("edit_project_settings");
const VString kITEM_MNU_EDIT_BUILD_SETTINGS		= CVSTR("edit_build_settings");
const VString kITEM_MNU_EDIT_DEPLOYEMENT_SETTINGS	= CVSTR("edit_deployement_settings");
const VString kITEM_MNU_SELECT_SOURCE_CONTROL	= CVSTR("select_source_control");

// ---------------------------------
// Menu contextual List Hierarchique
// ---------------------------------
const VString kITEM_MNU_SET_AS_STARTUP_PROJECT		= CVSTR("set_as_startup_project");
const VString kITEM_MNU_ADD_NEW_PROJECT				= CVSTR("add_new_project");
const VString kITEM_MNU_ADD_NEW_FILE				= CVSTR("add_new_file");
const VString kITEM_MNU_ADD_NEW_JAVASCRIPT_FILE		= CVSTR("add_new_javascript_file");
const VString kITEM_MNU_ADD_NEW_JSON_FILE			= CVSTR("add_new_json_file");
const VString kITEM_MNU_ADD_NEW_HTML_FILE			= CVSTR("add_new_html_file");
const VString kITEM_MNU_ADD_NEW_FORM_FILE			= CVSTR("add_new_form_file");
const VString kITEM_MNU_ADD_NEW_XML_FILE			= CVSTR("add_new_xml_file");
const VString kITEM_MNU_ADD_NEW_CSS_FILE			= CVSTR("add_new_css_file");
const VString kITEM_MNU_ADD_NEW_PHP_FILE			= CVSTR("add_new_php_file");
const VString kITEM_MNU_ADD_NEW_TEXT_FILE			= CVSTR("add_new_text_file");
const VString kITEM_MNU_ADD_NEW_STANDARD_FOLDER		= CVSTR("add_new_standard_folder");
const VString kITEM_MNU_ADD_EXISTING_PROJECT		= CVSTR("add_existing_project");
const VString kITEM_MNU_IMPORT_EXISTING_FOLDER		= CVSTR("import_existing_folder");
const VString kITEM_MNU_IMPORT_EXISTING_FILES		= CVSTR("import_existing_files");
const VString kITEM_MNU_REFERENCE_EXTERNAL_FOLDER	= CVSTR("reference_existing_folder");
const VString kITEM_MNU_REFERENCE_EXTERNAL_FILES	= CVSTR("reference_existing_files");
const VString kITEM_MNU_OPEN_IN_NEW_WINDOW			= CVSTR("open_in_new_window");
const VString kITEM_MNU_OPEN_IN_NEW_TAB				= CVSTR("open_in_new_tab");
const VString kITEM_MNU_SAVE_AS_TEMPLATE            = CVSTR("saveAsTemplate");
const VString kITEM_MNU_SET_AS_ACTIVE				= CVSTR("set_as_active");
const VString kITEM_MNU_TAG_AS_RPC					= CVSTR("tag_as_rpc");
const VString kITEM_MNU_REVEAL_IN_FINDER			= CVSTR("reveal_in_finder");
const VString kITEM_MNU_CUT							= CVSTR("cut");
const VString kITEM_MNU_COPY						= CVSTR("copy");
const VString kITEM_MNU_PASTE						= CVSTR("paste");
const VString kITEM_MNU_DELETE						= CVSTR("delete");
const VString kITEM_MNU_RENAME						= CVSTR("rename");
const VString kITEM_MNU_GET_LATEST_VERSION			= CVSTR("get_latest_version");
const VString kITEM_MNU_CHECK_OUT					= CVSTR("check_out");
const VString kITEM_MNU_ADD_TO_SOURCE_CONTROL		= CVSTR("add_to_source_control");
const VString kITEM_MNU_CHECK_IN					= CVSTR("check_in");
const VString kITEM_MNU_REVERT						= CVSTR("revert");
const VString kITEM_MNU_EXCLUDE_FROM_SOLUTION		= CVSTR("exclude_from_solution");
const VString kITEM_MNU_PROPERTIES					= CVSTR("properties");
const VString kITEM_MNU_SYNCHRONIZE					= CVSTR("refresh");

// ------------------------
// Menu contextual List Box
// ------------------------
const VString kITEM_MNU_LB_OPEN_IN_NEW_WINDOW		= CVSTR("LB_open_in_new_window");
const VString kITEM_MNU_LB_OPEN_IN_NEW_TAB			= CVSTR("LB_open_in_new_tab");
const VString kITEM_MNU_LB_REVEAL_IN_FINDER		= CVSTR("LB_reveal_in_finder");
const VString kITEM_MNU_LB_REVEAL_IN_SOLUTION_EXPLORER	= CVSTR("LB_reveal_in_solution_explorer");
const VString kITEM_MNU_LB_REVEAL_PARENT_IN_SOLUTION_EXPLORER	= CVSTR("LB_reveal_parent_in_solution_explorer");	// utile pour les champs !
const VString kITEM_MNU_LB_EXCLUDE_FROM_SOLUTION	= CVSTR("LB_exclude_from_solution");
const VString kITEM_MNU_LB_CUT						= CVSTR("LB_cut");
const VString kITEM_MNU_LB_COPY						= CVSTR("LB_copy");
const VString kITEM_MNU_LB_DELETE					= CVSTR("LB_delete");
const VString kITEM_MNU_LB_RENAME					= CVSTR("LB_rename");
const VString kITEM_MNU_LB_GET_LATEST_VERSION		= CVSTR("LB_get_latest_version");
const VString kITEM_MNU_LB_CHECK_OUT				= CVSTR("LB_check_out");
const VString kITEM_MNU_LB_ADD_TO_SOURCE_CONTROL	= CVSTR("LB_add_to_source_control");
const VString kITEM_MNU_LB_CHECK_IN					= CVSTR("LB_check_in");
const VString kITEM_MNU_LB_REVERT					= CVSTR("LB_revert");

// --------------------------
// Menu contextual Thumbnails
// --------------------------
const VString kITEM_MNU_TH_OPEN_IN_NEW_WINDOW		= CVSTR("TH_open_in_new_window");
const VString kITEM_MNU_TH_OPEN_IN_NEW_TAB			= CVSTR("TH_open_in_new_tab");
const VString kITEM_MNU_TH_REVEAL_IN_FINDER		= CVSTR("TH_reveal_in_finder");
const VString kITEM_MNU_TH_REVEAL_IN_SOLUTION_EXPLORER	= CVSTR("TH_reveal_in_solution_explorer");
const VString kITEM_MNU_TH_REVEAL_PARENT_IN_SOLUTION_EXPLORER	= CVSTR("TH_reveal_parent_in_solution_explorer");	// utile pour les champs !
const VString kITEM_MNU_TH_EXCLUDE_FROM_SOLUTION	= CVSTR("TH_exclude_from_solution");
const VString kITEM_MNU_TH_CUT						= CVSTR("TH_cut");
const VString kITEM_MNU_TH_COPY						= CVSTR("TH_copy");
const VString kITEM_MNU_TH_DELETE					= CVSTR("TH_delete");
const VString kITEM_MNU_TH_RENAME					= CVSTR("TH_rename");
const VString kITEM_MNU_TH_GET_LATEST_VERSION		= CVSTR("TH_get_latest_version");
const VString kITEM_MNU_TH_CHECK_OUT				= CVSTR("TH_check_out");
const VString kITEM_MNU_TH_ADD_TO_SOURCE_CONTROL	= CVSTR("TH_add_to_source_control");
const VString kITEM_MNU_TH_CHECK_IN					= CVSTR("TH_check_in");
const VString kITEM_MNU_TH_REVERT					= CVSTR("TH_revert");

// -----------------------------------
// Menu contextual Tool Bar principale
// -----------------------------------
const VString kITEM_MNU_TB_OPEN_FILE				= CVSTR("open_NEW");
const VString kITEM_MNU_TB_IMPORT_FILES				= CVSTR("open_ADD");

// -----------------------------------
// Tool Bar Solution Explorer
// -----------------------------------
const VString kITEM_BTN_HOME						= CVSTR("home");
const VString kITEM_BTN_PARENT						= CVSTR("parent");
const VString kITEM_BTN_PREVIOUS					= CVSTR("previous");
const VString kITEM_BTN_NEXT						= CVSTR("next");

// ----------------------------------------------------------------------------
// Fichiers :
// ----------------------------------------------------------------------------
const VString kUSER_TEMPLATES_FOLDER				= CVSTR("Templates");
const VString kDEFAULT_PROJECT_TEMPLATE_FOLDER		= CVSTR("Default Project");
const VString kUSER_CONFIGURATION_FOLDER			= CVSTR("Configuration");

const XBOX::VString	kTEMPLATE_PROJECT_NAME_MACRO( "$(projectName)");
const XBOX::VString	kTEMPLATE_SOLUTION_NAME_MACRO( "$(solutionName)");

// ----------------------------------------------------------------------------
// Chaines speciales :
// ----------------------------------------------------------------------------
const char POSIX_FOLDER_SEPARATOR = '/';
const VString kSCC_PREFIX = CVSTR("scc_");

// ----------------------------------------------------------------------------
// Properties :
// ----------------------------------------------------------------------------
const VString kPROJECT_ITEM_FILE_SIZE					= CVSTR("file_size");
const VString kPROJECT_ITEM_FILE_TYPE			 		= CVSTR("file_type");
const VString kPROJECT_ITEM_LAST_MODIFICATION			= CVSTR("file_last_modification");
// not used const VString kPROJECT_ITEM_CREATION_TIME				= CVSTR("file_creation_time");
// not used const VString kPROJECT_ITEM_LAST_ACCESS					= CVSTR("file_last_access");



const XBOX::ScrapKind kSOLUTION_UUID_SCRAP_KIND( "com.4d.wakanda.studio.solution-uuid");
const XBOX::ScrapKind kCUT_SESSION_UUID_SCRAP_KIND( "com.4d.wakanda.studio.cut-session-uuid");


namespace SolutionPermissions
{
	CREATE_BAGKEY_WITH_DEFAULT(permissions, XBOX::VString, "");
	CREATE_BAGKEY_WITH_DEFAULT(allow, XBOX::VString, "");
	CREATE_BAGKEY_WITH_DEFAULT(action, XBOX::VString, "");
	CREATE_BAGKEY_WITH_DEFAULT(groupID, XBOX::VString, "");
	CREATE_BAGKEY_WITH_DEFAULT(resource, XBOX::VString, "");
	CREATE_BAGKEY_WITH_DEFAULT(temporaryForcePermissions, XBOX::VString, "");
	CREATE_BAGKEY_WITH_DEFAULT(type, XBOX::VString, "");
}

// ----------------------------------------------------------------------------



class VSolutionFileStampSaver : public VObject
{
public:
	VSolutionFileStampSaver( VSolution *inSolution)	: fSolution(inSolution)
	{
		fDirtyStampSave = fSolution->_GetSolutionFileDirtyStamp();
	}

	virtual ~VSolutionFileStampSaver() {;}

			bool StampHasBeenChanged() const
			{
				return fSolution->_GetSolutionFileDirtyStamp() > fDirtyStampSave;
			}

private:
			VSolution	*fSolution;
			sLONG		fDirtyStampSave;
};



// ----------------------------------------------------------------------------
// Classe VSolution
// ----------------------------------------------------------------------------
VSolution::VSolution()
: fSolutionItem(NULL)
,fSolutionFileProjectItem( NULL)
,fSolutionFileDirtyStamp(0)
,fDelegate(NULL)
,fSuccessfulLoading(true)
,fAutoSave(false)
,fAcceptReloadCatalog(false)
,fSolutionMessageManager(NULL)
,fSolutionStartupParameters(NULL)
,fSolutionUser(NULL)
,fPermissions(NULL)
,fPermissionsHaveBeenModified(false)
,fBreakPointsManager(NULL)
,fParsingManager(NULL)
,fStartupProject(NULL)
{
	fUUID.Regenerate();
}


VSolution::~VSolution()
{
	xbox_assert(fParsingManager == NULL);

	ReleaseRefCountable( &fSolutionUser);

	ReleaseRefCountable( &fSolutionMessageManager);

	ReleaseRefCountable( &fSolutionStartupParameters);

	if (fSolutionItem != NULL)
	{
		fSolutionItem->DeleteChildren();
		delete fSolutionItem;
		fSolutionItem = NULL;
	}

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->Release();
	}
	
	ReleaseRefCountable( &fBreakPointsManager);
	ReleaseRefCountable( &fParsingManager);
	ReleaseRefCountable( &fPermissions);
}


VSolution* VSolution::Instantiate( VError& outError, const XBOX::VFilePath& inSolutionFile)
{
	VFilePath folderPath;
	if (!inSolutionFile.IsFile() || !inSolutionFile.GetFolder( folderPath))
	{
		outError = VE_INVALID_PARAMETER;
		return NULL;
	}

	VProjectItem *rootItem = NULL, *solutionFileItem = NULL;
	outError = VE_OK;

	// Create the root item of the solution
	rootItem = new VProjectItem( VURL( folderPath), VProjectItem::eSOLUTION);
	if (rootItem != NULL)
	{
		VString itemName;
		inSolutionFile.GetFileName( itemName, false);
		rootItem->SetName( itemName);
		rootItem->SetDisplayName( itemName);

		// Create an item for the solution file
		solutionFileItem = new VProjectItem( VURL( inSolutionFile), VProjectItem::eFILE);
		if (solutionFileItem != NULL)
		{
			inSolutionFile.GetFileName( itemName, true);
			solutionFileItem->SetName( itemName);
			solutionFileItem->SetDisplayName( itemName);
		}
		else
		{
			outError = VE_MEMORY_FULL;
		}
	}
	else
	{
		outError = VE_MEMORY_FULL;
	}

	VSolution *solution = NULL;

	if (outError == VE_OK)
	{
		solution = new VSolution();
		if (solution != NULL)
		{
			solution->_SetSolutionItem( rootItem);
			solution->_SetSolutionFileItem( solutionFileItem);
		}
		else
		{
			outError = VE_MEMORY_FULL;
		}
	}

	ReleaseRefCountable( &rootItem);
	ReleaseRefCountable( &solutionFileItem);

	return solution;
}


void VSolution::SetDelegate( ISolutionDelegate *inDelegate)
{
	fDelegate = inDelegate;
}


ISolutionDelegate* VSolution::GetDelegate() const
{
	return fDelegate;
}


XBOX::VError VSolution::CreateFromTemplate( const XBOX::VFolder& inSolutionFolder, const XBOX::VString& inSolutionName, const XBOX::VFolder& inSolutionTemplateFolder)
{
	VError err = VE_OK;

	if (!inSolutionTemplateFolder.Exists())
		err = VE_FOLDER_NOT_FOUND;

	if (err == VE_OK)
	{
		VFilePath sourceFilePath( inSolutionTemplateFolder.GetPath());
		sourceFilePath.SetFileName( kTEMPLATE_SOLUTION_NAME_MACRO, false);
		sourceFilePath.SetExtension( RIAFileKind::kSolutionFileExtension);

		VFile templateSolutionFile( sourceFilePath);
		if (!templateSolutionFile.Exists())
			err = VE_SOMA_SOLUTION_FILE_NOT_FOUND;

		if ((err == VE_OK) && !inSolutionFolder.Exists())
			err = inSolutionFolder.Create();

		if (err == VE_OK)
		{
			// Copy the template folder
			VFolder solutionFolder( inSolutionFolder);
			err = CopyFolder( inSolutionTemplateFolder, solutionFolder);
		}
		
		if (err == VE_OK)
		{
			// Resolve the files and folders names
			err = _ResolveMacro( inSolutionFolder, kTEMPLATE_SOLUTION_NAME_MACRO, inSolutionName);
		}

		if (err == VE_OK)
		{
			// Create the solution file
			VFilePath solutionFilePath( inSolutionFolder.GetPath());
			solutionFilePath.SetFileName( inSolutionName);
			solutionFilePath.SetExtension( RIAFileKind::kSolutionFileExtension);

			VFile solutionFile( solutionFilePath);
			if (!solutionFile.Exists())
				err = VE_SOMA_SOLUTION_FILE_NOT_FOUND;

			if (err == VE_OK)
			{
				// Resolve the items names which are referenced inside the solution file
				VValueBag solutionBag;
				err = LoadBagFromXML( solutionFile, kXML_ELEMENT_SOLUTION, solutionBag, XML_ValidateNever);
				if (err == VE_OK)
				{
					sLONG changesCount = 0;
					VBagArray *foldersBagArray = solutionBag.RetainElements( kXML_ELEMENT_FOLDER);
					if (foldersBagArray != NULL)
					{
						for (sLONG i = 1 ; i <= foldersBagArray->GetCount() ; i++)
						{
							VValueBag *folderBag = foldersBagArray->GetNth( i);
							if (folderBag != NULL)
							{
								VString relativePath = ProjectItemBagKeys::path.Get( folderBag);
								if (relativePath.Find( kTEMPLATE_SOLUTION_NAME_MACRO) != 0)
								{
									relativePath.ExchangeAll( kTEMPLATE_SOLUTION_NAME_MACRO, inSolutionName);
									ProjectItemBagKeys::path.Set( folderBag, relativePath);
									++changesCount;
								}
							}
						}

						solutionBag.SetElements( kXML_ELEMENT_FOLDER, foldersBagArray);
					}

					ReleaseRefCountable( &foldersBagArray);

					VBagArray *filesBagArray = solutionBag.RetainElements( kXML_ELEMENT_FILE);
					if (filesBagArray != NULL)
					{
						for (sLONG i = 1 ; i <= filesBagArray->GetCount() ; i++)
						{
							VValueBag *fileBag = filesBagArray->GetNth( i);
							if (fileBag != NULL)
							{
								VString relativePath = ProjectItemBagKeys::path.Get( fileBag);
								if (relativePath.Find( kTEMPLATE_SOLUTION_NAME_MACRO) != 0)
								{
									relativePath.ExchangeAll( kTEMPLATE_SOLUTION_NAME_MACRO, inSolutionName);
									ProjectItemBagKeys::path.Set( fileBag, relativePath);
									++changesCount;
								}
							}
						}

						solutionBag.SetElements( kXML_ELEMENT_FILE, filesBagArray);
					}

					ReleaseRefCountable( &filesBagArray);

					if (changesCount > 0)
					{
						err = WriteBagToFileInXML( solutionBag, kXML_ELEMENT_SOLUTION, &solutionFile);
					}
				}
			}
		}
	}

	return err;
}


const VUUID& VSolution::GetUUID() const
{
	return fUUID;
}


bool VSolution::GetName( XBOX::VString& outName) const
{
	bool ok = false;
	outName.Clear();

	if (fSolutionItem != NULL)
	{
		outName = fSolutionItem->GetDisplayName();
		ok = true;
	}
	return ok;
}


bool VSolution::GetSolutionFolderPath( XBOX::VFilePath& outPath) const
{
	bool ok = false;
	outPath.Clear();

	VFilePath		solutionPath;
	VProjectItem*	solutionProjectItem = GetSolutionFileProjectItem();
	xbox_assert ( solutionProjectItem != NULL );
	if ( solutionProjectItem != NULL )
	{
		ok = solutionProjectItem->GetFilePath( solutionPath );
		xbox_assert ( ok );
		ok = solutionPath.GetParent( outPath );
		xbox_assert ( ok );
	}

	return ok;
}


bool VSolution::GetSolutionFilePath( XBOX::VFilePath& outPath) const
{
	bool ok = false;
	outPath.Clear();

	if (fSolutionFileProjectItem != NULL)
		ok = fSolutionFileProjectItem->GetFilePath( outPath);

	return ok;
}


XBOX::VError VSolution::Rename( const XBOX::VString& inNewName)
{
	VError err = VE_OK;

	VString name;
	if (GetName( name) && (name != inNewName))
	{
		name = inNewName + L"." + RIAFileKind::kSolutionFileExtension;

		err = fSolutionFileProjectItem->RenameContent( name);
		if (err == VE_OK)
		{
			fSolutionItem->SetDisplayName( inNewName);
			fSolutionItem->Touch();
			
			// sc 29/09/2011 update the startup parameters
			if (fSolutionStartupParameters != NULL)
			{
				VFilePath path;
				fSolutionFileProjectItem->GetFilePath( path);
				VFile *solutionFile = new VFile( path);
				if (solutionFile != NULL)
				{
					fSolutionStartupParameters->SetSolutionFileToOpen( solutionFile);
					ReleaseRefCountable( &solutionFile);
				}
			}

			UpdateSolutionLinkFile();
		}
	}

	if (err != VE_OK)
		err = VE_SOMA_CANNOT_RENAME_SOLUTION;

	return err;
}


void VSolution::BuildPathFromRelativePath( XBOX::VFilePath& outFilePath, const XBOX::VString& inRelativePath, XBOX::FilePathStyle inRelativePathStyle) const
{
	outFilePath.Clear();

	if (fSolutionItem != NULL)
	{
		VFilePath folderPath;
		fSolutionItem->GetFilePath( folderPath);
		folderPath = folderPath.ToFolder();
		outFilePath.FromRelativePath(folderPath, inRelativePath, inRelativePathStyle);
	}	
}


void VSolution::DoProjectItemsChanged( const VectorOfFilePathes& inChangedPathes)
{
	if (fDelegate != NULL)
		fDelegate->DoProjectItemsChanged( inChangedPathes);
}


void VSolution::SetSolutionMessageManager( ISolutionMessageManager* inSolutionMessageManager)
{
	CopyRefCountable( &fSolutionMessageManager, inSolutionMessageManager);
}


void VSolution::SetSolutionUser( ISolutionUser* inSolutionUser)
{
	CopyRefCountable( &fSolutionUser, inSolutionUser);
	if (fSolutionUser)
		fSolutionUser->SetSolution( this);
}


void VSolution::DoStartParsingFiles( sLONG inExpectedCount)
{
	if (fDelegate != NULL)
		fDelegate->StartParsingFiles( inExpectedCount);
}


void VSolution::DoStopParsingFiles()
{
	if (fDelegate != NULL)
		fDelegate->StopParsingFiles();
}


void VSolution::DoParsedOneFile()
{
	if (fDelegate != NULL)
		fDelegate->ParsedOneFile();
}


void VSolution::SetBreakPointsManager( ISolutionBreakPointsManager* inBreakPointsManager)
{
	CopyRefCountable( &fBreakPointsManager, inBreakPointsManager);
	if (fBreakPointsManager != NULL)
	{
		fBreakPointsManager->SetSolution( this);
		fBreakPointsManager->InitSolutionFolderPosixPath();
	}
}


VProject* VSolution::GetProjectFromFilePathOfProjectFile(const XBOX::VFilePath& inFilePath) const
{
	VProject* project = NULL;
	if (!inFilePath.IsEmpty())
	{	
		VString strPathOfProjectFile;
		inFilePath.GetPath(strPathOfProjectFile);
		for (VectorOfProjectsConstIterator it = fProjects.begin(); it != fProjects.end(); ++it)
		{
			VFilePath filePath;
			(*it)->GetProjectItemProjectFile()->GetFilePath(filePath);
			VString strFilePath;
			filePath.GetPath(strFilePath);
			if (strFilePath == strPathOfProjectFile)
			{
				project = *it;
				break;
			}
		}
	}
	return project;
}


// ICI ON SAIT que la reponse est unique
VProjectItem* VSolution::GetProjectItemFromFullPath(const XBOX::VString& inFullPath)
{
	VProjectItem* projectItem = NULL;

	if (!inFullPath.IsEmpty())
	{
		// d'abord chercher l'occurrence physique de l'item : celle-ci est forcement unique
		VString currentItemFullPath;
		fSolutionItem->BuildFullPath(currentItemFullPath);

		if (inFullPath == currentItemFullPath) // au cas ou ce serait le path de la solution...
		{
			projectItem = fSolutionItem;
		}
		else
		{
			for (VProjectItemIterator it(fSolutionItem); it.IsValid(); ++it)
			{
				it->BuildFullPath(currentItemFullPath);

				if (it->GetKind() == VProjectItem::ePROJECT)
				{
					if (inFullPath.BeginsWith(currentItemFullPath, false))
					{
						if (inFullPath == currentItemFullPath)
						{
							projectItem = it;
							break;
						}
						else
						{
							VProject* project = it->GetProjectOwner();
							if (NULL != project)
								projectItem = project->GetProjectItemFromFullPath(inFullPath);
							if (projectItem != NULL)
								break;
						}
					}
				}
				else
				{
					if (currentItemFullPath == inFullPath)
					{
						projectItem = it;
						break;
					}
				}
			}
		}
	}

	return projectItem;
}


void VSolution::GetVectorOfProjectNames( std::vector<XBOX::VString>& outProjectNames) const
{
	outProjectNames.clear();

	for (VectorOfProjectsConstIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		VString name;
		(*iter)->GetName( name);
		outProjectNames.push_back( name);
	}
}

void VSolution::GetVectorOfProjects( VectorOfProjects& outProjects) const
{
	outProjects.clear();
	outProjects.insert( outProjects.begin(), fProjects.begin(), fProjects.end());
} 

VProject* VSolution::FindProjectByName( const XBOX::VString& inProjectName) const
{
	VProject *project = NULL;

	for (VectorOfProjectsConstIterator iter = fProjects.begin() ; iter != fProjects.end() && project == NULL ; ++iter)
	{
		VString name;
		(*iter)->GetName( name);
		if (name == inProjectName)
			project = *iter;
	}

	return project;
}


void VSolution::GetPhysicalFilesOfCurrentProject(VectorOfProjectItems& outProjectItemsVector)
{
	VProject* project = NULL;

	if (fDelegate != NULL)
		project = fDelegate->GetSelectedProject();

	if (project != NULL)
	{
		VProjectItemTools::GetChildFile( project->GetProjectItem(), outProjectItemsVector, true);
	}

	// 071009 : un jour, ajouter les references externes (si elles reviennent dans les specs)
}

VError VSolution::SaveProjectAsTemplate(const VFilePath& inProjectFolderPath, const VString& inProjectFileName, VString& inTemplateName, VString& inTypeReadMe, VString& inReadMe, VString& inPathImage, VProgressIndicator* inProgressIndicator)
{
	VError err = VE_OK;
	
	XBOX::VFolder *folder = XBOX::VFolder::RetainSystemFolder( eFK_UserPreferences, false);
	
	VString strFolderPath;
	folder->GetPath(strFolderPath);
	strFolderPath += "Wakanda";
	strFolderPath += FOLDER_SEPARATOR;
	strFolderPath += "UserTemplates";
	strFolderPath += FOLDER_SEPARATOR;
	VFolder tmpFolder(strFolderPath);
	if (!tmpFolder.Exists())
		tmpFolder.CreateRecursive();
	
	strFolderPath += inTemplateName;
	strFolderPath += FOLDER_SEPARATOR;
	VFilePath folderFilePath(strFolderPath);
	VFolder destFolder(strFolderPath);
	if (!destFolder.Exists())
	{
		bool userAbort = false;
		// on se base seulement sur le nombre de fichiers pour le progressIndicator
		sLONG allFilesCount = VProjectItemManager::GetFilesCountInFolderAndSubfolder(VFolder(inProjectFolderPath));
		if (inProgressIndicator)
			// TO DO a localiser !!!
			inProgressIndicator->BeginSession(allFilesCount, CVSTR("Copying files..."), true);

		if (!VProjectItemManager::Get()->FolderCopyTo(inProjectFolderPath, folderFilePath, inProgressIndicator, &userAbort))
			err = VE_UNKNOWN_ERROR;
		else
		{
			if (userAbort)
			{
				err = VE_UNKNOWN_ERROR;
			}
			else
			{
				VString strProjectFilePath = strFolderPath;
				strProjectFilePath += inProjectFileName;
				VFile projectFile(strProjectFilePath);
				if (projectFile.Exists())
				{
					VString newFileProjectName = inTemplateName;
					newFileProjectName += ".";
					newFileProjectName += RIAFileKind::kProjectFileExtension;
					projectFile.Rename(newFileProjectName);
				}

				if (inTypeReadMe == "path")
				{
					VFilePath srcReadMePath(inReadMe);
					VString extension;
					srcReadMePath.GetExtension(extension);
					VString strDestReadMePath = strFolderPath;
					strDestReadMePath += "ReadMe";
					strDestReadMePath += ".";
					strDestReadMePath += extension;
					VFilePath destFilePath(strDestReadMePath);
					if (!VProjectItemManager::Get()->FileCopyTo(srcReadMePath, destFilePath))
						err = VE_UNKNOWN_ERROR;
					else
					{
						if ((extension == "htm") || (extension == "html"))
						{
							// complement demande par Rodolphe le 180310
							VString strTxtReadMePath = strFolderPath;
							strTxtReadMePath += "ReadMe.txt";
							VFile txtReadMeFile(strTxtReadMePath);
							if (txtReadMeFile.Exists())
								txtReadMeFile.Delete();
						}
					}
				}
				else // type == text
				{
					VString strDestReadMePath = strFolderPath;
					strDestReadMePath += "ReadMe.txt";
					VFile readMeFile(strDestReadMePath);
					VFileDesc *fd = NULL;
					err = readMeFile.Open(FA_READ_WRITE, &fd, FO_CreateIfNotFound | FO_Overwrite);
					if (err == VE_OK && fd != NULL)
					{
						VStringConvertBuffer buffer(inReadMe, VTC_US_ASCII);
						err = fd->SetSize(buffer.GetSize());
						if (err == VE_OK)
						{
							err = fd->PutDataAtPos(buffer.GetCPointer(), buffer.GetSize());
							if (err == VE_OK)
								err = fd->Flush();
						}
						delete fd;
						// complement demande par Rodolphe le 180310
						VString strHTMLReadMePath = strFolderPath;
						strHTMLReadMePath += "ReadMe.html";
						VFile HTMLReadMeFile(strHTMLReadMePath);
						if (HTMLReadMeFile.Exists())
							HTMLReadMeFile.Delete();
						VString strHTMReadMePath = strFolderPath;
						strHTMReadMePath += "ReadMe.htm";
						VFile HTMReadMeFile(strHTMReadMePath);
						if (HTMReadMeFile.Exists())
							HTMReadMeFile.Delete();
					}
					else
					{
						err = VE_FILE_CANNOT_OPEN;
					}
				}
				
				if ((err == VE_OK) && (!inPathImage.IsEmpty()))
				{
					VFilePath srcImagePath(inPathImage);
					VString extension;
					srcImagePath.GetExtension(extension);
					VString strDestImagePath = strFolderPath;
					strDestImagePath += inTemplateName;
					strDestImagePath += ".";
					strDestImagePath += extension;
					VFilePath destFilePath(strDestImagePath);
					if (!VProjectItemManager::Get()->FileCopyTo(srcImagePath, destFilePath))
						err = VE_UNKNOWN_ERROR;
				}
			}
		}
	}
	else
	{
		err = VE_FOLDER_ALREADY_EXISTS;
		if (fSolutionMessageManager)
		{
			VString localizedMessage;
			fSolutionMessageManager->GetLocalizedStringFromResourceName(kSOMA_TEMPLATE_ALREADY_EXISTS, localizedMessage);
			fSolutionMessageManager->DisplayMessage(localizedMessage);
		}
	}

	return err;
}

VProjectItem* VSolution::GetProjectItemFromTag( const VProjectItemTag& inTag ) const
{
	VectorOfProjectItems itemsVector;
	GetProjectItemsFromTag( inTag, itemsVector);
	return (!itemsVector.empty()) ? itemsVector[0] : NULL;
}

void VSolution::GetProjectItemsFromTag( const VProjectItemTag& inTag, VectorOfProjectItems& outProjectItems) const
{
	outProjectItems.clear();

	if (!VProjectItemTagManager::Get()->IsRegisteredProjectItemTag( inTag))
		return;

	bool done = false;

	EProjectItemTagProperties projectItemTagProperties = VProjectItemTagManager::Get()->GetProperties( inTag);
	bool singleProjectItem = ((projectItemTagProperties & ePITP_ApplyToSingleFile) != 0) || ((projectItemTagProperties & ePITP_ApplyToSingleFolder) != 0)  || (inTag == kSettingTag);
	bool applyToFiles = ((projectItemTagProperties & ePITP_ApplyToSingleFile) != 0) || ((projectItemTagProperties & ePITP_ApplyToMultipleFiles) != 0);
	bool applyToFolders = ((projectItemTagProperties & ePITP_ApplyToSingleFolder) != 0) || ((projectItemTagProperties & ePITP_ApplyToMultipleFolders) != 0);
	bool applyToFolderContent = (projectItemTagProperties & ePITP_ApplyToFolderContent) != 0;

	// First, search through the referenced items.
	for (VectorOfProjectItemsConstIterator iter = fReferencedItems.begin() ; iter != fReferencedItems.end() && !done ; ++iter)
	{
		VProjectItem *item = *iter;
		if (item->HasTag( inTag))
		{
			if (item->IsPhysicalFile())
			{
				if (std::find( outProjectItems.begin(), outProjectItems.end(), item) == outProjectItems.end())
				{
					outProjectItems.push_back( item);
					if (singleProjectItem)
						done = true;
				}
			}
			else if(item->IsPhysicalFolder())
			{
				if (applyToFolderContent)
				{
					// TBD
					xbox_assert(false);
				}
				else
				{
					if (std::find( outProjectItems.begin(), outProjectItems.end(), item) == outProjectItems.end())
					{
						outProjectItems.push_back( item);
						if (singleProjectItem)
							done = true;
					}
				}
			}
		}
	}

	if (!done && (inTag == kSettingTag))
	{
		// Particular case for settings file: search into the solution folder
		VFileKind *fileKind = VProjectItemTagManager::Get()->RetainDefaultFileKind( inTag);

		for (VProjectItemIterator iter(fSolutionItem) ; iter.IsValid() && !done ; ++iter)
		{
			if (iter->IsPhysicalFile())
			{
				VFilePath path;
				if (iter->GetFilePath( path))
				{
					VFile file (path);
					if (file.ConformsTo( *fileKind))
					{
						if (std::find( outProjectItems.begin(), outProjectItems.end(), (VProjectItem*)iter) == outProjectItems.end())
						{
							outProjectItems.push_back( iter);
							if (singleProjectItem)
								done = true;
						}
					}
				}
			}
		}

		ReleaseRefCountable( &fileKind);
	}
}


void VSolution::_ReferenceItem( VProjectItem *inItem, bool inTouchSolutionFile)
{
	if (!_IsItemReferenced( inItem))
	{
		fReferencedItems.push_back( inItem);
		if (inTouchSolutionFile)
			_TouchSolutionFile();
	}
}


void VSolution::_UnreferenceItem( VProjectItem *inItem, bool inTouchSolutionFile)
{
	VectorOfProjectItemsIterator found = std::find(  fReferencedItems.begin(), fReferencedItems.end(), inItem);
	if (found != fReferencedItems.end())
	{
		fReferencedItems.erase( found);
		if (inTouchSolutionFile)
			_TouchSolutionFile();
	}
}


bool VSolution::_IsItemReferenced( VProjectItem *inItem) const
{
	return (std::find( fReferencedItems.begin(), fReferencedItems.end(), inItem) != fReferencedItems.end());
}


bool VSolution::_IsVectorContainsReferencedItems( const VectorOfProjectItems& inProjectItems, bool inRecursive) const
{
	bool result = false;

	for (VectorOfProjectItemsConstIterator iter = inProjectItems.begin() ; (iter != inProjectItems.end()) && !result ; ++iter)
	{
		result = _IsItemReferenced( *iter);
		if (!result && inRecursive)
		{
			VectorOfProjectItems children;
			VProjectItemTools::GetChildren( *iter, children, false);
			result = _IsVectorContainsReferencedItems( children, true);
		}
	}

	return result;
}


XBOX::VError VSolution::CountFilesAndFolders( const VFolder& inFolder, sLONG& outFilesCount, sLONG &outFoldersCount, bool inRecursive)
{
	outFilesCount = 0;
	outFoldersCount = 0;
	return _CountFilesAndFolders( inFolder, outFilesCount, outFoldersCount, inRecursive);

}


VError VSolution::CopyFolder( const XBOX::VFolder& inSourceFolder, XBOX::VFolder& inDestinationFolder, const XBOX::FileCopyOptions inOptions, XBOX::VProgressIndicator *inProgressIndicator, bool *outUserAbort)
{
	VError err = VE_OK;

	VString destinationPath, sourcePath;
	inDestinationFolder.GetPath( destinationPath);
	inSourceFolder.GetPath( sourcePath);
	if (destinationPath.BeginsWith( sourcePath))
	{
		err = VE_UNKNOWN_ERROR;
	}
	else
	{
		if (inProgressIndicator != NULL)
		{
			sLONG filesCount = 0, foldersCount = 0;
			CountFilesAndFolders( inSourceFolder, filesCount, foldersCount, true);
			inProgressIndicator->BeginSession( filesCount, CVSTR("Copying files..."), true);
		}

		err = _CopyFolder( inSourceFolder, inDestinationFolder, inOptions, inProgressIndicator, outUserAbort);

		if (inProgressIndicator != NULL)
			inProgressIndicator->EndSession();
	}

	return err;
}


XBOX::VError VSolution::_CountFilesAndFolders( const VFolder& inFolder, sLONG& outFilesCount, sLONG &outFoldersCount, bool inRecursive)
{
	VError err = VE_OK;

	for (VFileIterator it_file( &inFolder, FI_WANT_FILES | FI_WANT_INVISIBLES) ; it_file.IsValid() && err == VE_OK ; ++it_file)
	{
		++outFilesCount;
	}

	for (VFolderIterator it_folder( &inFolder, FI_WANT_FOLDERS | FI_WANT_INVISIBLES) ; it_folder.IsValid() && err == VE_OK ; ++it_folder)
	{
		++outFoldersCount;
		if (inRecursive)
		{
			_CountFilesAndFolders( *it_folder.Current(), outFilesCount, outFoldersCount, true);
		}
	}

	return err;
}


VError VSolution::_CopyFolder( const XBOX::VFolder& inSourceFolder, XBOX::VFolder& inDestinationFolder, const XBOX::FileCopyOptions inOptions, XBOX::VProgressIndicator *inProgressIndicator, bool *outUserAbort)
{
	VError err = VE_OK;

	if (!inSourceFolder.Exists())
		err = VE_FOLDER_NOT_FOUND;

	if (err == VE_OK && !inDestinationFolder.Exists())
		err = inDestinationFolder.CreateRecursive();

	if (err == VE_OK)
	{
		bool userAbort = false;
		
		for (VFolderIterator it_folder( &inSourceFolder, FI_WANT_FOLDERS | FI_WANT_INVISIBLES) ; it_folder.IsValid() && err == VE_OK && !userAbort ; ++it_folder)
		{
			VFilePath destinationFolderPath( inDestinationFolder.GetPath());
			VString name;
			it_folder.Current()->GetName( name);
			destinationFolderPath.ToSubFolder( name);
			VFolder destinationFolder( destinationFolderPath);

			err = CopyFolder( *it_folder.Current(), destinationFolder, inOptions, inProgressIndicator, &userAbort);
		}

		for (VFileIterator it_file( &inSourceFolder, FI_WANT_FILES | FI_WANT_INVISIBLES) ; it_file.IsValid() && err == VE_OK && !userAbort ; ++it_file)
		{
			VFilePath destinationFilePath( inDestinationFolder.GetPath());
			VString name;
			it_file.Current()->GetName( name);
			destinationFilePath.SetFileName( name);
			VFile destinationFile( destinationFilePath);

			err = it_file.Current()->CopyTo( destinationFile, inOptions);

			if (inProgressIndicator != NULL && err == VE_OK)
				userAbort = inProgressIndicator->Increment();
		}

		if (outUserAbort != NULL)
			*outUserAbort = userAbort;
	}

	return err;
}


XBOX::VError VSolution::_ResolveMacro( const XBOX::VFolder& inFolder, const XBOX::VString& inMacroName, const XBOX::VString& inNewName)
{
	VError err = VE_OK;

	if (!inFolder.Exists())
		err = VE_FOLDER_NOT_FOUND;

	for (VFolderIterator it_folder( &inFolder, FI_WANT_FOLDERS | FI_WANT_INVISIBLES) ; it_folder.IsValid() && err == VE_OK ; ++it_folder)
	{
		err = _ResolveMacro( *it_folder.Current(), inMacroName, inNewName);
		
		if (err == VE_OK)
		{
			VString name;
			it_folder.Current()->GetName( name);
			if (name.Find( inMacroName) != 0)
			{
				name.ExchangeAll( inMacroName, inNewName);
				err = it_folder.Current()->Rename( name, NULL);
			}
		}
	}

	for (VFileIterator it_file( &inFolder, FI_WANT_FILES | FI_WANT_INVISIBLES) ; it_file.IsValid() && err == VE_OK ; ++it_file)
	{
		VString name;
		it_file.Current()->GetName( name);
		if (name.Find( inMacroName) != 0)
		{
			name.ExchangeAll( inMacroName, inNewName);
			err = it_file.Current()->Rename( name);
		}
	}

	return err;	
}


bool VSolution::Open()
{
	// cree les items projets depuis le fichier XML de la solution
	bool fSuccessfulLoading = true;

	VError err = _LoadSolutionFile();

	if ((err != VE_OK) 
	 && (err != VE_SOMA_PROJECT_FILE_NOT_FOUND)
	 && (err != VE_SOMA_SETTINGS_FILE_NOT_FOUND))
		fSuccessfulLoading = false;

	if (fSolutionMessageManager)
		fSolutionMessageManager->CheckErrorStack();

	if (fSuccessfulLoading)
	{
// TEST ONLY
//DisableDebuggerGroup ( );
/*VError	vErrTEST = VE_OK;
RetainUserGroups ( vErrTEST );*/
/*VUUID		vUUID;
vUUID. Regenerate ( );
SetDebuggerGroup ( vUUID );*/
/*VError	vErrTEST = VE_OK;
VValueBag* vbGroups = RetainUserGroups ( vErrTEST );
if ( vbGroups != 0 )
	vbGroups-> Release ( );*/
/*VString	vstrName;
VUUID	vUUID;
VError	vErrTEST = GetDebuggerGroup ( vstrName, vUUID );
VString	vstrID;
vUUID. GetString ( vstrID );*/
	}

	if (fSuccessfulLoading)
	{
		CLanguageSyntaxComponent *languageEngine = VComponentManager::RetainComponentOfType< CLanguageSyntaxComponent >();
		if (languageEngine != NULL)
		{
			fParsingManager = languageEngine->CreateDocumentParserManager();
			ReleaseRefCountable( &languageEngine);
		}

		if (fBreakPointsManager != NULL)
		{
			fBreakPointsManager->Load();
		}
	}
	return fSuccessfulLoading;
}


XBOX::VError VSolution::_LoadPermissions ( bool inCreateIfDoesNotExist )
{
	VProjectItem*			pItemPermissions = GetProjectItemFromTag ( kPermissionsTag );
	if ( pItemPermissions == 0 )
	{
		// Solution's waPerm is not defined. Not a big deal.
		if ( inCreateIfDoesNotExist )
		{
			fPermissions = new VValueBag ( );
			//VValueBag*		vBagPermissions = new VValueBag ( );
			//fPermissions-> AddElement ( SolutionPermissions::permissions, vBagPermissions );
			//vBagPermissions-> Release ( );
		}

		return VE_OK;
	}

	VFilePath				vPermPath;
	bool					bResult = pItemPermissions-> GetFilePath ( vPermPath );
	xbox_assert ( bResult );
	if ( !bResult )
		return VE_SOMA_FAILED_TO_LOAD_SOLUTION_PERMISSIONS;

	VError					vError = VE_OK;

	VFile					vPermFile ( vPermPath );
	if ( !vPermFile. Exists ( ) )
		// Solution permissions are referenced but are not present. Technically, not a big deal but just in case let's assert-false this - solution config might just be broken
		xbox_assert ( false );
	else
	{
		fPermissions = new VValueBag ( );
		xbox_assert ( fPermissions != 0 );
		vError = LoadBagFromXML ( vPermFile, kXML_ELEMENT_PERMISSIONS, *fPermissions, XML_ValidateNever );
	}

	return vError;
}


VError VSolution::SavePermissions ( )
{
	if ( fPermissions == 0 || !fPermissionsHaveBeenModified )
		// Permissions have never been loaded or have not been modified so there were no updates and so there is nothing to save
		return VE_OK;

	VProjectItem*			pItemPermissions = GetProjectItemFromTag ( kPermissionsTag );
	if ( pItemPermissions == 0 )
	{
		// This is the first time permissions will be written
		// Let's create a new project item and give it permissions role
		VString				vstrName;
		fSolutionItem-> GetName ( vstrName, false );
		vstrName. AppendCString ( ".waPerm" );
		pItemPermissions = VProjectItemFile::Instantiate ( vstrName, fSolutionItem );
		xbox_assert ( pItemPermissions != 0 );

		pItemPermissions-> AddTag ( kPermissionsTag );

		_DoItemAdded ( pItemPermissions, true );
	}

	VFilePath				vPermPath;
	bool					bResult = pItemPermissions-> GetFilePath ( vPermPath );
	xbox_assert ( bResult );
	if ( !bResult )
		return VE_SOMA_FAILED_TO_LOAD_SOLUTION_PERMISSIONS;

	VFile					vPermFile ( vPermPath );
	VError					vError = WriteBagToFileInXML ( *fPermissions, kXML_ELEMENT_PERMISSIONS, &vPermFile, false );

	fPermissionsHaveBeenModified = false;

	return vError;
}


VError VSolution::GetDebuggerGroup ( XBOX::VString& outName, XBOX::VUUID& outUUID )
{
	VError					vError = VE_OK;

	if ( fPermissions == 0 )
	{
		vError = _LoadPermissions ( );
		if ( fPermissions == 0 )
			return VE_SOMA_DEBUGGER_GROUP_NOT_DEFINED;
		if ( vError != VE_OK )
			return vError;
	}

	const VValueBag*		vbagDebuggerGroup = fPermissions-> RetainUniqueElementWithAttribute ( SolutionPermissions::allow, SolutionPermissions::action, CVSTR ( "debug" ) );
	if ( vbagDebuggerGroup == 0 )
		return VE_SOMA_DEBUGGER_GROUP_NOT_DEFINED;

	bool					bResult = vbagDebuggerGroup-> GetVUUID ( SolutionPermissions::groupID, outUUID );
	xbox_assert ( bResult );
	vbagDebuggerGroup-> Release ( );

	if ( !bResult )
		return VE_SOMA_DEBUGGER_GROUP_NOT_DEFINED;

	// Let's now lookup group name
	VValueBag*				vbagDirectory = RetainUserGroups ( vError );
	if ( vbagDirectory == 0 )
	{
		if ( vError == VE_OK )
			vError = VE_SOMA_DEBUGGER_GROUP_NOT_DEFINED;
	}
	else
	{
		const VBagArray*	vbarrGroups = vbagDirectory-> RetainElements ( kXML_ELEMENT_GROUP );
		vError = VE_SOMA_DEBUGGER_GROUP_NOT_DEFINED;
		if ( vbarrGroups != 0 )
		{
			for ( VIndex i = 1; i <= vbarrGroups-> GetCount ( ); i++ )
			{
				const VValueBag*		vbGroup = vbarrGroups-> GetNth ( i );
				if ( vbGroup == 0 )
					continue;

				VUUID			vIDCurrent;
				bResult = vbGroup-> GetVUUID ( kXML_ELEMENT_ID, vIDCurrent );
				xbox_assert ( bResult );
				if ( outUUID == vIDCurrent )
				{
					bResult = vbGroup-> GetString ( kXML_ELEMENT_NAME, outName );
					xbox_assert ( bResult );
					vError = VE_OK;

					break;
				}
			}
			vbarrGroups-> Release ( );
		}
		vbagDirectory-> Release ( );
	}

	return vError;
}


VValueBag* VSolution::RetainUserGroups ( VError& outError )
{
	VProjectItem*			itmDirectory = GetProjectItemFromTag ( kUAGDirectoryTag );
	if ( itmDirectory == 0 )
	{
		outError = VE_SOMA_FILE_NOT_FOUND;

		return 0;
	}

	VFilePath				vfpathDirectory;
	bool					bResult = itmDirectory-> GetFilePath ( vfpathDirectory );
	xbox_assert ( bResult );
	if ( !bResult )
	{
		outError = VE_SOMA_FILE_NOT_FOUND;

		return 0;
	}

	VValueBag*				vBagDirectory = 0;

	VFile					vfDirectory ( vfpathDirectory );
	if ( !vfDirectory. Exists ( ) )
		// Solution permissions are referenced but are not present. Technically, not a big deal but just in case let's assert-false this - solution config might just be broken
		xbox_assert ( false );
	else
	{
		vBagDirectory = new VValueBag ( );
		outError = LoadBagFromXML ( vfDirectory, kXML_ELEMENT_DIRECTORY, *vBagDirectory, XML_ValidateNever );
	}

	return vBagDirectory;
}


VError VSolution::SetDebuggerGroup ( VUUID& inUUID )
{
	VError					vError = VE_OK;

	if ( fPermissions == 0 )
	{
		vError = _LoadPermissions ( true );
		xbox_assert ( fPermissions != 0 );
		if ( fPermissions == 0 )
			return VE_SOMA_FAILED_TO_LOAD_SOLUTION_PERMISSIONS;
	}

	VValueBag*				vbagDebuggerGroup = fPermissions-> RetainUniqueElementWithAttribute ( SolutionPermissions::allow, SolutionPermissions::action, CVSTR ( "debug" ) );
	if ( vbagDebuggerGroup == 0 )
	{
		vbagDebuggerGroup = new VValueBag ( );
		fPermissions-> AddElement ( SolutionPermissions::allow, vbagDebuggerGroup );
		vbagDebuggerGroup-> SetString ( SolutionPermissions::action, "debug" );
		vbagDebuggerGroup-> SetString ( SolutionPermissions::resource, "jsRuntime" );
		vbagDebuggerGroup-> SetString ( SolutionPermissions::temporaryForcePermissions, "true" );
		vbagDebuggerGroup-> SetString ( SolutionPermissions::type, "jsDebugger" );
	}
	vbagDebuggerGroup-> SetVUUID ( SolutionPermissions::groupID, inUUID );
	vbagDebuggerGroup-> Release ( );
	fPermissionsHaveBeenModified = true;

	return vError;
}

VError VSolution::DisableDebuggerGroup ( )
{
	VError					vError = VE_OK;

	if ( fPermissions == 0 )
	{
		vError = _LoadPermissions ( );
		if ( fPermissions == 0 || vError != VE_OK )
			// If there are no permissions defined then there is nothing to disable
			return vError;
	}

	VBagArray*				vbarrAllow = fPermissions-> RetainElements ( SolutionPermissions::allow );
	if ( vbarrAllow != 0 )
	{
		for ( VIndex i = 1; i <= vbarrAllow-> GetCount ( ); i++ )
		{
			VValueBag*		vbg = vbarrAllow-> GetNth ( i );
			if ( vbg == 0 )
				continue;

			VString			vstrDebug;
			if ( vbg-> GetString ( SolutionPermissions::action, vstrDebug ) && vstrDebug. EqualToString ( CVSTR ( "debug" ) ) )
			{
				vbarrAllow-> DeleteNth ( i );
				fPermissionsHaveBeenModified = true;

				break;
			}
		}
	}

	/*VValueBag*				vbagDebuggerGroup = fPermissions-> RetainUniqueElementWithAttribute ( SolutionPermissions::allow, SolutionPermissions::action, CVSTR ( "debug" ) );
	if ( vbagDebuggerGroup != 0 )
	{
		vbagDebuggerGroup-> Destroy ( );
		vbagDebuggerGroup-> Release ( );

		fPermissionsHaveBeenModified = true;
	}*/

	return vError;
}

VError VSolution::LoadProjects()
{
	VError err = VE_OK;

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() && err == VE_OK ; ++iter)
	{
		err = (*iter)->Load();
	}
	return err;
}


VError VSolution::UnloadProjects()
{
	VError err = VE_OK;

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->Unload();
	}
	return err;
}


VError VSolution::StartWatchingFileSystem()
{
	VError err = VE_OK;

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->StartWatchingFileSystem();
	}

	if (err == VE_OK)
	{
		VFilePath path;

		if ( ! GetSolutionFolderPath(path) )
			return VE_UNKNOWN_ERROR;

		VFolder folder(path);

		err = VFileSystemNotifier::Instance()->StartWatchingForChanges( folder, VFileSystemNotifier::kAll, this, 100 );
	}

	return err;
}


VError VSolution::StopWatchingFileSystem()
{
	VError err = VE_OK;

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->StopWatchingFileSystem();
	}

	if (err == VE_OK)
	{
		VFilePath path;

		if ( ! GetSolutionFolderPath(path) )
			return VE_UNKNOWN_ERROR;

		VFolder folder(path);

		err = VFileSystemNotifier::Instance()->StopWatchingForChanges( folder, this );
	}

	return err;
}

void VSolution::FileSystemEventHandler( const std::vector< VFilePath > &inFilePaths, VFileSystemNotifier::EventKind inKind )
{
	if (inKind == VFileSystemNotifier::kFileModified)
	{
		for (std::vector<VFilePath>::const_iterator it = inFilePaths.begin(); it != inFilePaths.end(); ++it)
		{
			VProjectItem *item = GetProjectItemFromFullPath( (*it).GetPath() );
			if ( fSolutionItem->IsParentOf(item) && item->GetKind() != VProjectItem::ePROJECT )
			{
				if ( item->ConformsTo( RIAFileKind::kUAGDirectoryFileKind ) ||
					 item->ConformsTo( RIAFileKind::kSettingsFileKind ) )
				{
					std::vector<VFilePath> pathes;

					pathes.push_back( *it );
					if (fDelegate != NULL)	// sc 15/11/2011 WAK0073846
						fDelegate->DoProjectItemsChanged( pathes );
				}
			}
		}
	}
}



VError VSolution::StartUpdatingSymbolTable()
{
	VError err = VE_OK;

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->BackgroundParseFiles();
	}
	return err;
}


VError VSolution::StopUpdatingSymbolTable()
{
	VError err = VE_OK;

	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->StopBackgroundParseFiles();
	}
	return err;
}


XBOX::VError VSolution::ParseCatalogs()
{
	VError err = VE_OK;

	VectorOfProjectItems projectItemsVector;
	fSolutionItem->GetProjectItemsVector(VProjectItem::eCATALOG_FILE, projectItemsVector, false);

	for (VectorOfProjectItemsIterator i = projectItemsVector.begin(); 
		i != projectItemsVector.end(); ++i)
	{
		err = ReloadCatalog( *i);
	}
			
	return err;
}

void VSolution::LoadFromUserFile()
{
	if (fSolutionUser)
		LoadSolutionUserFile();
}


// synchronisation de toute la solution
void VSolution::SynchronizeWithFileSystem()
{
	// TBD: synchronize the children of the solution
	
	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->SynchronizeWithFileSystem( (*iter)->GetProjectItem());
	}
}


VError VSolution::_LoadSolutionFile()
{
	VError err = VE_OK;

	VProjectItem* solutionFileProjectItem = GetSolutionFileProjectItem();

	if (solutionFileProjectItem != NULL)
	{
		VFile solutionFile(solutionFileProjectItem->GetURL());
		VValueBag *bagSolution = new VValueBag();
		err = LoadBagFromXML( solutionFile, fSolutionItem->GetXMLElementName(), *bagSolution, XML_ValidateNever);
		if (err == VE_OK)
		{
			// -----------------------------------------------------------
			// on traite d'abord l'ensemble des projets
			// -----------------------------------------------------------
			VBagArray*	bagArrayProjects = bagSolution->RetainElements(kXML_ELEMENT_PROJECT);
			for (sLONG i = 1; i <= bagArrayProjects->GetCount(); i++)
			{
				VValueBag* bagProject = bagArrayProjects->GetNth(i);

				// recuperer le path relatif depuis le fichier solution
				VString path;
				bagProject->GetString(ProjectItemBagKeys::path, path);
				path.TrimeSpaces();

				VFilePath projectFilePath;
				BuildPathFromRelativePath( projectFilePath, path, FPS_POSIX);
				projectFilePath.GetPath( path);

				// Tester si le fichier existe avant d'aller plus loin !
				VFile file( projectFilePath);
				if (!file.Exists())
				{
					if (fSolutionMessageManager != NULL)
					{
						VString localizedMessage;
						fSolutionMessageManager->GetLocalizedStringFromError(VE_SOMA_PROJECT_FILE_NOT_FOUND, localizedMessage);
						fSolutionMessageManager->DisplayMessage(localizedMessage+"\n"+path);
					}
					continue;
				}

				VError lErr = VE_OK;
				VProject *project = VProject::Instantiate( lErr, this, projectFilePath);
				if (lErr == VE_OK && project != NULL)
				{
					_AddProject( project, true, false);

					if (ProjectItemBagKeys::startupProject.Get( bagProject) && (fStartupProject == NULL))
						fStartupProject = project;
				}
				else
				{
					ReleaseRefCountable( &project);
				}
			}
			bagArrayProjects->Release();
			// -----------------------------------------------------------
			// on traite ensuite l'ensemble des fichiers
			// -----------------------------------------------------------
			std::vector<VProjectItemTag> singleFileTagsFound;
			VBagArray* bagArrayFiles = bagSolution->RetainElements(kXML_ELEMENT_FILE);
			for (sLONG i = 1; i <= bagArrayFiles->GetCount(); i++)
			{
				err = VE_OK;
				VValueBag* bagFile = bagArrayFiles->GetNth(i);
				// recuperer le path relatif depuis le fichier projet
				VString path;
				bagFile->GetString(ProjectItemBagKeys::path, path);
				path.TrimeSpaces();
				if (!path.IsEmpty())
				{
					// en deduire le path complet du fichier
					VFilePath externalFileVFilePath;
					BuildPathFromRelativePath(externalFileVFilePath, path, FPS_POSIX);
					externalFileVFilePath.GetPath(path);

					// tester si le fichier existe avant d'aller plus loin !
					VFile file(externalFileVFilePath);
					if (!file.Exists())
					{
						err = VE_SOMA_SETTINGS_FILE_NOT_FOUND;
						VErrorBase* errorBase = new VErrorBase(err, 0);
						errorBase->GetBag()->SetString(kSOMA_FILE_FULL_PATH, path);
						VTask::GetCurrent()->PushRetainedError(errorBase);
						continue;
					}
					else
					{
						VProjectItem* projectItem = fSolutionItem->NewChild( VURL(externalFileVFilePath), VProjectItem::eFILE);
						if (projectItem != NULL)
						{
							// Read the item tags
							VBagArray *tagArray = bagFile->RetainElements( kXML_ELEMENT_TAG);
							if (tagArray != NULL)
							{
								for (VIndex pos = 1 ; pos <= tagArray->GetCount() ; ++pos)
								{
									VProjectItemTag tagName = ProjectItemBagKeys::name.Get( tagArray->GetNth( pos));
									if (VProjectItemTagManager::Get()->IsRegisteredProjectItemTag( tagName))
									{
										// Sanity check
										if (	((VProjectItemTagManager::Get()->GetProperties( tagName) & ePITP_ApplyToSingleFile) != 0)
											||	((VProjectItemTagManager::Get()->GetProperties( tagName) & ePITP_ApplyToSingleFolder) != 0) 
											||	(tagName == kSettingTag))
										{
											if (std::find( singleFileTagsFound.begin(), singleFileTagsFound.end(), tagName) == singleFileTagsFound.end())
											{
												singleFileTagsFound.push_back( tagName);
												projectItem->AddTag( tagName);
											}
										}
										else
										{
											projectItem->AddTag( tagName);
										}
									}
								}
							}
							ReleaseRefCountable( &tagArray);

							if (projectItem->IsTagged())
								_ReferenceItem( projectItem, false);
						}
					}
				}
			}
			ReleaseRefCountable( &bagArrayFiles);
		}
		else
		{
			err = VE_SOMA_SOLUTION_FILE_COULD_NOT_OPENED;
			VErrorBase* errorBase = new VErrorBase(err, 0);
			VTask::GetCurrent()->PushRetainedError(errorBase);
		}
		ReleaseRefCountable( &bagSolution);
	}

	return err;
}


// CreatePhysicalItem cree la representation physique d'un projectItem
// ex: un fichier sur disque, une table dans le catalogue
// les informations necessaires sont fournies par le VProjectItem en entree
VError VSolution::CreatePhysicalItem(VProjectItem* inNewProjectItem, const VString& inSubType)
{
	VError err = VE_OK;

	if (inNewProjectItem->GetParent()->GetKind() == VProjectItem::eSOLUTION)
		if (inNewProjectItem->GetKind() != VProjectItem::ePROJECT)
			return VE_INVALID_PARAMETER;		

	if (inNewProjectItem != NULL)
	{
		if (fDelegate != NULL)
			fDelegate->DoStartPossibleLongTimeOperation();

        // creer l'item physique correspondant
		if (!inNewProjectItem->ContentExists())
		{
			if (inNewProjectItem->CreatePhysicalItem())
			{
				_InitializeProjectItem(inNewProjectItem, inSubType);
			}
			else
			{
				err = VE_SOMA_FILE_COULD_NOT_BE_CREATED;	
				if (fSolutionMessageManager)
				{
					VString localizedMessage;
					fSolutionMessageManager->GetLocalizedStringFromError(VE_SOMA_FILE_COULD_NOT_BE_CREATED, localizedMessage);
					fSolutionMessageManager->DisplayMessage(localizedMessage);
				}
			}
		}
		else
		{
			err = VE_SOMA_FILE_ALREADY_EXISTS;
			if (fSolutionMessageManager)
			{
				VString localizedMessage;
				fSolutionMessageManager->GetLocalizedStringFromError(VE_SOMA_FILE_ALREADY_EXISTS, localizedMessage);
				fSolutionMessageManager->DisplayMessage(localizedMessage);
			}
		}

		if (fDelegate != NULL)
			fDelegate->DoEndPossibleLongTimeOperation();
	}

	return err;
}

VError VSolution::InitializeDefaultFileContent( const VFilePath& inPath, const VString& inSubType )
{
	VError err = VE_OK;

	VString strExtension, strInitialization;

	inPath.GetExtension(strExtension);
	
	if (strExtension == RIAFileKind::kHTMLFileExtension)
	{
		VFolder* resourcesFolder = VProcess::Get()->RetainFolder( VProcess::eFS_Resources);
		VFilePath sourcePath;
		resourcesFolder->GetPath( sourcePath);
		resourcesFolder->Release();

		sourcePath.ToSubFolder( VString( DEFAULT_FILES_PATH ));
		if ( inSubType.IsEmpty() )
			sourcePath.ToSubFile( VString( INTRO_HTML_BASIC ));
		else
			sourcePath.ToSubFile( inSubType );

		VFile sourceFile( sourcePath );
		VMemoryBuffer<> buffer;
		sourceFile.GetContent( buffer );
		strInitialization.FromBlock( buffer.GetDataPtr(), buffer.GetDataSize(), XBOX::VTC_UTF_8);

		VString	newTitle;
		inPath.GetFileName(newTitle, false);
		newTitle.TranscodeToXML();
		newTitle.Insert(CVSTR("<title>"), 1);
		newTitle += CVSTR("</title>");
		strInitialization.Exchange( CVSTR("<title></title>"), newTitle);
	}
	else if (strExtension == RIAFileKind::kXMLFileExtension)
		strInitialization.FromCString(INTRO_XML_UTF8);
	else if (strExtension == RIAFileKind::kJSFileExtension)
		strInitialization.FromCString(INTRO_JAVASCRIPT);
	else if (strExtension == RIAFileKind::kCSSFileExtension)
		strInitialization.FromCString(INTRO_CSS);
	else if (strExtension == RIAFileKind::kPHPFileExtension)
		strInitialization.FromCString(INTRO_PHP);
	else if (strExtension == RIAFileKind::kCatalogFileExtension)
		strInitialization.FromCString(INTRO_CATALOG);
	else if (strExtension == RIAFileKind::kPermissionsFileExtension)
		strInitialization.FromCString(INTRO_PERM);

	if (!strInitialization.IsEmpty())
	{
		VFile file(inPath);
		VFileStream stream( &file );
		err = stream.OpenWriting();

		if ( VE_OK == err )
		{
			stream.SetSize( 0 );
			stream.SetCharSet( VTC_UTF_8 );
			stream.SetCarriageReturnMode( eCRM_NATIVE );
			stream.WriteBOM();
			stream.PutText( strInitialization );
			stream.CloseWriting();
		}
	}
	return err;
}


void VSolution::_SetSolutionItem( VProjectItem *inProjectItem)
{
	if (testAssert(fSolutionItem == NULL && inProjectItem != NULL))
	{
		fSolutionItem = RetainRefCountable( inProjectItem);

		VProjectItemSolution *solutionBehaviour = dynamic_cast<VProjectItemSolution*>(fSolutionItem->GetBehaviour());
		if (testAssert(solutionBehaviour != NULL))
			solutionBehaviour->SetSolution( this);
	}
}


void VSolution::_SetSolutionFileItem( VProjectItem *inProjectItem)
{
	if (testAssert(fSolutionFileProjectItem == NULL && inProjectItem != NULL))
	{
		fSolutionFileProjectItem = RetainRefCountable( inProjectItem);

		if (fSolutionItem != NULL)
			fSolutionItem->AttachChild( fSolutionFileProjectItem);
	}
}


void VSolution::_AddProject( VProject *inProject, bool inReferenceIt, bool inTouchSolutionFile)
{
	if (inProject != NULL)
	{
		if (testAssert(std::find( fProjects.begin(), fProjects.end(), inProject) == fProjects.end()))
		{
			fProjects.push_back(inProject);

			if (testAssert(inProject->GetProjectItem() != NULL))
			{
				fSolutionItem->AttachChild( inProject->GetProjectItem());
				if (inReferenceIt)
				{
					_ReferenceItem( inProject->GetProjectItem(), inTouchSolutionFile);
				}
			}
		}
	}
}


VError VSolution::_RemoveProject( VProject *inProject, bool inDeleteProjectFolder, bool inTouchSolutionFile)
{
	VError err = VE_OK;

	if (inProject != NULL)
	{
		if (std::find( fProjects.begin(), fProjects.end(), inProject) != fProjects.end())
		{
			VFilePath projectFolderPath;
			
			inProject->GetProjectFolderPath( projectFolderPath);
			inProject->StopWatchingFileSystem();
			inProject->StopBackgroundParseFiles();
			inProject->Unload();

			_RemoveProject( inProject, inTouchSolutionFile);

			if (fStartupProject == inProject)
			{
				if (fProjects.empty())
					fStartupProject = NULL;
				else
					fStartupProject = fProjects.front();

				if (inTouchSolutionFile)
					_TouchSolutionFile();
			}

			if (inDeleteProjectFolder)
			{
				VFolder folder( projectFolderPath);
				folder.Delete( true);
			}
			
			// Notify solution explorer delegates that project has been deleted
			if (fDelegate != NULL)
				fDelegate->DoProjectFileDeleted();
		}
		else
		{
			err = VE_SOMA_UNKNOWN_PROJECT;
		}
	}
	return err;
}


void VSolution::_RemoveProject( VProject *inProject, bool inTouchSolutionFile)
{
	if (inProject != NULL)
	{
		VectorOfProjectsIterator found = std::find( fProjects.begin(), fProjects.end(), inProject);
		if (testAssert(found != fProjects.end()))
		{
			if (testAssert(inProject->GetProjectItem() != NULL))
			{
				_UnreferenceItem( inProject->GetProjectItem(), inTouchSolutionFile);

				fSolutionItem->DetachChild( inProject->GetProjectItem());
			}

			fProjects.erase( found);
		}
	}
}


void VSolution::_DoItemAdded( VProjectItem *inItem, bool inTouchSolutionFile)
{
	// Append the item
	if (inItem->IsTagged() && !_IsItemReferenced( inItem))
		_ReferenceItem( inItem, inTouchSolutionFile);

	// Append child items
	for (VProjectItemIterator iter(inItem) ; iter.IsValid() ; ++iter)
	{
		_DoItemAdded( iter, inTouchSolutionFile);
	}
}


void VSolution::_DoItemRemoved( VProjectItem *inItem, bool inTouchSolutionFile)
{
	// Remove the children
	for (VProjectItemIterator iter(inItem) ; iter.IsValid() ; ++iter)
	{
		_DoItemRemoved( iter, inTouchSolutionFile);
	}

	// Remove the item
	if (_IsItemReferenced( inItem))
		_UnreferenceItem( inItem, inTouchSolutionFile);
}


void VSolution::_TouchSolutionFile()
{
	++fSolutionFileDirtyStamp;
}


VError VSolution::_InitializeProjectItem(VProjectItem* inProjectItem, const VString& inSubType)
{
	VError err = VE_OK;

	if (inProjectItem != NULL)
	{
		if (inProjectItem->IsPhysicalFile())
		{
			VFilePath filePath;
			inProjectItem->GetFilePath(filePath);
			err = InitializeDefaultFileContent( filePath, inSubType );
		}
	}

	// retour code erreur : success
	return err;
}


VError VSolution::ImportFromExistingFile(VProjectItem* inNewProjectItem, const VURL& inExistingURL)
{
	VError err = VE_OK;

	if (inNewProjectItem != NULL)
	{
		if (fDelegate != NULL)
			fDelegate->DoStartPossibleLongTimeOperation();

        // copier l'item physique correspondant
		if (inNewProjectItem->CopyPhysicalItemFrom(inExistingURL))
		{
			VFilePath filePath;
			inNewProjectItem->GetFilePath(filePath);
			VFile file(filePath);

			VProject* project = inNewProjectItem->GetProjectOwner();
			assert(project != NULL);
			if (project)
			{
				project->RegisterProjectItemInMapOfFullPathes(inNewProjectItem);
			}
		}
		else
		{
			err = VE_FILE_CANNOT_COPY;
		}

		if (fDelegate != NULL)
			fDelegate->DoEndPossibleLongTimeOperation();
	}

	return err;
}


VProjectItem* VSolution::ImportFromExistingFolder( XBOX::VError& outError, VProjectItem* inProjectItem, const XBOX::VFilePath& inNewParentFolderPath, const XBOX::VFilePath& inSrcFolderPath, bool inIncludeSrcFolder, XBOX::VProgressIndicator* inProgressIndicator)
{
	VProjectItem *newProjectItem = NULL;
	
	outError = VE_OK;

	VFilePath newParentFolderPath(inNewParentFolderPath);
	VString strSrcFolderName;
	inSrcFolderPath.GetName(strSrcFolderName);
	if ((newParentFolderPath == inSrcFolderPath) || newParentFolderPath.IsChildOf(inSrcFolderPath))
	{
		outError = VE_SOMA_DEST_FOLDER_IS_SUBFOLDER_OF_SRC_FOLDER;
		if (fSolutionMessageManager)
		{		
			VErrorBase errorBase(outError, 0);
			errorBase.GetBag()->SetString(kSOMA_SOURCE_FOLDER_NAME, strSrcFolderName);
			VString strNewParentFolderName;
			newParentFolderPath.GetName(strNewParentFolderName);
			errorBase.GetBag()->SetString(kSOMA_DESTINATION_FOLDER_NAME, strNewParentFolderName);
			VString localizedMessage;
			errorBase.GetErrorDescription(localizedMessage);
			fSolutionMessageManager->DisplayMessage(localizedMessage);
		}
	}
	else
	{
		VString strDestFolderPath;
		newParentFolderPath.GetPath(strDestFolderPath);
		if (inIncludeSrcFolder)
		{
			strDestFolderPath += strSrcFolderName;
			strDestFolderPath += FOLDER_SEPARATOR;
		}
		VFilePath destFolderPath(strDestFolderPath);
		// test prealable
		VProjectItem* projectItem = GetProjectItemFromFullPath(strDestFolderPath);
		if (projectItem)
		{
			if (fSolutionMessageManager)
			{
				VString localizedMessage;
				fSolutionMessageManager->GetLocalizedStringFromResourceName(kSOMA_DESTINATION_FOLDER_ALREADY_EXISTS_NO_IMPORT, localizedMessage);
				fSolutionMessageManager->DisplayMessage(localizedMessage);
			}
			outError = VE_FOLDER_ALREADY_EXISTS;
		}
		else
		{
			VURL srcFolderURL(inSrcFolderPath);
			VURL destFolderURL(destFolderPath);

			bool userAbort = false;
			// on se base seulement sur le nombre de fichiers pour le progressIndicator
			sLONG allFilesCount = VProjectItemManager::GetFilesCountInFolderAndSubfolder(VFolder(inSrcFolderPath));
			if (inProgressIndicator)
				// TO DO a localiser !!!
				inProgressIndicator->BeginSession(allFilesCount, CVSTR("Copying files..."), true);

			// copier l'item physique correspondant
			bool ok = VProjectItemManager::FolderCopyTo(inSrcFolderPath, destFolderPath, inProgressIndicator, &userAbort);
			if (ok)
			{
				VProject *project = inProjectItem->GetProjectOwner();
				project->SynchronizeWithFileSystem( inProjectItem);
				newProjectItem = project->GetProjectItemFromFullPath( destFolderPath.GetPath());
			}
		}
	}

	return newProjectItem;
}


VProjectItem* VSolution::ReferenceExternalFolder( XBOX::VError& outError, VProjectItem *inParentItem, const XBOX::VURL& inURL)
{
	outError = VE_OK;
	VProjectItem *result = NULL;

	if (inParentItem != NULL)
	{
		VProject *project = inParentItem->GetProjectOwner();
		if (project != NULL)
			result = project->ReferenceExternalFolder( outError, inParentItem, inURL);
		else
			outError = VE_UNKNOWN_ERROR;
	}
	return result;
}


VProjectItem* VSolution::ReferenceExternalFile( XBOX::VError& outError, VProjectItem *inParentItem, const XBOX::VURL& inURL)
{
	outError = VE_OK;
	VProjectItem *result = NULL;

	if (inParentItem != NULL)
	{
		VProject *project = inParentItem->GetProjectOwner();
		if (project != NULL)
			result = project->ReferenceExternalFile( outError, inParentItem, inURL);
		else
			outError = VE_UNKNOWN_ERROR;
	}
	return result;
}


VProject* VSolution::AddExistingProject(const VURL& inProjectFileURL, bool inReferenceIt)
{
	if (IsProjectInSolution(inProjectFileURL))
	{
		if (fSolutionMessageManager)
		{
			VString localizedMessage;
			fSolutionMessageManager->GetLocalizedStringFromResourceName(kSOMA_PROJECT_ALREADY_EXISTS, localizedMessage);
			fSolutionMessageManager->DisplayMessage(localizedMessage);
		}
		return NULL;
	}

	VError lErr = VE_OK;
	VFilePath projectFilePath;
	inProjectFileURL.GetFilePath( projectFilePath);
	VProject *project = VProject::Instantiate( lErr, this, projectFilePath);
	if (lErr == VE_OK && project != NULL)
	{
		e_Save_Action saveAction = e_SAVE;

		if (fDelegate != NULL)
			saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_AppendProject, true);

		if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
		{
			VSolutionFileStampSaver stampSaver( this);
			
			_AddProject( project, inReferenceIt, true);

			project->Load();

			if (fStartupProject == NULL)
			{
				fStartupProject = project;
				_TouchSolutionFile();
			}

			if ((saveAction == e_SAVE) && stampSaver.StampHasBeenChanged())
				_SaveSolutionFile();
		}
		else
		{
			ReleaseRefCountable( &project);
		}
	}
	else
	{
		ReleaseRefCountable( &project);
	}

	return project;
}


VError VSolution::RemoveProject( VProject *inProject, bool inDeleteProjectFolder)
{
	VError err = VE_OK;

	if (inProject != NULL)
	{
		e_Save_Action saveAction = e_SAVE;

		if (fDelegate != NULL)
			saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_RemoveProject, true);

		if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
		{
			VSolutionFileStampSaver stampSaver( this);

			_RemoveProject( inProject, inDeleteProjectFolder, true);

			// Save the solution file
			if ((saveAction == e_SAVE) && stampSaver.StampHasBeenChanged())
				_SaveSolutionFile();
		}
	}
	return err;
}


bool VSolution::IsProjectInSolution(const VURL& inProjectFileURL) const
{
	VFilePath projectFilePath;
	inProjectFileURL.GetFilePath(projectFilePath);	
	VString strProjectFileFullPath;
	projectFilePath.GetPath(strProjectFileFullPath);

	bool found = false;

	for (VectorOfProjectsConstIterator iter = fProjects.begin() ; iter != fProjects.end() && !found ; ++iter)
	{
		VString strProjectPath;
		(*iter)->GetProjectItemProjectFile()->BuildFullPath(strProjectPath);
		if (strProjectFileFullPath == strProjectPath)
			found = true;
	}
	return found;
}


VError VSolution::SetStartupProject( VProject *inProject)
{
	if (fStartupProject != inProject)
	{
		e_Save_Action saveAction = e_SAVE;

		if (fDelegate != NULL)
			saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_ChangeStartupProject, true);

		if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
		{
			fStartupProject = inProject;
			_TouchSolutionFile();

			if (saveAction == e_SAVE)
				_SaveSolutionFile();
		}
	}

	return VE_OK;
}


bool VSolution::IsStartupProject( const VProject *inProject) const
{
	return fStartupProject == inProject;
}


VProject* VSolution::GetStartupProject() const
{
	return fStartupProject;
}


bool VSolution::ContainsProject(VProjectItem* inProjectItem) const
{
	bool found = false;

	if (inProjectItem->GetKind() == VProjectItem::ePROJECT)	
	{
		for (VectorOfProjectsConstIterator it = fProjects.begin(); it != fProjects.end(); ++it)
		{
			if ((*it)->GetProjectItem() == inProjectItem)
			{
				found = true;
				break;
			}
		}
	}

	return found;
}


XBOX::VError VSolution::RemoveItem( VProjectItem *inProjectItem, bool inDeletePhysicalItems)
{
	VectorOfProjectItems items( 1, inProjectItem);
	return RemoveItems( items, inDeletePhysicalItems);
}


XBOX::VError VSolution::RemoveItems( const VectorOfProjectItems& inProjectItems, bool inDeletePhysicalItems)
{
	VError err = VE_OK;
	e_Save_Action saveAction = e_SAVE;

	if ((fDelegate != NULL) && _IsVectorContainsReferencedItems( inProjectItems, true))
		saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_RemoveItem, true);

	if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
	{
		VSolutionFileStampSaver stampSaver( this);
		VectorOfProjectItems items( inProjectItems.begin(), inProjectItems.end());
		
		VProjectItemTools::RemoveUselessItems( items);

		for (VectorOfProjectItemsIterator itemIter = items.begin() ; itemIter != items.end() ; ++itemIter)
		{
			if ((*itemIter != fSolutionItem) && (*itemIter != fSolutionFileProjectItem))
			{
				if (fSolutionItem->IsParentOf( *itemIter))
				{
					if ((*itemIter)->GetKind() == VProjectItem::ePROJECT)
					{
						VProject *project = (*itemIter)->GetBehaviour()->GetProject();
						if (testAssert(project != NULL))
						{
							_RemoveProject( project, inDeletePhysicalItems, true);
							ReleaseRefCountable( &project);
						}
					}
					else
					{
						_DoItemRemoved( *itemIter, true);

						if (inDeletePhysicalItems)
							(*itemIter)->DeleteContent();
					}

					(*itemIter)->SetGhost(true);
					(*itemIter)->DeleteChildren();
					(*itemIter)->Release();
				}
				else
				{
					VProject *project = (*itemIter)->GetProjectOwner();
					if (testAssert(project != NULL))
						project->RemoveItem( *itemIter, inDeletePhysicalItems);
				}
			}
		}

		if ((saveAction == e_SAVE) && stampSaver.StampHasBeenChanged())
			_SaveSolutionFile();
	}
	return err;
}


XBOX::VError VSolution::RenameItem( VProjectItem *inProjectItem, const XBOX::VString& inNewName)
{
	VError err = VE_OK;

	if (inProjectItem != NULL)
	{
		if ((inProjectItem == fSolutionItem) || (inProjectItem == fSolutionFileProjectItem))
		{
			VString name( inNewName);
			VString extension( L"." + RIAFileKind::kSolutionFileExtension);
			if (name.EndsWith( extension))
				name.Truncate( name.GetLength() -  extension.GetLength());

			err = Rename( name);
		}
		else
		{
			if (fSolutionItem->IsParentOf( inProjectItem))
			{
				VSolutionFileStampSaver stampSaver( this);
				e_Save_Action saveAction = e_SAVE;

				if (inProjectItem->GetKind() == VProjectItem::ePROJECT)
				{
					if (fDelegate != NULL)
						saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_RenameProject, true);

					if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
					{
						VProject *project = inProjectItem->GetBehaviour()->GetProject();
						if (testAssert(project != NULL))
						{
							VString name( inNewName);
							VString extension( L"." + RIAFileKind::kProjectFileExtension);
							if (name.EndsWith( extension))
								name.Truncate( name.GetLength() -  extension.GetLength());
							
							err = project->Rename( name);
							if (err == VE_OK)
								_TouchSolutionFile();
						}
					}
				}
				else
				{
					VString name;
					inProjectItem->GetName( name);
					if (name != inNewName)
					{
						if ((fDelegate != NULL) && _IsItemReferenced( inProjectItem))
							saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_RenameItem, true);

						if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
						{
							_DoItemRemoved( inProjectItem, true);

							err = inProjectItem->RenameContent( inNewName);
							if (err == VE_OK)
								inProjectItem->Touch();

							_DoItemAdded( inProjectItem, true);
						}
					}
				}

				if ((saveAction == e_SAVE) && stampSaver.StampHasBeenChanged())
					_SaveSolutionFile();
			}
			else
			{
				VProject *project = inProjectItem->GetProjectOwner();
				if (testAssert(project != NULL))
					err = project->RenameItem( inProjectItem, inNewName);
			}
		}
	}

	return err;
}


void VSolution::UpdateSolutionLinkFile()
{
	VSolutionStartupParameters *startupParams = RetainStartupParameters();
	if ((startupParams != NULL) && startupParams->GetStoreInLinkFile())
	{
		VString strSolutionName;
		fSolutionItem->GetDisplayName(strSolutionName);
		SaveSolutionStartupParametersToLinkFile( strSolutionName, *startupParams);
	}
	QuickReleaseRefCountable( startupParams);
}


VError VSolution::ReloadCatalog(VProjectItem* inProjectItem)
{
	VError err = VE_OK;

	if (inProjectItem->ConformsTo( RIAFileKind::kCatalogFileKind))
	{
		VCatalog* catalog = inProjectItem->GetBehaviour()->GetCatalog();
		if (catalog)
		{
			inProjectItem->DeleteChildren();
			err = catalog->ParseCatalogAndCreateProjectItems();
		}
	}

	return err;
}

void VSolution::SaveProjectItemPosition( const VFilePath& inFilePath, bool inMaximized, sLONG inMaxX, sLONG inMaxY, sLONG inX, sLONG inY, sLONG inWidth, sLONG inHeight )
{
	if ( fSolutionUser )
		fSolutionUser->SaveProjectItemPosition( inFilePath, inMaximized, inMaxX, inMaxY, inX, inY, inWidth, inHeight );
}

bool VSolution::GetProjectItemPosition( const VFilePath& inFilePath, bool& outMaximized, sLONG& outMaxX, sLONG& outMaxY, sLONG& outX, sLONG& outY, sLONG& outWidth, sLONG& outHeight )
{
	if ( fSolutionUser )
		return fSolutionUser->GetProjectItemPosition( inFilePath, outMaximized, outMaxX, outMaxY, outX, outY, outWidth, outHeight );
	else
		return false;
}


void VSolution::SetSolutionStartupParameters(VSolutionStartupParameters* inSolutionStartupParameters)
{
	XBOX::CopyRefCountable(&fSolutionStartupParameters, inSolutionStartupParameters);
}


void VSolution::GetPreferencesUserFilePath( VFilePath& outPath )
{
	fSolutionItem->GetURL().GetFilePath(outPath);
	VString fileName;
	fSolutionItem->GetDisplayName(fileName);
	fileName += ".";
	VString userName;
	VSystem::GetLoginUserName(userName);
	fileName += userName;
	fileName += ".";
	fileName += RIAFileKind::kPreferencesFileExtension;
	outPath.SetFileName( fileName );
}


void VSolution::LoadSolutionUserFile()
{
	if (fSolutionUser)
	{
		VFilePath path;
		GetPreferencesUserFilePath(path);
		VFile userFile(path);
		fSolutionUser->LoadFromUserFile(userFile);
	}
}

void VSolution::SaveSolutionUserFile()
{
	if (fSolutionUser)
	{
		VFilePath path;
		GetPreferencesUserFilePath(path);
		VFile userFile(path);
		fSolutionUser->SaveToUserFile(userFile);
	}
}

VSolutionStartupParameters* VSolution::RetainStartupParameters()
{
	if (fSolutionStartupParameters == NULL)
	{
		fSolutionStartupParameters = new VSolutionStartupParameters();
		if (fSolutionStartupParameters != NULL)
		{
			VProjectItem *solutionFileProjectItem = GetSolutionFileProjectItem();
			if (solutionFileProjectItem != NULL)
			{
				VFile *solutionFile = new VFile( solutionFileProjectItem->GetURL());
				fSolutionStartupParameters->SetSolutionFileToOpen( solutionFile);
				QuickReleaseRefCountable( solutionFile);
			}
		}
	}

	return RetainRefCountable( fSolutionStartupParameters);
}


void VSolution::ConnectProjectsToSourceControl(XBOX::VString& inSourceControlId)
{
	for (VectorOfProjectsIterator it = fProjects.begin(); it != fProjects.end(); ++it)
		(*it)->ConnectToSourceControl(inSourceControlId);
}

VError VSolution::_DispatchCommandByProject(_e_Command inCommand, const VectorOfProjectItems& inSelectedProjectItems)
{
	VError err = VE_OK;

	assert(!inSelectedProjectItems.empty());

	if (!inSelectedProjectItems.empty())
	{
		VectorOfProjects candidateProjects;
		
		for (VectorOfProjectItemsConstIterator it = inSelectedProjectItems.begin();
			it != inSelectedProjectItems.end(); ++it)
		{
			VProject* project = (*it)->GetProjectOwner();
			if (project != NULL && project->IsConnectedToSourceControl())
			{
				if (std::find( candidateProjects.begin(), candidateProjects.end(), project) == candidateProjects.end())
					candidateProjects.push_back( project);
			}
		}

		for (VectorOfProjectsIterator it_project = candidateProjects.begin(); it_project != candidateProjects.end(); ++it_project)
		{
			VectorOfProjectItems projectItems;
			for (VectorOfProjectItemsConstIterator it = inSelectedProjectItems.begin();
				it != inSelectedProjectItems.end(); ++it)
			{
				if ((*it)->GetProjectOwner() == (*it_project))
				{
					projectItems.push_back((*it));
				}
			}

			switch(inCommand)
			{
			case eADD_TO_SOURCE_CONTROL:
				err = (*it_project)->AddToSourceControl(projectItems);
				break;
			case eGET_LATEST_VERSION:
				err = (*it_project)->GetLatestVersion(projectItems);
				break;
			case eCHECKOUT:
				err = (*it_project)->CheckOut(projectItems);
				break;
			case eCHECKIN:
				err = (*it_project)->CheckIn(projectItems);
				break;
			case eREVERT:
				err = (*it_project)->Revert(projectItems);
				break;
			default:
				assert(false);
			}
		}
	}

	return err;
}

VError VSolution::AddToSourceControl(const VectorOfProjectItems& inProjectItems)
{
	return _DispatchCommandByProject(eADD_TO_SOURCE_CONTROL, inProjectItems);
}

VError VSolution::GetLatestVersion(const VectorOfProjectItems& inProjectItems)
{
	return _DispatchCommandByProject(eGET_LATEST_VERSION, inProjectItems);
}

VError VSolution::CheckOut(const VectorOfProjectItems& inProjectItems)
{
	return _DispatchCommandByProject(eCHECKOUT, inProjectItems);
}

VError VSolution::CheckIn(const VectorOfProjectItems& inProjectItems)
{
	return _DispatchCommandByProject(eCHECKIN, inProjectItems);
}

VError VSolution::Revert(const VectorOfProjectItems& inProjectItems)
{
	return _DispatchCommandByProject(eREVERT, inProjectItems);
}


VError VSolution::DoSolutionWillClose()
{
	VError err = VE_OK;

	if (fSuccessfulLoading)
	{
		for (VectorOfProjectsIterator iter = fProjects.begin() ; (iter != fProjects.end()) && (err == VE_OK) ; ++iter)
		{
			err = (*iter)->DoProjectWillClose();
		}

		if (err == VE_OK)
		{
			if (fSolutionFileDirtyStamp > 0)
			{
				if (fDelegate != NULL)
				{
					e_Save_Action saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_BeforeClosing, true);

					if ((saveAction == e_SAVE) || (saveAction == e_NO_SAVE))
					{
						if (saveAction == e_SAVE)
							err = _SaveSolutionFile();
					}
					else
					{
						err = VE_USER_ABORT;
					}
				}
			}
		}
	}

	return err;
}


VError VSolution::Close()
{
	for (VectorOfProjectsIterator iter = fProjects.begin() ; iter != fProjects.end() ; ++iter)
	{
		(*iter)->Close();
	}

	ReleaseRefCountable( &fParsingManager);

	return VE_OK;
}


VProject* VSolution::CreateProjectFromTemplate( VError& outError, const VFolder& inParentFolder, const VString& inProjectName, const VFolder& inProjectTemplateFolder)
{
	if (!inProjectTemplateFolder.Exists() || !inParentFolder.Exists())
	{
		outError = VE_FOLDER_NOT_FOUND;
		return NULL;
	}

	if (!inProjectTemplateFolder.Contains(RIAFileKind::kProjectFileExtension))
	{
		outError = VE_SOMA_PROJECT_FILE_NOT_FOUND;
		return NULL;
	}

	if (inProjectName.IsEmpty())
	{
		outError = VE_UNKNOWN_ERROR;
		return NULL;
	}

	e_Save_Action saveAction = e_SAVE;
	
	if (fDelegate != NULL)
		saveAction = fDelegate->DoActionRequireSolutionFileSaving( fSolutionFileProjectItem, eSA_CreateProject, true);

	if ((saveAction != e_SAVE) && (saveAction != e_NO_SAVE))
		return VE_OK;
	
	outError = VE_OK;
	VProject *project = NULL;

	VFilePath projectFolderPath( inParentFolder.GetPath());
	projectFolderPath.ToSubFolder( inProjectName);
	VFilePath projectFilePath( projectFolderPath);
	projectFilePath.SetFileName( inProjectName, false);
	projectFilePath.SetExtension( RIAFileKind::kProjectFileExtension);

	// Check if this project already exists in the solution
	for (VectorOfProjectsConstIterator projectIter = fProjects.begin() ; projectIter != fProjects.end() && outError == VE_OK ; ++projectIter)
	{
		VFilePath path ;
		(*projectIter)->GetProjectFilePath( path);
		if (path == projectFilePath)
			outError = VE_SOMA_PROJECT_ALREADY_EXISTS;
	}

	if (outError == VE_OK)
	{
		sLONG httpPort = 8081;
		VFolder projectFolder( projectFolderPath);

		if (!projectFolder.Exists())
			outError = projectFolder.Create();

		if (outError == VE_OK)
		{
			// Calculate the http port for the new project
			std::set<sLONG> usedHttpPort;
			for (VectorOfProjectsIterator projectIter = fProjects.begin() ; projectIter != fProjects.end() ; ++projectIter)
			{
				VError lerr = VE_OK;
				VProjectSettings *settings = (*projectIter)->RetainSettings( lerr);
				if (lerr == VE_OK && settings != NULL)
				{
					if (settings->HasHTTPServerSettings())
					{
						usedHttpPort.insert( settings->GetListeningPort());
					}
				}
				ReleaseRefCountable( &settings);
			}

			while (usedHttpPort.find( httpPort) != usedHttpPort.end())
				++httpPort;
		}

		if (outError == VE_OK)
		{
			// Copy the template folder
			outError = CopyFolder( inProjectTemplateFolder, projectFolder);
		}
		
		if (outError == VE_OK)
		{
			// Resolve the files and folders names
			outError = _ResolveMacro( projectFolder, kTEMPLATE_PROJECT_NAME_MACRO, inProjectName);
		}

		if (outError == VE_OK)
		{
			// Rename the project file if needed
			VFile *projectFile = new VFile( projectFilePath);
			if (projectFile != NULL && !projectFile->Exists())
			{
				bool found = false;
				for (VFileIterator it_file( &projectFolder, FI_WANT_FILES | FI_WANT_INVISIBLES) ; it_file.IsValid() && outError == VE_OK && !found ; ++it_file)
				{
					if (it_file.Current()->ConformsTo( RIAFileKind::kProjectFileKind))
					{
						VString fileName;
						projectFilePath.GetFileName( fileName, true);
						outError = it_file.Current()->Rename( fileName, NULL);
						found = true;
					}
				}
			}
			ReleaseRefCountable( &projectFile);
		}

		if (outError == VE_OK)
		{
			// Resolve the items names which are referenced inside the project file
			VFile projectFile( projectFilePath);
			if (projectFile.Exists())
			{
				VValueBag projectBag;
				outError = LoadBagFromXML( projectFile, kXML_ELEMENT_PROJECT, projectBag, XML_ValidateNever);
				if (outError == VE_OK)
				{
					sLONG changesCount = 0;
					VBagArray *foldersBagArray = projectBag.RetainElements( kXML_ELEMENT_FOLDER);
					if (foldersBagArray != NULL)
					{
						for (sLONG i = 1 ; i <= foldersBagArray->GetCount() ; i++)
						{
							VValueBag *folderBag = foldersBagArray->GetNth( i);
							if (folderBag != NULL)
							{
								VString relativePath = ProjectItemBagKeys::path.Get( folderBag);
								if (relativePath.Find( kTEMPLATE_PROJECT_NAME_MACRO) != 0)
								{
									relativePath.ExchangeAll( kTEMPLATE_PROJECT_NAME_MACRO, inProjectName);
									ProjectItemBagKeys::path.Set( folderBag, relativePath);
									++changesCount;
								}
							}
						}

						projectBag.SetElements( kXML_ELEMENT_FOLDER, foldersBagArray);
					}

					ReleaseRefCountable( &foldersBagArray);

					VBagArray *filesBagArray = projectBag.RetainElements( kXML_ELEMENT_FILE);
					if (filesBagArray != NULL)
					{
						for (sLONG i = 1 ; i <= filesBagArray->GetCount() ; i++)
						{
							VValueBag *fileBag = filesBagArray->GetNth( i);
							if (fileBag != NULL)
							{
								VString relativePath = ProjectItemBagKeys::path.Get( fileBag);
								if (relativePath.Find( kTEMPLATE_PROJECT_NAME_MACRO) != 0)
								{
									relativePath.ExchangeAll( kTEMPLATE_PROJECT_NAME_MACRO, inProjectName);
									ProjectItemBagKeys::path.Set( fileBag, relativePath);
									++changesCount;
								}
							}
						}

						projectBag.SetElements( kXML_ELEMENT_FILE, filesBagArray);
					}

					ReleaseRefCountable( &filesBagArray);

					if (changesCount > 0)
					{
						outError = WriteBagToFileInXML( projectBag, kXML_ELEMENT_PROJECT, &projectFile);
					}
				}
			}
		}

		if (outError == VE_OK)
		{
			project = VProject::Instantiate( outError, this, projectFilePath);
			if (outError == VE_OK && project != NULL)
			{
				_AddProject( project, true, true);

				project->Load();

				if (fStartupProject == NULL)
				{
					fStartupProject = project;
					_TouchSolutionFile();
				}

				if (saveAction == e_SAVE)
					_SaveSolutionFile();

				// Update the settings file
				VError err = VE_OK;
				VProjectSettings *settings = project->RetainSettings( err);
				if (err == VE_OK && settings != NULL)
				{
					// Exchange the pattern if need
					bool done = false;
					VRIASettingsFile *settingsFile = settings->RetainSettingsFile( RIASettingID::project);
					if (settingsFile != NULL)
					{
						VValueBag *projectSettings = settingsFile->RetainSettings( RIASettingID::project);
						if (projectSettings != NULL)
						{
							VString pattern = RIASettingsKeys::Project::pattern.Get( projectSettings);
							if (pattern == kTEMPLATE_PROJECT_NAME_MACRO)
							{
							#if 0	// sc 25/03/2011 disable project pattern support
								RIASettingsKeys::Project::pattern.Set( projectSettings, inProjectName);
								settingsFile->Save();
								done = true;
							#else
								RIASettingsKeys::Project::pattern.Set( projectSettings, L"");
								settingsFile->Save();
							#endif
							}
						}
						ReleaseRefCountable( &projectSettings);
					}
					ReleaseRefCountable( &settingsFile);

					if (!done)
					{
						// Set the http port
						settingsFile = settings->RetainSettingsFile( RIASettingID::http);
						if (settingsFile != NULL)
						{
							VValueBag *httpSettings = settingsFile->RetainSettings( RIASettingID::http);
							if (httpSettings != NULL)
							{
								RIASettingsKeys::HTTP::port.Set( httpSettings, httpPort);
								settingsFile->Save();
							}
							ReleaseRefCountable( &httpSettings);
						}
						ReleaseRefCountable( &settingsFile);
					}
				}
				ReleaseRefCountable( &settings);
			}
			else
			{
				ReleaseRefCountable( &project);
			}
		}
	}

	return project;
}


VError VSolution::_SaveSolutionFile(bool inForceSave)
{
	VError err = VE_OK;

	if (inForceSave || (fAutoSave && (fSolutionFileDirtyStamp > 0)))
	{
		VProjectItem* solutionFileProjectItem = GetSolutionFileProjectItem();

		if (solutionFileProjectItem != NULL)
		{
			VString strProjectPath;
			VString strRelativeProjectPath;

			VString strSolutionPath;
			fSolutionItem->GetURL().GetPath(strSolutionPath, eURL_POSIX_STYLE, false);

			VValueBag* bagSolution = new VValueBag();

			for ( VProjectItemIterator it( fSolutionItem ); it.IsValid(); ++it )
			{
				if ( ( ! it->IsGhost() ) && ( it->GetKind() == VProjectItem::ePROJECT ) )	
				{
					// ne pas enregistrer des projets qui ne seraient pas definis comme appartenant a la solution !
					if ( ContainsProject( it ) )
					{
						VProject* project = it->GetBehaviour()->GetProject();
						assert( project != NULL );

						if ( project )
						{
							if ( ! _IsItemReferenced( project->GetProjectItem() ) )
								continue;

							VProjectItem* projectItem = project->GetProjectItemProjectFile();
							VFilePath projectFilePath;
							projectItem->GetFilePath( projectFilePath );
							VURL projectFileURL( projectFilePath );
							VString strNetLoc;
							projectFileURL.GetNetworkLocation( strNetLoc, false );
							projectFileURL.GetPath( strProjectPath, eURL_POSIX_STYLE, false );

							if (!strNetLoc.IsEmpty())
								strProjectPath = "//" + strNetLoc + "/" + strProjectPath;
							
							GetPathRelativeToFolderPath(strSolutionPath, strProjectPath, strRelativeProjectPath );

							VValueBag* bagProject = new VValueBag();
							ProjectItemBagKeys::path.Set(bagProject, strRelativeProjectPath );
							ProjectItemBagKeys::startupProject.Set( bagProject, IsStartupProject( project ) );
							bagSolution->AddElement(it->GetXMLElementName(), bagProject );
						}
					}
				}
			}

			// Save referenced items
			for ( VectorOfProjectItems::iterator iter = fReferencedItems.begin() ; iter != fReferencedItems.end() && err == VE_OK ; ++iter )
			{
				VProjectItem *item = *iter;
				if (!item->IsGhost() && (item->GetKind() != VProjectItem::ePROJECT))
				{
					VValueBag *itemBag = new VValueBag();
					if (itemBag != NULL)
					{
						// Resolve the relative path;
						VFilePath projectItemFilePath;
						item->GetFilePath( projectItemFilePath);
						VURL projectItemURL( projectItemFilePath);
						
						VString strNetLoc, strProjectItemPath, strRelativeProjectItemPath;
						projectItemURL.GetNetworkLocation( strNetLoc, false);
						projectItemURL.GetPath( strProjectItemPath, eURL_POSIX_STYLE, false);
						if (!strNetLoc.IsEmpty())
							strProjectItemPath = "//" + strNetLoc + "/" + strProjectItemPath;
						GetPathRelativeToFolderPath( strSolutionPath, strProjectItemPath, strRelativeProjectItemPath);

						// Append the relative path
						ProjectItemBagKeys::path.Set( itemBag, strRelativeProjectItemPath);

						// Append the tags
						std::vector<VProjectItemTag> tags;
						item->GetTags( tags);
						if (!tags.empty())
						{
							VBagArray *tagArray = new VBagArray();
							if (tagArray != NULL)
							{
								for (std::vector<VProjectItemTag>::iterator iter = tags.begin() ; iter != tags.end() && err == VE_OK ; ++iter)
								{
									VValueBag *bag = new VValueBag();
									if (bag != NULL)
									{
										ProjectItemBagKeys::name.Set( bag, *iter);
										tagArray->AddTail( bag);
									}
									else
									{
										err = VE_MEMORY_FULL;
									}
									ReleaseRefCountable( &bag);
								}
								itemBag->SetElements( kXML_ELEMENT_TAG, tagArray);
							}
							else
							{
								err = VE_MEMORY_FULL;
							}
							ReleaseRefCountable( &tagArray);
						}

						if (err == VE_OK)
						{
							VString elementName = item->GetXMLElementName();
							if (testAssert( elementName.GetLength() > 0))
							{
								bagSolution->AddElement( elementName, itemBag);
							}
						}
					}
					else
					{
						err = VE_MEMORY_FULL;
					}
					ReleaseRefCountable( &itemBag);
				}	
			}

			VFile solutionFile(solutionFileProjectItem->GetURL());
			{
				StErrorContextInstaller( false );
				err = WriteBagToFileInXML( *bagSolution, kXML_ELEMENT_SOLUTION, &solutionFile );
			}

			bagSolution->Release();

		}
		if (err == VE_OK)
			fSolutionFileDirtyStamp = 0;
	}

	return err;
}


void VSolution::GetPathRelativeToFolderPath(const VString& inFolderPath, const VString& inFullPath, VString& outRelativePath) const
{
	if (inFolderPath == inFullPath)
	{
		outRelativePath = "./";
	}
	else
	{
		VString baseFolder(inFolderPath);
		outRelativePath = inFullPath;
		sLONG start = 1;
		if (baseFolder.GetUniChar(start) == outRelativePath.GetUniChar(start))
		{
			sLONG baseFolderLength  = baseFolder.GetLength();
			sLONG outRelativePathLength = outRelativePath.GetLength();
			sLONG min = baseFolderLength;
			if (outRelativePathLength < min)
				min = outRelativePathLength;
			while ((start <= min) && (baseFolder.GetUniChar(start) == outRelativePath.GetUniChar(start))) start++;
			sLONG pos = baseFolder.FindUniChar('/', start - 1, true);
			start = pos + 1;
			sLONG nbSeparator = 0;
			for (sLONG j = start; j <= baseFolderLength; j++)
			{
				if (baseFolder.GetUniChar(j) == '/')
					nbSeparator++;
			}
			VString prefix;
			if (baseFolderLength > start)
			{
				for (sLONG j = 0; j < nbSeparator; j++)
				{
					prefix += "..";
					prefix += "/";
				}
			}
			else
			{
				prefix += ".";
				prefix += "/";
			}
			if (start <= outRelativePath.GetLength())
			{
				outRelativePath.SubString(start, outRelativePathLength - start + 1);
				outRelativePath = prefix + outRelativePath;
			}
			else
				outRelativePath = prefix;
		}
	}
}



// ----------------------------------------------------------------------------
// Pattern Singleton - Construction / Initialisation
// ----------------------------------------------------------------------------
VSolutionManager* VSolutionManager::sSolutionManager = NULL;

VSolutionManager::VSolutionManager()
: fSourceControlManager(NULL)
{
}


VSolutionManager::~VSolutionManager()
{
}


bool VSolutionManager::Init()
{
	bool ok = true;
	
	if (sSolutionManager == NULL)
	{
		sSolutionManager = new VSolutionManager();
		ok = (sSolutionManager != NULL);
		
		if (ok) 
			ok = VProjectItemManager::Init();

	#if SOLUTION_PROFILING_ENABLED
		if (ok)
			ok = VSolutionProfiler::Init();
	#endif		
	}
	return ok;
}


void VSolutionManager::DeInit()
{
	VProjectItemManager::DeInit();

#if SOLUTION_PROFILING_ENABLED
	VSolutionProfiler::DeInit();
#endif
		
	delete sSolutionManager; 
	sSolutionManager = NULL;
}


VSolution* VSolutionManager::CreateNewSolution( VSolutionStartupParameters* inSolutionStartupParameters, ISolutionMessageManager* inSolutionMessageManager, ISolutionBreakPointsManager* inSolutionBreakPointsManager, ISolutionUser* inSolutionUser)
{
	VSolution *solution = _CreateSolution( inSolutionStartupParameters, inSolutionMessageManager, inSolutionBreakPointsManager, inSolutionUser);
	if (solution != NULL)
	{
		solution->_TouchSolutionFile();
		solution->GetSolutionItem()->CreatePhysicalChildren( true);
		solution->_SaveSolutionFile();
		if (!solution->Open())	// sc 30/06/2010 Wakanda bug 701
		{
			delete solution;
			solution = NULL;
		}
	}

	return solution;
}


VSolution* VSolutionManager::CreateNewSolution( const VFilePath& inSolutionFilePath, ISolutionMessageManager* inSolutionMessageManager, ISolutionBreakPointsManager* inSolutionBreakPointsManager, ISolutionUser* inSolutionUser)
{
	VSolution *solution = NULL;

	if (inSolutionFilePath.IsValid() && inSolutionFilePath.IsFile())
	{
		VFile *file = new VFile( inSolutionFilePath);
		VSolutionStartupParameters *startupParams = new VSolutionStartupParameters();
		if (startupParams != NULL && file != NULL)
		{
			startupParams->SetSolutionFileToOpen( file);
			solution = CreateNewSolution( startupParams, inSolutionMessageManager, inSolutionBreakPointsManager, inSolutionUser);
		}
		ReleaseRefCountable( &startupParams);
		ReleaseRefCountable( &file);
	}
	return solution;
}


VSolution* VSolutionManager::CreateNewSolutionFromTemplate( const XBOX::VFilePath& inSolutionFilePath, const XBOX::VFolder& inSolutionTemplateFolder, ISolutionMessageManager* inSolutionMessageManager, ISolutionBreakPointsManager* inSolutionBreakPointsManager, ISolutionUser* inSolutionUser)
{
	VSolution *solution = NULL;

	VString solutionName;
	VFilePath solutionFolderPath;
	inSolutionFilePath.GetFileName( solutionName, false);
	inSolutionFilePath.GetFolder( solutionFolderPath);

	VFolder solutionFolder( solutionFolderPath);
	VError err = VSolution::CreateFromTemplate( solutionFolder, solutionName, inSolutionTemplateFolder);
	if (err == VE_OK)
	{
		VFile *file = new VFile( inSolutionFilePath);
		VSolutionStartupParameters *startupParams = new VSolutionStartupParameters();
		if (startupParams != NULL && file != NULL)
		{
			startupParams->SetSolutionFileToOpen( file);
			solution = OpenSolution( startupParams, inSolutionMessageManager, inSolutionBreakPointsManager, inSolutionUser);
		}
		ReleaseRefCountable( &startupParams);
		ReleaseRefCountable( &file);
	}

	return solution;
}


VSolution* VSolutionManager::OpenSolution( VSolutionStartupParameters* inSolutionStartupParameters, ISolutionMessageManager* inSolutionMessageManager, ISolutionBreakPointsManager* inSolutionBreakPointsManager, ISolutionUser* inSolutionUser)
{
	VSolution *solution = _CreateSolution( inSolutionStartupParameters, inSolutionMessageManager, inSolutionBreakPointsManager, inSolutionUser);
	if (solution != NULL)
	{
		if (solution->Open())
		{
			solution->LoadProjects();
		}
		else
		{
			delete solution;
			solution = NULL;
		}
	}

	return solution;
}


VSolution* VSolutionManager::OpenSolution( const VFilePath& inSolutionFilePath, ISolutionMessageManager* inSolutionMessageManager, ISolutionBreakPointsManager* inSolutionBreakPointsManager, ISolutionUser* inSolutionUser)
{
	VSolution *solution = NULL;

	if (inSolutionFilePath.IsValid() && inSolutionFilePath.IsFile())
	{
		VFile *file = new VFile( inSolutionFilePath);
		VSolutionStartupParameters *startupParams = new VSolutionStartupParameters();
		if (startupParams != NULL && file != NULL)
		{
			startupParams->SetSolutionFileToOpen( file);
			solution = OpenSolution( startupParams, inSolutionMessageManager, inSolutionBreakPointsManager, inSolutionUser);
		}
		ReleaseRefCountable( &startupParams);
		ReleaseRefCountable( &file);
	}
	return solution;
}


bool VSolutionManager::CloseSolution( VSolution *inSolution)
{
	bool ok = true;

	if (inSolution != NULL)
	{
		inSolution->StopWatchingFileSystem();
		inSolution->StopUpdatingSymbolTable();
		inSolution->UnloadProjects();
		inSolution->Close();

		fprintf ( stdout, "\nSolution closed\n\n" );
	}
	return ok;
}


void VSolutionManager::SetSourceControlManager( ISourceControlManager* inSourceControlManager)
{
	fSourceControlManager = inSourceControlManager;
}


VSolution* VSolutionManager::_CreateSolution( VSolutionStartupParameters* inSolutionStartupParameters, ISolutionMessageManager* inSolutionMessageManager, ISolutionBreakPointsManager* inSolutionBreakPointsManager, ISolutionUser* inSolutionUser)
{
	VSolution *solution = NULL;

	if (inSolutionStartupParameters != NULL && inSolutionStartupParameters->GetSolutionFileToOpen() != NULL)
	{
		VError err = VE_OK;
		VFilePath solutionFilePath;
		inSolutionStartupParameters->GetSolutionFileToOpen()->GetPath( solutionFilePath);

		solution = VSolution::Instantiate( err, solutionFilePath);
		if (solution != NULL)
		{
			solution->SetSolutionStartupParameters( inSolutionStartupParameters);
			solution->SetSolutionMessageManager( inSolutionMessageManager);
			solution->SetSolutionUser( inSolutionUser);
			solution->SetBreakPointsManager( inSolutionBreakPointsManager);
		}
	}
		
	return solution;
}


#define PRODUCT_NAME			"Wakanda Server"
#define RUNNING_FILE_SUBFOLDER	"runningServers"
#define RUNNING_FILE_PREFIX		"wakandaServer"

XBOX::VFile *VSolution::RetainRunningServerFile()
{
	VFile *file = NULL;

	VFolder *folder = RetainRunningServerFilesFolder();
	if (folder != NULL)
	{
		VString fileName( RUNNING_FILE_PREFIX);
		fileName.AppendLong( VProcess::Get()->GetSystemID());
		file = new VFile( *folder, fileName);
	}
	ReleaseRefCountable( &folder);
	
	return file;
}


XBOX::VFolder *VSolution::RetainRunningServerFilesFolder( bool inAllUsers)
{
	VFolder *runningServersFolder = NULL;

	VFolder *parentFolder = VFolder::RetainSystemFolder( inAllUsers ? eFK_CommonApplicationData : eFK_UserApplicationData, true);
	if (parentFolder != NULL)
	{
		VFilePath path( parentFolder->GetPath());
		path.ToSubFolder( PRODUCT_NAME).ToSubFolder( RUNNING_FILE_SUBFOLDER);
		
		runningServersFolder = new VFolder( path);
		if (!runningServersFolder->Exists())
		{
			// try to create it silently
			StErrorContextInstaller context( false);
			runningServersFolder->CreateRecursive();
		}
	}
	ReleaseRefCountable( &parentFolder);

	if ( (runningServersFolder != NULL) && !runningServersFolder->Exists())
		ReleaseRefCountable( &runningServersFolder);
	
	return runningServersFolder;
}


bool VSolution::GetPidFromRunningServerFile (const XBOX::VString &inFileName, uLONG *outPid)
{
	XBOX::VString	prefix, pidString;

	prefix = RUNNING_FILE_PREFIX;
	if (!inFileName.BeginsWith(prefix)) 
		
		return false;
	
	else {
		
		inFileName.GetSubString(1 + prefix.GetLength(), inFileName.GetLength() - prefix.GetLength(), pidString);
		*outPid = pidString.GetLong();
		return true;
	
	}
}




void VSolutionTools::RemoveUselessPaths( std::vector<VFilePath>& ioPaths)
{
	std::vector<VFilePath> lPaths( ioPaths.begin(), ioPaths.end()), ancestorPaths;

	for (std::vector<VFilePath>::iterator pathIter = lPaths.begin() ; pathIter != lPaths.end() ; ++pathIter)
	{
		if ((*pathIter).IsFolder())
			ancestorPaths.push_back( *pathIter);
	}

	ioPaths.clear();

	for (std::vector<VFilePath>::iterator pathIter = lPaths.begin() ; pathIter != lPaths.end() ; ++pathIter)
	{
		bool hasAncestor = false;

		for (std::vector<VFilePath>::iterator ancestorIter = ancestorPaths.begin() ; (ancestorIter != ancestorPaths.end()) && !hasAncestor ; ++ancestorIter)
			hasAncestor = (*pathIter).IsChildOf( *ancestorIter);
		
		if (!hasAncestor)
			ioPaths.push_back( *pathIter);
	}
}


void VSolutionTools::GetItemsByProject( const VectorOfProjectItems& inItems, std::map< VProject*, VectorOfProjectItems >& outItemsByProject)
{
	for (VectorOfProjectItemsConstIterator itemIter = inItems.begin() ; itemIter != inItems.end() ; ++itemIter)
	{
		VProject *project = (*itemIter)->GetProjectOwner();
		if (project != NULL)
		{
			std::map< VProject*, VectorOfProjectItems >::iterator found = outItemsByProject.find( project);
			if (found == outItemsByProject.end())
				outItemsByProject[project] = VectorOfProjectItems( 1, *itemIter);
			else
				found->second.push_back( *itemIter);
		}
	}
}



XBOX::VError VSolutionTools::CopyFile( const XBOX::VFolder& inDestinationFolder, const XBOX::VFilePath& inFilePath, XBOX::VFilePath *outNewFilePath)
{
	if (!inDestinationFolder.Exists())
		return VE_FOLDER_NOT_FOUND;

	VFile sourceFile( inFilePath);
	if (!sourceFile.Exists())
		return VE_FILE_NOT_FOUND;
	
	VError err = VE_OK;
	VIndex cpt = 0;

	VString fileName, fileNameNoExt;
	inFilePath.GetFileName( fileName, true);
	inFilePath.GetFileName( fileNameNoExt, false);

	VFilePath newPath;
	inDestinationFolder.GetPath( newPath);
	newPath.SetFileName( fileName);

	VFile newFile( newPath);
	bool exists =  newFile.Exists();
	while (exists)
	{
		++cpt;
		VString name( fileNameNoExt);
		name.AppendString( L" (");
		name.AppendLong( cpt);
		name.AppendUniChar( ')');
		newPath.SetFileName( name, false);
		VFile otherFile( newPath);
		exists = otherFile.Exists();
	}

	if (outNewFilePath != NULL)
		*outNewFilePath = newPath;

	err = sourceFile.CopyTo( newPath, NULL);

	return err;
}


XBOX::VError VSolutionTools::MoveFile( const XBOX::VFolder& inDestinationFolder, const XBOX::VFilePath& inFilePath, XBOX::VFilePath *outNewFilePath)
{
	if (!inDestinationFolder.Exists())
		return VE_FOLDER_NOT_FOUND;

	VFile sourceFile( inFilePath);
	if (!sourceFile.Exists())
		return VE_FILE_NOT_FOUND;
	
	VError err = VE_OK;

	VString fileName;
	inFilePath.GetFileName( fileName, true);

	VFilePath newPath;
	inDestinationFolder.GetPath( newPath);
	newPath.SetFileName( fileName);

	VFile newFile( newPath);
	if (!newFile.Exists())
	{
		err = sourceFile.CopyTo( newPath, NULL);
		if (err == VE_OK)
		{
			err = sourceFile.Delete();
		}
	}
	
	if (outNewFilePath != NULL)
		*outNewFilePath = newPath;

	return err;
}


XBOX::VError VSolutionTools::CopyFolder( const XBOX::VFolder& inDestinationFolder, const XBOX::VFilePath& inFolderPath, XBOX::VFilePath *outNewFolderPath)
{
	if (!inDestinationFolder.Exists())
		return VE_FOLDER_NOT_FOUND;

	VFolder sourceFolder( inFolderPath);
	if (!sourceFolder.Exists())
		return VE_FOLDER_NOT_FOUND;

	if ((inDestinationFolder.GetPath() == inFolderPath) || inDestinationFolder.GetPath().IsChildOf( inFolderPath))
		return VE_SOMA_DEST_FOLDER_IS_SUBFOLDER_OF_SRC_FOLDER;

	VError err = VE_OK;
	VIndex cpt = 0;

	VString folderName, folderNameNoExt;
	inFolderPath.GetFolderName( folderName, true);
	inFolderPath.GetFolderName( folderNameNoExt, false);

	VFilePath newPath;
	inDestinationFolder.GetPath( newPath);
	newPath.ToSubFolder( folderName);

	VFolder newFolder( newPath);
	bool exists =  newFolder.Exists();
	while (exists)
	{
		++cpt;
		VString name( folderNameNoExt);
		name.AppendString( L" (");
		name.AppendLong( cpt);
		name.AppendUniChar( ')');
		newPath.SetFolderName( name, false);
		VFolder otherFolder( newPath);
		exists = otherFolder.Exists();
	}

	if (outNewFolderPath != NULL)
		*outNewFolderPath = newPath;

	err = sourceFolder.CopyContentsTo( VFolder( newPath));

	return err;
}


XBOX::VError VSolutionTools::MoveFolder( const XBOX::VFolder& inDestinationFolder, const XBOX::VFilePath& inFolderPath, XBOX::VFilePath *outNewFolderPath)
{
	if (!inDestinationFolder.Exists())
		return VE_FOLDER_NOT_FOUND;

	VFolder sourceFolder( inFolderPath);
	if (!sourceFolder.Exists())
		return VE_FOLDER_NOT_FOUND;

	if ((inDestinationFolder.GetPath() == inFolderPath) || inDestinationFolder.GetPath().IsChildOf( inFolderPath))
		return VE_SOMA_DEST_FOLDER_IS_SUBFOLDER_OF_SRC_FOLDER;

	VError err = VE_OK;

	VString folderName;
	inFolderPath.GetFolderName( folderName, true);

	VFilePath newPath;
	inDestinationFolder.GetPath( newPath);
	newPath.ToSubFolder( folderName);

	VFolder newFolder( newPath);
	if (!newFolder.Exists())
	{
		err = sourceFolder.CopyContentsTo( newFolder);
		if (err == VE_OK)
		{
			err = sourceFolder.Delete( true);
		}
	}

	if (outNewFolderPath != NULL)
		*outNewFolderPath = newPath;

	return err;
}



// ----------------------------------------------------------------------------
// Profiling utilities

class VProfilerCallTimeCounter : public VObject
{
public:
			VProfilerCallTimeCounter() : fStarted(0), fDuration(0) {;}

			virtual ~VProfilerCallTimeCounter() { xbox_assert(fStarted == 0); }

			sLONG8 GetDuration() const { return fDuration; }

			void Start()
			{
				if (fStarted == 0)
					fCounter.Start();
				++fStarted;
			}

			void Stop()
			{
				--fStarted;
				if (fStarted == 0)
					fDuration += fCounter.Stop();
			}

			bool IsStarted() const { return fStarted > 0; }

private:
			uLONG8					fStarted;
			VMicrosecondsCounter	fCounter;
			sLONG8					fDuration;
};



class VProfilerCall;

typedef std::map< uLONG , VProfilerCall* >					MapOfProfilerCall;
typedef std::map< uLONG , VProfilerCall* >::iterator		MapOfProfilerCall_iter;
typedef std::map< uLONG , VProfilerCall* >::const_iterator	MapOfProfilerCall_citer;

class VProfilerCall : public VObject
{
public:
			VProfilerCall( uLONG inID, VProfilerCall *inParent)
			: fID(inID), fParent(inParent), fEntered(0), fCallsCount(0)
			{
			}

			virtual ~VProfilerCall()
			{
				xbox_assert(fEntered == 0);

				for (MapOfProfilerCall_iter iter = fChildren.begin() ; iter != fChildren.end() ; ++iter)
					delete iter->second;
			}

			uLONG GetID() const { return fID; }

			VProfilerCall* GetParent() const { return fParent; }

			uLONG8 GetCallsCount() const { return fCallsCount; }

			const VProfilerCallTimeCounter& GetTimeCounter() const { return fTimeCounter; }

			const MapOfProfilerCall& GetChildren() const { return fChildren; }

			bool IsEntered() const { return  fEntered > 0; }

			void Enter()
			{
				++fEntered;
				++fCallsCount;
				fTimeCounter.Start();
			}

			void Exit()
			{
				--fEntered;
				fTimeCounter.Stop();
			}

			VProfilerCall* CreateChild( uLONG inID)
			{
				VProfilerCall *result = new VProfilerCall( inID, this);
				if (result != NULL)
					fChildren[inID] = result;

				return result;
			}

			VProfilerCall* FindChild( uLONG inID)
			{
				MapOfProfilerCall_iter found = fChildren.find( inID);
				if (found != fChildren.end())
					return found->second;

				return NULL;
			}

private:
			uLONG						fID;
			VProfilerCall				*fParent;
			MapOfProfilerCall			fChildren;
			uLONG8						fEntered;
			VProfilerCallTimeCounter	fTimeCounter;
			uLONG8						fCallsCount;
};



class VProfilerCallDesc : public VObject
{
public:
	VProfilerCallDesc() {;}
	virtual ~VProfilerCallDesc() {;}

	VString		fClassName;
	VString		fMethodName;
};



VSolutionProfiler* VSolutionProfiler::sProfiler = NULL;

VSolutionProfiler::VSolutionProfiler()
: fProfilStarted( false)
{
}


VSolutionProfiler::~VSolutionProfiler()
{
	xbox_assert(!fProfilStarted);

	for (MapOfProfilerCallDesc_iter iter = fCallsDescription.begin() ; iter != fCallsDescription.end() ; ++iter)
		delete iter->second;
}


bool VSolutionProfiler::Init()
{
	if (sProfiler == NULL)
	{
		sProfiler = new VSolutionProfiler();
		if (sProfiler != NULL)
		{
			sProfiler->SetCallDescription( PROFILER_CALL_VProject_SynchronizeWithFileSystem, L"VProject", L"SynchronizeWithFileSystem");
			sProfiler->SetCallDescription( PROFILER_CALL_VProjectItem_ConformsTo, L"VProjectItem", L"ConformsTo");
			sProfiler->SetCallDescription( PROFILER_CALL_VSolutionExplorerDialog__LoadStandardListBox, L"VSolutionExplorerDialog", L"_LoadStandardListBox");
			sProfiler->SetCallDescription( PROFILER_CALL_ISolutionDelegate_Synchronize, L"ISolutionDelegate", L"Synchronize");
			sProfiler->SetCallDescription( PROFILER_CALL_VProject_GetProjectItemFromFullPath, L"VProject", L"GetProjectItemFromFullPath");
			sProfiler->SetCallDescription( PROFILER_CALL_VProjectItem_BuildFullPath, L"VProjectItem", L"BuildFullPath");
			sProfiler->SetCallDescription( PROFILER_CALL_VProject__SynchronizeWithFileSystem, L"VProject", L"_SynchronizeWithFileSystem");
			sProfiler->SetCallDescription( PROFILER_CALL_VProject_RegisterProjectItemInMapOfFullPathes, L"VProject", L"RegisterProjectItemInMapOfFullPathes");
			sProfiler->SetCallDescription( PROFILER_CALL_VFile_ConformsTo, L"VFile", L"ConformsTo");
			sProfiler->SetCallDescription( PROFILER_CALL_VSolutionExplorerDialog__MakeViewFromTreeSolution, L"VSolutionExplorerDialog", L"MakeViewFromTreeSolution");
			sProfiler->SetCallDescription( PROFILER_CALL_VSolutionExplorerDialog__SetTreemItemPropertiesFromProjectItem, L"VSolutionExplorerDialog", L"_SetTreemItemPropertiesFromProjectItem");
			sProfiler->SetCallDescription( PROFILER_CALL_VProjectItem_FindChildByRelativePath, L"VProjectItem", L"FindChildByRelativePath");

			return true;
		}
	}
	return false;
}


void VSolutionProfiler::DeInit()
{
	delete sProfiler;
	sProfiler = NULL;
}


bool VSolutionProfiler::StartProfiling( const XBOX::VString& inName, const XBOX::VFilePath& inLogFolderPath, const XBOX::VString& inSolutionName)
{
	bool result = false;

	if (testAssert(!fProfilStarted))
	{
		fProfilStarted = true;
		fProfilName = inName;
		fLogFolderPath = inLogFolderPath;
		fSolutionName = inSolutionName;

		VProfilerCall *rootCall = new VProfilerCall( 0, NULL);
		if (rootCall != NULL)
		{
			fCallStack.push_back( rootCall);
			rootCall->Enter();
			result = true;
		}
	}
	return result;
}


void VSolutionProfiler::EndProfiling()
{
	if (testAssert(fProfilStarted))
	{
		if (testAssert(fCallStack.size() == 1))
		{
			VProfilerCall *rootCall = fCallStack.back();

			rootCall->Exit();

			VString name;
		#if VERSIONDEBUG 
			name = fSolutionName + L"_debug_profil_";
		#else
			name = fSolutionName + L"_release_profil_";
		#endif

			VFolder logFolder( fLogFolderPath);
			VSplitableLogFile logFile( logFolder, name);
			if (logFile.Open( false))
			{
				StStringConverter<char> stringConverter( VTC_StdLib_char);

				logFile.AppendString( "===============================================================================\n");

				VString msg = L"Profiling report for \"" + fProfilName + L"\"\n";
				logFile.AppendString( stringConverter.ConvertString( msg));

				VTime now;
				VTime::Now( now);
				VString nowString;
				now.FormatByOS( nowString, false);
				msg = "Profil start at " + nowString + L", ";
				msg += "profil duration: ";
				msg.AppendLong8( rootCall->GetTimeCounter().GetDuration() / 1000);
				msg += L" ms (";
				msg.AppendLong8( rootCall->GetTimeCounter().GetDuration());
				msg += L" us).\n";
				logFile.AppendString( stringConverter.ConvertString( msg));

				for (MapOfProfilerCall_citer iter = rootCall->GetChildren().begin() ; iter != rootCall->GetChildren().end() ; ++iter)
					_LogCall( &logFile, iter->second, 0, true);

				logFile.Flush();
				logFile.Close();
			}

			fCallStack.clear();
			delete rootCall;
		}

		fProfilStarted = false;
	}
}


void VSolutionProfiler::SetCallDescription( uLONG inID, const XBOX::VString& inClassName, const XBOX::VString& inMethodName)
{
	if (fCallsDescription.find( inID) == fCallsDescription.end())
	{
		VProfilerCallDesc *desc = new VProfilerCallDesc();
		if (desc != NULL)
		{
			desc->fClassName = inClassName;
			desc->fMethodName = inMethodName;
			fCallsDescription[inID] = desc;
		}
	}
}


bool VSolutionProfiler::EnterCall( uLONG inID)
{
	bool result = false;

	if (fProfilStarted)
	{
		if (testAssert(!fCallStack.empty()))
		{
			VProfilerCall *currentCall = fCallStack.back();
			VProfilerCall *nextCall = NULL;

			for (std::deque< VProfilerCall* >::reverse_iterator iter = fCallStack.rbegin() ; (iter != fCallStack.rend()) && (nextCall == NULL) ; ++iter)
			{
				if ((*iter)->GetID() == inID)
					nextCall = *iter;
			}

			if (nextCall == NULL)
				nextCall = currentCall->FindChild( inID);

			if (nextCall == NULL)
				nextCall = currentCall->CreateChild( inID);

			if (testAssert(nextCall != NULL))
			{
				fCallStack.push_back( nextCall);
				nextCall->Enter();
				result = true;
			}
		}
	}
	return result;
}


void VSolutionProfiler::ExitCall( uLONG inID)
{
	if (fProfilStarted)
	{
		if (testAssert(!fCallStack.empty() && (fCallStack.back()->GetID() == inID)))
		{
			fCallStack.back()->Exit();
			fCallStack.pop_back();
		}
	}
}


bool VSolutionProfiler::VFile_ConformsTo( XBOX::VFile *inFile, const XBOX::VString& inFileKindID)
{
	bool result;
	EnterCall( PROFILER_CALL_VFile_ConformsTo);
	result = inFile->ConformsTo( inFileKindID);
	ExitCall( PROFILER_CALL_VFile_ConformsTo);
	return result;
}


void VSolutionProfiler::_LogCall( XBOX::VSplitableLogFile *inLogFile, VProfilerCall *inCall, sLONG inIndentationCount, bool inRecursive) const
{
	VString msg;
	StStringConverter<char> stringConverter( VTC_StdLib_char);

	xbox_assert(!inCall->IsEntered());
	xbox_assert(!inCall->GetTimeCounter().IsStarted());
	
	for (sLONG i = 0 ; i < inIndentationCount ; ++i)
		msg += L"     ";

	VProfilerCallDesc *desc = _GetCallDescription( inCall->GetID());
	if (desc != NULL)
		msg += desc->fClassName +L"::" + desc->fMethodName;
	else
		msg += "Undefined call";

	msg += " duration: ";
	msg.AppendLong8( inCall->GetTimeCounter().GetDuration() / 1000);
	msg += L" ms (";
	msg.AppendLong8( inCall->GetTimeCounter().GetDuration());
	msg += L" us)";

	msg += " calls count: ";
	msg.AppendULong8( inCall->GetCallsCount());
	msg += L"\n";

	inLogFile->AppendString( stringConverter.ConvertString( msg));

	if (inRecursive)
	{
		++inIndentationCount;

		for (MapOfProfilerCall_citer iter = inCall->GetChildren().begin() ; iter != inCall->GetChildren().end() ; ++iter)
			_LogCall( inLogFile, iter->second, inIndentationCount, true);
	}
}


VProfilerCallDesc* VSolutionProfiler::_GetCallDescription( uLONG inID) const
{
	VProfilerCallDesc *desc = NULL;

	MapOfProfilerCallDesc_citer found = fCallsDescription.find( inID);
	if (found != fCallsDescription.end())
		desc = found->second;

	return desc;
}



VSolutionProfilerTester::VSolutionProfilerTester()
{
	VSolutionProfiler::Get()->SetCallDescription( 10001, L"VSolutionProfilerTester", L"_BasicMethod1");
	VSolutionProfiler::Get()->SetCallDescription( 10002, L"VSolutionProfilerTester", L"_BasicMethod2");
	VSolutionProfiler::Get()->SetCallDescription( 10003, L"VSolutionProfilerTester", L"_BasicMethod3");
	VSolutionProfiler::Get()->SetCallDescription( 10004, L"VSolutionProfilerTester", L"_BasicMethod4");
	VSolutionProfiler::Get()->SetCallDescription( 10011, L"VSolutionProfilerTester", L"_RecursiveMethod1");
	VSolutionProfiler::Get()->SetCallDescription( 10012, L"VSolutionProfilerTester", L"_RecursiveMethod2");
	VSolutionProfiler::Get()->SetCallDescription( 10013, L"VSolutionProfilerTester", L"_RecursiveMethod3");
}


VSolutionProfilerTester::~VSolutionProfilerTester()
{
}


void VSolutionProfilerTester::Run()
{
	for (sLONG i = 0 ; i < 10 ; ++i)
		_BasicMethod1();

	for (sLONG i = 0 ; i < 10 ; ++i)
		_BasicMethod2();

	for (sLONG i = 0 ; i < 10 ; ++i)
		_BasicMethod3();

	_RecursiveMethod1( 10);
	_RecursiveMethod2( 10);

}


void VSolutionProfilerTester::_BasicMethod1()
{
	VSolutionProfiler::Get()->EnterCall( 10001);

	_BasicMethod2();

	VSolutionProfiler::Get()->ExitCall( 10001);
}


void VSolutionProfilerTester::_BasicMethod2()
{
	VSolutionProfiler::Get()->EnterCall( 10002);
	
	_BasicMethod3();

	VSolutionProfiler::Get()->ExitCall( 10002);
}


void VSolutionProfilerTester::_BasicMethod3()
{
	VSolutionProfiler::Get()->EnterCall( 10003);

	VTask::GetCurrent()->Sleep( 10);

	VSolutionProfiler::Get()->ExitCall( 10003);
}


void VSolutionProfilerTester::_BasicMethod4()
{
	VSolutionProfiler::Get()->EnterCall( 10004);

	VTask::GetCurrent()->Sleep( 10);

	VSolutionProfiler::Get()->ExitCall( 10004);
}


void VSolutionProfilerTester::_RecursiveMethod1( sLONG inCallsCount)
{
	VSolutionProfiler::Get()->EnterCall( 10011);

	_BasicMethod4();

	--inCallsCount;
	if (inCallsCount > 0)
		_RecursiveMethod1(inCallsCount);

	VSolutionProfiler::Get()->ExitCall( 10011);
}


void VSolutionProfilerTester::_RecursiveMethod2( sLONG inCallsCount)
{
	VSolutionProfiler::Get()->EnterCall( 10012);

	_BasicMethod4();

	--inCallsCount;
	if (inCallsCount > 0)
		_RecursiveMethod3(inCallsCount);

	VSolutionProfiler::Get()->ExitCall( 10012);
}


void VSolutionProfilerTester::_RecursiveMethod3( sLONG inCallsCount)
{
	VSolutionProfiler::Get()->EnterCall( 10013);

	_BasicMethod4();

	--inCallsCount;
	if (inCallsCount > 0)
		_RecursiveMethod2(inCallsCount);

	VSolutionProfiler::Get()->ExitCall( 10013);
}