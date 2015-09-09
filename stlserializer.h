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
    void readSolid(std::istream &stream, Scene &scene);
    void readFacet(std::istream &stream, Facet &facet);
    void readNormale(std::istream &stream, Vector3D &normale);
    void readTriangle(std::istream &stream, Triangle &triangle);
    void readVertex(std::istream &stream, Vector3D &vertex);

    void writeSolid(std::ostream &stream, const Scene &scene);
    void writeFacet(std::ostream &stream, const Facet &facet);
    void writeNormale(std::ostream &stream, const Vector3D &normale);
    void writeTriangle(std::ostream &stream, const Triangle &triangle);
    void writeVertex(std::ostream &stream, const Vector3D &vertex);

};

#endif // STLSERIALIZER_H
