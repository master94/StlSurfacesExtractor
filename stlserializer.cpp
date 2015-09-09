#include "stlserializer.h"

#include <iostream>
#include <sstream>

#define CHECKED_READ(stream, line) std::getline(stream, line);\
    if (stream.eof()) throw UnexpectedEofException();\
    if (stream.fail() || stream.bad()) throw CantReadException();

#define ASSERT_FORMAT_EXPR(expr, message) if (!(expr))\
    throw BadFormatException(message);
#define ASSERT_FORMAT_TOKEN(token, ethalon, message) if ((token) != (ethalon))\
    throw BadFormatException(message);

namespace
{
    std::vector<std::string> split(const std::string &str)
    {
        std::vector<std::string> tokens;

        int prev = -1;

        for (int i = 0; i < str.size(); ++i) {
            if (str[i] == ' ') {
                if (i > prev + 1)
                    tokens.push_back(str.substr(prev + 1, i - prev - 1));
                prev = i;
            }
        }

        if (prev + 1 < str.size())
            tokens.push_back(str.substr(prev + 1, str.size() - prev - 1));

        return tokens;
    }

    bool from_string(float &value, const std::string &str)
    {
        return !(std::istringstream(str) >> value).fail();
    }

    std::string peekLine(std::istream &stream)
    {
        const auto pos = stream.tellg();
        std::string line;
        std::getline(stream, line);
        stream.seekg(pos);
        return line;
    }

    const std::string cSolidStartMarker = "solid";
    const std::string cSolidEndMarker = "endsolid";
    const std::string cFacetStartMarker = "facet";
    const std::string cFacetEndMarker = "endfacet";
    const std::string cOuterLoopStartMarker = "outer loop";
    const std::string cOuterLoopEndMarker = "endloop";
    const std::string cVertexMarker = "vertex";
    const std::string cNormalMarker = "normal";
}


StlSerializer::StlSerializer()
{
}

void StlSerializer::serializeTo(std::ostream &stream, const Scene &scene) throw ()
{
    writeSolid(stream, scene);
}

void StlSerializer::serializeFrom(std::istream &stream, Scene &scene) throw (BadFormatException, UnexpectedEofException, CantReadException)
{
    readSolid(stream, scene);
}

void StlSerializer::readSolid(std::istream &stream, Scene &scene)
{
    std::string line;
    CHECKED_READ(stream, line);

    std::vector<std::string> tokens = split(line);
    ASSERT_FORMAT_EXPR(tokens.size() == 1 || tokens.size() == 2, "Wrong tokens qty for SOLID header.");
    ASSERT_FORMAT_TOKEN(tokens[0], cSolidStartMarker, "Wrong token for SOLID header.");

    if (tokens.size() == 2)
        scene.setName(tokens[1]);

    while (peekLine(stream) != cSolidEndMarker) {
        Facet facet;
        readFacet(stream, facet);
        scene.addFacet(facet);
    }

    CHECKED_READ(stream, line);
    tokens = split(line);
    ASSERT_FORMAT_EXPR(tokens.size() == 1 || tokens.size() == 2, "Wrong tokens qty for SOLID end.");
    ASSERT_FORMAT_TOKEN(tokens[0], cSolidEndMarker, "Wrong token for SOLID end.");
}

void StlSerializer::readFacet(std::istream &stream, Facet &facet)
{
    readNormale(stream, facet.normal);

    std::string line;
    CHECKED_READ(stream, line);
    ASSERT_FORMAT_TOKEN(line, cOuterLoopStartMarker, "Wrong token for vertices loop start.");

    readTriangle(stream, facet.triangle);

    CHECKED_READ(stream, line);
    ASSERT_FORMAT_TOKEN(line, cOuterLoopEndMarker, "Wrong token for vertices end.");

    CHECKED_READ(stream, line);
    ASSERT_FORMAT_TOKEN(line, cFacetEndMarker, "Wrong token for facet end.");
}

void StlSerializer::readNormale(std::istream &stream, Vector3D &normale)
{
    std::string line;
    CHECKED_READ(stream, line);

    const std::vector<std::string> tokens = split(line);
    ASSERT_FORMAT_EXPR(tokens.size() == 5, "Wrong tokens qty for facet begin.");

    ASSERT_FORMAT_TOKEN(tokens[0], cFacetStartMarker, "Wrong token for facet.");
    ASSERT_FORMAT_TOKEN(tokens[1], cNormalMarker, "Wrong token for normal.");

    ASSERT_FORMAT_EXPR(from_string(normale.x, tokens[2]), "Can't parse normal X-value.");
    ASSERT_FORMAT_EXPR(from_string(normale.y, tokens[3]), "Can't parse normal Y-value.");
    ASSERT_FORMAT_EXPR(from_string(normale.z, tokens[4]), "Can't parse normal Z-value.");
}

void StlSerializer::readTriangle(std::istream &stream, Triangle &triangle)
{
    readVertex(stream, triangle.p1);
    readVertex(stream, triangle.p2);
    readVertex(stream, triangle.p3);
}

void StlSerializer::readVertex(std::istream &stream, Vector3D &vertex)
{
    std::string line;
    CHECKED_READ(stream, line);

    const std::vector<std::string> tokens = split(line);
    ASSERT_FORMAT_EXPR(tokens.size() == 4, "Wrong tokens qty for vertex.");
    ASSERT_FORMAT_TOKEN(tokens[0], cVertexMarker, "Wrong token for vertex.");

    ASSERT_FORMAT_EXPR(from_string(vertex.x, tokens[1]), "Can't parse vertex X-value");
    ASSERT_FORMAT_EXPR(from_string(vertex.y, tokens[2]), "Can't parse vertex Y-value");
    ASSERT_FORMAT_EXPR(from_string(vertex.z, tokens[3]), "Can't parse vertex Z-value");
}

void StlSerializer::writeSolid(std::ostream &stream, const Scene &scene)
{
    stream << cSolidStartMarker << " " << scene.name() << std::endl;

    for (const Facet &facet : scene.facets())
        writeFacet(stream, facet);

    stream << cSolidEndMarker << " " << scene.name() << std::endl;
}

void StlSerializer::writeFacet(std::ostream &stream, const Facet &facet)
{
    writeNormale(stream, facet.normal);
    stream << cOuterLoopStartMarker << std::endl;
    writeTriangle(stream, facet.triangle);
    stream << cOuterLoopEndMarker << std::endl;
    stream << cFacetEndMarker << std::endl;
}

void StlSerializer::writeNormale(std::ostream &stream, const Vector3D &normale)
{
    stream << cFacetStartMarker << " " << cNormalMarker << " "
           << normale.x << " " << normale.y << " " << normale.z << std::endl;
}

void StlSerializer::writeTriangle(std::ostream &stream, const Triangle &triangle)
{
    writeVertex(stream, triangle.p1);
    writeVertex(stream, triangle.p2);
    writeVertex(stream, triangle.p3);
}

void StlSerializer::writeVertex(std::ostream &stream, const Vector3D &vertex)
{
    stream << cVertexMarker << " " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
}
