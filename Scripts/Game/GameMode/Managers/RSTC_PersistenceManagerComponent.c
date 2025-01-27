[ComponentEditorProps(category: "Persistence", description: "Must be attached to the gamemode entity to setup the persistence system.")]
class RSTC_PersistenceManagerComponentClass : EPF_PersistenceManagerComponentClass
{
}

class RSTC_PersistenceManagerComponent : EPF_PersistenceManagerComponent
{
	const string DB_BASE_DIR = "$profile:/.db/Resistance";
	
	protected World m_World;
	
	void SaveGame()
	{
		if (m_pPersistenceManager)
			m_pPersistenceManager.AutoSave();
		
	}
	
	bool HasSaveGame()
	{
		return FileIO.FileExists(DB_BASE_DIR + "/Resistances");
	}
	
	void WipeSave()
	{
		FileIO.FindFiles(DeleteFileCallback, DB_BASE_DIR, ".json");
		FileIO.FindFiles(DeleteFileCallback, DB_BASE_DIR, ".bin");
		FileIO.FindFiles(DeleteFileCallback, DB_BASE_DIR, string.Empty);
		FileIO.DeleteFile(DB_BASE_DIR);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteFileCallback(string path, FileAttribute attributes)
	{
		FileIO.DeleteFile(path);
	}
}