#ifndef STLSERIALIZER_H
#define STLSERIALIZER_H

#include "scene.h"
#include "exceptions.h"

class StlSerializer
{
public:
    StlSerializer();

    void serializeTo(std::ostream &stream, const Scene &scene) throw ();
    void serializeFrom(std::istream &stream, Scene &scene) throw (BadFormatException, UnexpectedEofException, CantReadException);

private:
    std::istream *m_inputStream;
    std::string m_lineBuffer;

    std::string checkedRead();
    std::string peekLine() const;

    void readSolid(Scene &scene);
    void readFacet(Facet &facet);
    void readNormale(Vector3D &normale);
    void readTriangle(Triangle &triangle);
    void readVertex(Vector3D &vertex);

    void writeSolid(std::ostream &stream, const Scene &scene);
    void writeFacet(std::ostream &stream, const Facet &facet);
    void writeNormale(std::ostream &stream, const Vector3D &normale);
    void writeTriangle(std::ostream &stream, const Triangle &triangle);
    void writeVertex(std::ostream &stream, const Vector3D &vertex);

};

#endif // STLSERIALIZER_H
