//
// Created by anonymus-raccoon on 1/24/20.
//

#include <iostream>
#include <QtWidgets/QApplication>
#include <getopt.h>
#include "SNES.hpp"
#include "Renderer/QtRenderer/QtSFML.hpp"

using namespace ComSquare;

void usage(char *bin)
{
	std::cout << "ComSquare:" << std::endl
		<< "\tUsage: " << bin << " rom_path [options]" << std::endl
		<< "Options:" << std::endl
		<< "\t-c, --cpu:       \tEnable the debugger of the CPU." << std::endl
		<< "\t-m, --memory:    \tEnable the memory viewer panel." << std::endl
		<< "\t-h, --header:    \tShow the header of the cartridge." << std::endl
		<< "\t-b, --bus:       \tShow the memory bus's log." << std::endl
		<< "\t-g, --cgram:     \tShow the palette viewer." << std::endl
		<< "\t-r, --registers: \tShow the registers viewer." << std::endl;
}

void parseArguments(int argc, char **argv, SNES &snes)
{
	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{"cpu",       no_argument, 0, 'c'},
			{"apu",       no_argument, 0, 'a'},
			{"memory",    no_argument, 0, 'm'},
			{"header",    no_argument, 0, 'h'},
			{"bus",       no_argument, 0, 'b'},
			{"cgram",     no_argument, 0, 'g'},
			{"registers", no_argument, 0, 'r'},
			{0,           0,           0, 0}
		};

		int c = getopt_long(argc, argv, "camhbgr", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argv[0]);
			break;
		case 'c':
			snes.enableCPUDebugging();
			break;
		case 'a':
			snes.enableAPUDebugging();
			break;
		case 'm':
			snes.enableRamViewer();
			break;
		case 'h':
			snes.enableHeaderViewer();
			break;
		case 'b':
			snes.enableMemoryBusDebugging();
			break;
		case 'g':
			snes.enableCgramDebugging();
			break;
		case 'r':
			snes.enableRegisterDebugging();
			break;
		default:
			break;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		usage(argv[0]);
		return 1;
	}
	QApplication app(argc, argv);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
	Renderer::QtSFMLWindow renderer(1100, 1100);
	try {
		SNES snes(argv[1], renderer);
		renderer.createWindow(snes, 60);
		parseArguments(argc, argv, snes);
		return QApplication::exec();
	}
	catch(std::exception &ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}