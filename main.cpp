//
// Created by anonymus-raccoon on 1/24/20.
//

#include <iostream>
#include <QtWidgets/QApplication>
#include <getopt.h>
#include "sources/SNES.hpp"
#include "sources/Renderer/SFRenderer.hpp"
#include "sources/Renderer/QtRenderer/QtSFML.hpp"

using namespace ComSquare;

void usage(char *bin)
{
	std::cout << "ComSquare:" << std::endl << "\tUsage: " << bin << " rom_path" << std::endl;
}

void parseArguments(int argc, char **argv, SNES &snes)
{
	while (true) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"cpu",     no_argument, 0,  'c' },
			{"memory",     no_argument, 0,  'm' },
			{"header",     no_argument, 0,  'h' },
			{0,         0,                 0,  0 }
		};

		char c = getopt_long(argc, argv, "cmh", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argv[0]);
			break;
		case 'c':
			snes.enableCPUDebugging();
			break;
		case 'm':
			snes.enableRamViewer();
			break;
		case 'h':
			snes.enableHeaderViewer();
			break;
		default:
			break;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}
	QApplication app(argc, argv);
	Renderer::QtSFML renderer(600, 800);
	SNES snes(std::make_shared<Memory::MemoryBus>(), argv[1], renderer);
	renderer.createWindow(snes, 60);
	parseArguments(argc, argv, snes);
	return QApplication::exec();
}