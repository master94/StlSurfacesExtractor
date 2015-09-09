#include <iostream>
#include <sstream>
#include <fstream>

#include "stlserializer.h"

void printUsage()
{
    std::cout << "Usage: <application> <input_stl_file> [<output_folder_path>]" << std::endl;
}

void printCantOpenFile(const std::string &filename)
{
    std::cout << "Cannot open \"" << filename << "\" file." << std::endl;
}

void printParseException(const std::string &filename, const std::string &error)
{
    std::cout << "Cannot parse \"" << filename << "\". " << error << std::endl;
}

std::string buildOutputFilePath(const std::string &folder, int partIndex)
{
    return (std::ostringstream() << folder << "/" << "part_" << partIndex << ".stl").str();
}


int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3) {
        printUsage();
        return EXIT_FAILURE;
    }

    Scene scene;
    StlSerializer serializer;
    std::ifstream stream(argv[1], std::ifstream::in);
    if (stream.fail()) {
        printCantOpenFile(argv[1]);
        return EXIT_FAILURE;
    }

    try {
        serializer.serializeFrom(stream, scene);
    }
    catch (const BadFormatException &e) {
        printParseException(argv[1], e.what());
        return EXIT_FAILURE;
    }
    catch (const UnexpectedEofException &) {
        printParseException(argv[1], "Unexpected EOF.");
        return EXIT_FAILURE;
    }
    catch (const CantReadException &) {
        printParseException(argv[1], "Some strange has happened. Can't read data from stream.");
        return EXIT_FAILURE;
    }

    int i = 0;
    for (const Scene::Surface &surface : scene.surfaces()) {
        const std::string outputFolder = argc == 3 ? argv[2] : "./";
        const std::string outputFilePath = buildOutputFilePath(outputFolder, i);
        ++i;

        std::ofstream ostream(outputFilePath, std::ofstream::out);
        if (ostream.fail()) {
            printCantOpenFile(outputFilePath);
            return EXIT_FAILURE;
        }

        Scene newScene(surface);
        newScene.centralize();
        serializer.serializeTo(ostream, newScene);
    }

    return EXIT_SUCCESS;
}

