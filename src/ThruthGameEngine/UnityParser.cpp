#include "UnityParser.h"
#include <fstream>


Truth::UnityParser::UnityParser()
{
	m_ignore.insert(".vs");
	m_ignore.insert("Library");
	m_ignore.insert("Logs");
	m_ignore.insert("obj");
	m_ignore.insert("Packages");
	m_ignore.insert("ProjectSettings");
	m_ignore.insert("UserSettings");
}

Truth::UnityParser::~UnityParser()
{

}

/// <summary>
/// unity 파일을 파싱해서 필요한 정보만 가져오기
/// </summary>
/// <param name="_path">유니티 프로젝트의 루트 디렉토리</param>
void Truth::UnityParser::Parsing(std::string _path)
{
	fs::path root(_path);

	if (!fs::exists(root))
	{
		assert(false && "Wrong File or Directory Path");
		return;
	}
	if (fs::is_directory(root))
	{
		ParsingDir(root);
	}
	else if (fs::is_regular_file(root))
	{
		ParsingFile(root);
	}
}

void Truth::UnityParser::ParsingFile(fs::path& _path)
{
	if (!fs::is_regular_file(_path))
	{
		assert(false && "Not File Path");
		return;
	}

	std::string extnName = _path.extension().generic_string();
	if (extnName != ".meta")
	{
		return;
	}

	std::ifstream fin(_path);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Meta File");
	}

	std::string line;
	while (fin)
	{
		getline(fin, line);

		StringConverter::DeleteAlpha(line, ' ');
		std::vector<std::string> sLine = StringConverter::split(line, ':');
		if (!sLine.empty() && sLine[0] == "guid")
		{
			UnityFormat uf
			{
				sLine[1],
				_path,
				_path.replace_extension()
			};

			m_fileMap[uf.m_guid] = uf;

			break;
		}
	}
}

void Truth::UnityParser::ParsingDir(fs::path& _path)
{
	if (!fs::is_directory(_path))
	{
		assert(false && "Not Directory Path");
		return;
	}

	if (m_ignore.find(_path.filename().generic_string()) != m_ignore.end())
	{
		return;
	}

	std::queue<fs::path> dirQueue;
	std::queue<fs::path> fileQueue;

	fs::directory_iterator itr(_path);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			dirQueue.push(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			fileQueue.push(childPath);
		}
		itr++;
	}

	while (!fileQueue.empty())
	{
		ParsingFile(fileQueue.front());
		fileQueue.pop();
	}

	while (!dirQueue.empty())
	{
		ParsingDir(dirQueue.front());
		dirQueue.pop();
	}
}

void Truth::UnityParser::IntegrateUnityFile()
{

}
