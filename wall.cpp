// wall.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <fileapi.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <iterator>
#include <regex>

const bool GET_KEY_FOR_REMOVE = true;
const bool CREATE_AND_GET_MODE = true;
const bool GET_MODE_ONLY = false;
const std::string FILE_OF_PATHES = "pathes.txt";
const wchar_t* APP_NAME = L"WallpaperTracker";
const wchar_t* DELETE_APP_REG_PATH = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Wallpapers\\";
const wchar_t* COMMAND_PATH = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

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

std::wstring getFullPathOfFile() { // OK
	wchar_t buffer[255];
	DWORD bufferLength = 255; 
	GetFullPathName(L"wall.exe", bufferLength, buffer, NULL);
	std::wstring wstr(buffer);
	return wstr;
}

template<bool fullMode = true>
HKEY createAndGetKey() {
	HKEY hKey;

	//const wchar_t* path = COMMAND_PATH;
	LONG result = -1;
	result = RegOpenKeyEx(HKEY_CURRENT_USER, COMMAND_PATH, 0, KEY_READ, &hKey);
	if (result != ERROR_SUCCESS) {
		if (result == ERROR_FILE_NOT_FOUND) {
			std::cout << "no such key" << std::endl;
			if (fullMode) {
				result = RegCreateKey(HKEY_CURRENT_USER, COMMAND_PATH, &hKey);
				if (result == ERROR_SUCCESS) {
					std::cout << "xcbvb" << std::endl;
				}
			}
		}
	}
	else {
		std::cout << "key opened" << std::endl;
	}

	return hKey;
}


//LONG getAutoStartStatus() {
//	std::wstring absPath = getFullPathOfFile();
//	HKEY hKey = createAndGetKey();
//
//	LONG lnRes = 0;
//	lnRes = RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (BYTE*)absPath.c_str(), (absPath.size() + 1) / sizeof(wchar_t));
//	RegCloseKey(hKey);
//	if (hKey) {
//		lnRes = RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (BYTE*)absPath.c_str(), (absPath.size() + 1) / sizeof(wchar_t));
//		RegCloseKey(hKey);
//	}
//	else {
//		std::cout << "something went wrong" << std::endl;
//	}
//
//	return lnRes;
//}

void setAutoStart() {
	std::wstring absPath = getFullPathOfFile();
	HKEY hKey = createAndGetKey();

	LONG lnRes = 0;

	if (hKey) {
		lnRes = RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (BYTE*)absPath.c_str(), (absPath.size() + 1) / sizeof(wchar_t));
		if (lnRes == ERROR_SUCCESS) std::cout << "added to startup" << std::endl;
		RegCloseKey(hKey);
	}
	else {
		std::cout << "something went wrong" << std::endl;
	}
}

void removeAutoStart() { // change it 
	HKEY hKey = createAndGetKey<GET_MODE_ONLY>();
	LONG res = RegDeleteValue(hKey, APP_NAME);
	if (res == ERROR_SUCCESS) {
		std::cout << "removed" << std::endl;
	}
	else {
		std::cout << "something went wrong" << std::endl;
	}
}

int main()
{
	std::vector<std::string> pathes = {}; // OK
	getPathesFromFile(pathes);

	std::string pathDir = "";
	if (pathes.empty()) {
		std::cout << " do you want to add dir with images ? (yes, no) ";
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

	HKEY hKey = createAndGetKey<GET_MODE_ONLY>();

	std::string answer = "";

	if (hKey) {
		std::cout << "Seems you use auto start up option do you want to turn off it ? (yes, no) ";
		std::cin >> answer;
		std::cout << answer << std::endl;
		if (answer == "yes") {
			removeAutoStart();
		}
		else {
			std::cout << "enjoy app" << std::endl;
		}
		answer = "";
	}
	else {
		std::cout << "Do you want to add utility to auto startup? (yes, no) ";
		std::cin >> answer;
		if (answer == "yes") {
			setAutoStart();
		}
		else {
			std::cout << "enjoy app" << std::endl;
		}
		answer = "";
	}

	RegCloseKey(hKey);

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
