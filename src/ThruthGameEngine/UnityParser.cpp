#include "UnityParser.h"
#include <fstream>


Truth::UnityParser::UnityParser()
{

}

Truth::UnityParser::~UnityParser()
{

}

/// <summary>
/// unity ������ �Ľ��ؼ� �ʿ��� ������ ��������
/// </summary>
/// <param name="_path">����Ƽ ������Ʈ�� ��Ʈ ���丮</param>
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

}

void Truth::UnityParser::ParsingDir(fs::path& _path)
{
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


}

void Truth::UnityParser::IntegrateUnityFile()
{

}
