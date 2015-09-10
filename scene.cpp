#include "scene.h"

#include <queue>
#include <iostream>

#include "Vector3d.h"

namespace
{
    const float eps = 0.00001f;
}


Scene::Scene()
{
}

Scene::Scene(const Surface &surface)
    : m_facets(surface)
{
}

void Scene::setName(const std::string &name)
{
    m_name = name;
}

std::string Scene::name() const
{
    return m_name;
}

void Scene::addFacet(const Facet &facet)
{
    m_facets.push_back(facet);
}

std::vector<Facet> Scene::facets() const
{
    return m_facets;
}

std::vector<Scene::Surface> Scene::surfaces() const
{
    using PointData = std::pair<Vector3D, int>;
    std::vector<PointData> points;

    // add all (Point, PointFacetIndex) pairs to array
    for (size_t i = 0; i < m_facets.size(); ++i) {
        const Triangle &tr = m_facets[i].triangle;
        points.push_back(std::make_pair(tr.p1, i));
        points.push_back(std::make_pair(tr.p2, i));
        points.push_back(std::make_pair(tr.p3, i));
    }

    /*
     * sort all points by X-coord and check only points which have dX < eps,
     * cause if dX > eps => sqrt(dX ^ 2 + dY ^ 2 + dZ ^ 2) > eps
     * */
    std::sort(points.begin(), points.end(), [](const PointData &d1, const PointData &d2) {
        return d1.first.x < d2.first.x;
    });

    // graph represented using adjacency list
    std::vector<std::vector<int>> facetsConnectivityGraph(m_facets.size(), std::vector<int>());

    for (size_t i = 0; i + 1 < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            if (fabs(points[i].first.x - points[j].first.x) >= eps)
                break;

            if (Vector3D::distance(points[i].first, points[j].first) < eps) {
                facetsConnectivityGraph[points[i].second].push_back(points[j].second);
                facetsConnectivityGraph[points[j].second].push_back(points[i].second);
            }
        }
    }

    std::vector<Surface> surfaces;

    // breadth-first connectivity components search
    std::vector<bool> used(m_facets.size(), false);
    for (size_t i = 0; i < used.size(); ++i) {
        if (used[i])
            continue;

        Surface surface;
        std::queue<int> facetsQueue;
        facetsQueue.push(i);
        used[i] = true;

        while (!facetsQueue.empty()) {
            const int currFacetIndex = facetsQueue.front();
            facetsQueue.pop();

            surface.push_back(m_facets[currFacetIndex]);

            for (size_t j = 0; j < facetsConnectivityGraph[currFacetIndex].size(); ++j) {
                const int otherVertexIndex = facetsConnectivityGraph[currFacetIndex][j];
                if (!used[otherVertexIndex]) {
                    facetsQueue.push(otherVertexIndex);
                    used[otherVertexIndex] = true;
                }
            }
        }

        surfaces.push_back(surface);
    }

    return surfaces;
}

void Scene::transform(const TransformMatrix &trMatrix)
{
    for (Facet &facet : m_facets) {
        Triangle &tr = facet.triangle;
        tr.p1.applyTransform(trMatrix);
        tr.p2.applyTransform(trMatrix);
        tr.p3.applyTransform(trMatrix);
    }
}

void Scene::centralize()
{
    const BoundingBox bbox = boundingBox();
    const Vector3D diff = -(bbox.p2 - bbox.p1) / 2;
    const auto matrix = TransformMatrix::buildTransposeMatrix(diff.x, diff.y, diff.z);
    transform(matrix);
}

BoundingBox Scene::boundingBox() const
{
    Vector3D p1 = { 0, 0, 0 };
    Vector3D p2 = { 0, 0, 0 };

    if (!m_facets.empty()) {
        p1 = m_facets[0].triangle.p1;
        p2 = p1;
    }

    for (const Facet &facet : m_facets) {
        const Triangle &tr = facet.triangle;
        p1 = Vector3D::min(p1, tr.p1);
        p2 = Vector3D::max(p2, tr.p1);

        p1 = Vector3D::min(p1, tr.p2);
        p2 = Vector3D::max(p2, tr.p2);

        p1 = Vector3D::min(p1, tr.p3);
        p2 = Vector3D::max(p2, tr.p3);
    }

    BoundingBox bBox;
    bBox.p1 = p1;
    bBox.p2 = p2;
    return bBox;
}
