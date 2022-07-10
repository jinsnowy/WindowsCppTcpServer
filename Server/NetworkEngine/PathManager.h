#pragma once

class PathManager : public ISingleton<PathManager>
{
	friend class ISingleton<PathManager>;
private:
	std::string mExecutableFolderPath;

private:
	PathManager()
	{
		CHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		mExecutableFolderPath = buffer;
		mExecutableFolderPath = mExecutableFolderPath.substr(0, mExecutableFolderPath.find_last_of('\\'));
	}

public:
	const std::string& getExecutableDirPath() const { return mExecutableFolderPath; }
};