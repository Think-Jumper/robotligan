#include <iostream>

#include <glob/graphics.h>
#include "testphysics.h"
#include "testSound.h"
#include "testNetwork.h"

//#include <glad/glad.h>


int main(unsigned argc, char **argv)
{
	std::cout << "Hello World!*!!!111\n";

	std::cout << "Test fr�n development " << testPhysics() << " " << testSound() << " " << testNetwork();

	std::cout << "Test fr�n development2 " << glob::GraphicsTest() << "\n";

	return EXIT_SUCCESS;
}