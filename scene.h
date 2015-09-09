#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>

#include "structures.h"


class Scene
{
public:
    using Surface = std::vector<Facet>;

    Scene();
    Scene(const Surface &surface);

    void setName(const std::string &name);
    std::string name() const;

    void addFacet(const Facet &facet);
    std::vector<Facet> facets() const;

    std::vector<Surface> surfaces() const;

    void transform(const TransformMatrix &trMatrix);
    void centralize();
    BoundingBox boundingBox() const;

private:
    std::vector<Facet> m_facets;
    std::string m_name;

};

#endif // SCENE_H
