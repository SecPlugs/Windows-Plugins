// secplugs-windows-oas-scanner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "filewatcher.hpp"
#include "scannerservice.h"

int main(int argc, char** argv)
{
	secplugs::scannerservice plugin("c:\\program files\\secplugs\\config.json");
	return plugin.run(argc, argv);
}

