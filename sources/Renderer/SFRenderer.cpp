//
// Created by cbihan on 1/30/20.
//

#include "SNES.hpp"
#include "SFRenderer.hpp"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

namespace ComSquare::Renderer
{
	SFRenderer::SFRenderer(unsigned int height, unsigned int width)
	{
		this->_videoMode = {width, height, 32};
		this->_texture.create(width, height);
		this->_sprite.setTexture(this->_texture);
		this->_pixelBuffer = new sf::Color[height * width];
		this->_sound.setBuffer(this->_soundBuffer);
	}

	void SFRenderer::createWindow(SNES &snes, int maxFPS)
	{
		sf::Image icon;
		this->_window.create(this->_videoMode, "ComSquare Emulator", sf::Style::Default);
		if (icon.loadFromFile("resources/Logo.png"))
			this->_window.setIcon(314, 314, icon.getPixelsPtr());
		this->_window.setFramerateLimit(maxFPS);
		this->setWindowName(snes.cartridge.header.gameName);

		while (!this->shouldExit) {
			snes.update();
			this->getEvents();
		}
	}

	SFRenderer::~SFRenderer()
	{
		delete [] this->_pixelBuffer;
	}

	void SFRenderer::setWindowName(std::string &newWindowName)
	{
		this->_window.setTitle(newWindowName + " - ComSquare");
	}

	void SFRenderer::drawScreen()
	{
		this->_texture.update(reinterpret_cast<sf::Uint8 *>(this->_pixelBuffer));
		this->_sprite.setTexture(this->_texture);
		this->_window.draw(this->_sprite);
		this->_window.display();
	}

	void SFRenderer::playAudio(std::span<int16_t> samples)
	{
		this->_soundBuffer.loadFromSamples(samples.data(), samples.size(), 2, 32040);
		this->_sound.play();
	}

	void SFRenderer::putPixel(unsigned y, unsigned x, uint32_t rgba)
	{
		if (x >= this->_videoMode.width)
			throw InvalidPixelPosition("Width", x, this->_videoMode.width);
		if (y >= this->_videoMode.height)
			throw InvalidPixelPosition("Height", y, this->_videoMode.height);

		sf::Color pixels(rgba);
		this->_pixelBuffer[this->_videoMode.width * y + x] = pixels;
	}

	void SFRenderer::getEvents()
	{
		sf::Event event;
		while (this->_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				this->shouldExit = true;
				break;
			}
		}
	}
}
