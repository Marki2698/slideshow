// wall.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <iterator>
#include <regex>

const std::string FILE_OF_PATHES = "pathes.txt";

template <bool vectorOnly = true> //maybe remove template
void addPathesToVectorOrPathFile(std::vector<std::string>& pathVec, std::string pathOfDir) {
	DIR* dir;
	struct dirent* ent;
	std::fstream file;

	std::regex regEx("(.+)\.([a-zA-Z0-9]+)");


	if ((dir = opendir(pathOfDir.c_str())) != NULL) {

		if (!vectorOnly) file.open(FILE_OF_PATHES, std::fstream::in | std::fstream::out | std::fstream::app);

		while ((ent = readdir(dir)) != NULL) {

			if (std::regex_match(std::string(ent->d_name), regEx)) {
				if (!vectorOnly) {
					file << pathOfDir << ent->d_name << std::endl;
				}
				pathVec.push_back(pathOfDir + std::string(ent->d_name));
			}
		}

		if (!vectorOnly) file.close();
	}
	else {
		std::cout << "error while opening file" << std::endl;
	}
}


void getPathesFromFile(std::vector<std::string>& pathesVec) {
	std::fstream file;
	file.open(FILE_OF_PATHES, std::fstream::in | std::fstream::out | std::fstream::app);
	if (file.is_open()) {
		std::string path;
		while (std::getline(file, path)) {
			pathesVec.push_back(path);
		}
		file.close();
	}
}

int main()
{
	std::vector<std::string> pathes = {}; // OK
	getPathesFromFile(pathes);

	std::string pathDir = "";
	if (pathes.empty()) {
		std::cout << " do you want to add dir with images ? (yes, no)";
		std::string answer = "";
		std::cin >> answer;
		std::cout << std::endl;
		if (answer == "yes") {
			std::cout << "Please enter path to desired dir (dir should ends with \ symbol): ";
			std::cin.ignore();
			std::getline(std::cin, pathDir);
			addPathesToVectorOrPathFile<false>(pathes, pathDir);
		}
		else {
			std::cout << "goodbye :)" << std::endl;
			exit(0);
		}
	}
	

	bool slideShow = pathes.size() > 1 ? true : false;
	short increment = 0;

	while (true) {
		std::wstring wstr = std::wstring(pathes[increment].cbegin(), pathes[increment].cend());
		if (slideShow) {
			SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)wstr.c_str(), SPIF_UPDATEINIFILE);
			
			if (increment == pathes.size() - 1) increment = 0;
			else ++increment;
		}
		else {
			SystemParametersInfo(SPI_SETDESKWALLPAPER, 1, (void*)wstr.c_str(), SPIF_UPDATEINIFILE);
		}		

		std::this_thread::sleep_for(std::chrono::minutes(5));
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
