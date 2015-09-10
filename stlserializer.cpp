#include "stlserializer.h"

#include <sstream>
#include <cassert>

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
    : m_inputStream(nullptr)
{
}

void StlSerializer::serializeTo(std::ostream &stream, const Scene &scene) throw ()
{
    writeSolid(stream, scene);
}

void StlSerializer::serializeFrom(std::istream &stream, Scene &scene) throw (BadFormatException, UnexpectedEofException, CantReadException)
{
    m_inputStream = &stream;
    checkedRead(); // initialize buffer
    readSolid(scene);
    m_inputStream = nullptr;
}

std::string StlSerializer::checkedRead()
{
    assert(m_inputStream != nullptr && "Input stream must not be NULL");

    if (m_inputStream->eof())
        throw UnexpectedEofException();

    if (m_inputStream->fail() || m_inputStream->bad())
        throw CantReadException();

    const std::string tmpLine = m_lineBuffer;
    std::getline(*m_inputStream, m_lineBuffer);
    return tmpLine;
}

std::string StlSerializer::peekLine() const
{
    return m_lineBuffer;
}

void StlSerializer::readSolid(Scene &scene)
{
    std::vector<std::string> tokens = split(checkedRead());
    ASSERT_FORMAT_EXPR(tokens.size() == 1 || tokens.size() == 2, "Wrong tokens qty for SOLID header.");
    ASSERT_FORMAT_TOKEN(tokens[0], cSolidStartMarker, "Wrong token for SOLID header.");

    if (tokens.size() == 2)
        scene.setName(tokens[1]);

    while (peekLine() != cSolidEndMarker) {
        Facet facet;
        readFacet(facet);
        scene.addFacet(facet);
    }

    tokens = split(checkedRead());
    ASSERT_FORMAT_EXPR(tokens.size() == 1 || tokens.size() == 2, "Wrong tokens qty for SOLID end.");
    ASSERT_FORMAT_TOKEN(tokens[0], cSolidEndMarker, "Wrong token for SOLID end.");
}

void StlSerializer::readFacet(Facet &facet)
{
    readNormale(facet.normal);

    ASSERT_FORMAT_TOKEN(checkedRead(), cOuterLoopStartMarker, "Wrong token for vertices loop start.");

    readTriangle(facet.triangle);

    ASSERT_FORMAT_TOKEN(checkedRead(), cOuterLoopEndMarker, "Wrong token for vertices end.");
    ASSERT_FORMAT_TOKEN(checkedRead(), cFacetEndMarker, "Wrong token for facet end.");
}

void StlSerializer::readNormale(Vector3D &normale)
{
    const std::vector<std::string> tokens = split(checkedRead());
    ASSERT_FORMAT_EXPR(tokens.size() == 5, "Wrong tokens qty for facet begin.");

    ASSERT_FORMAT_TOKEN(tokens[0], cFacetStartMarker, "Wrong token for facet.");
    ASSERT_FORMAT_TOKEN(tokens[1], cNormalMarker, "Wrong token for normal.");

    ASSERT_FORMAT_EXPR(from_string(normale.x, tokens[2]), "Can't parse normal X-value.");
    ASSERT_FORMAT_EXPR(from_string(normale.y, tokens[3]), "Can't parse normal Y-value.");
    ASSERT_FORMAT_EXPR(from_string(normale.z, tokens[4]), "Can't parse normal Z-value.");
}

void StlSerializer::readTriangle(Triangle &triangle)
{
    readVertex(triangle.p1);
    readVertex(triangle.p2);
    readVertex(triangle.p3);
}

void StlSerializer::readVertex(Vector3D &vertex)
{
    const std::vector<std::string> tokens = split(checkedRead());
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
