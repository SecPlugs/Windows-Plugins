// secplugs-windows-oas-scanner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "filewatcher.hpp"

int main(int argc, char** argv)
{
    std::cout << "Starting!\n";
	secplugs::configloader cfg("d:\\temp\\config.json");
	secplugs::restclient rest(cfg);
	secplugs::filewatcher fw;
	fw.init();
	fw.watch(rest);
	return 0;
}

