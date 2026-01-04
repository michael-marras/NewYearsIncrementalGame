#include "world/planet.h"

int GetPlanetRadius(PlanetSize size) {
    switch (size) {
        case PlanetSize::TINY:
            return 25;
        case PlanetSize::SMALL:
            return 50;
        case PlanetSize::MEDIUM:
            return 100;
        case PlanetSize::LARGE:
            return 200;
        case PlanetSize::EXTRA_LARGE:
            return 300;
        case PlanetSize::GIANT:
            return 500;
        default:
            return 100;
    }
}

Planet::Planet() {
}

Planet::~Planet() {
}

PlanetFaceData* Planet::GetFaceData(PlanetFace face) {
    auto it = faces.find(face);
    if (it != faces.end()) {
        return &it->second;
    }
    return nullptr;
}

const PlanetFaceData* Planet::GetFaceData(PlanetFace face) const {
    auto it = faces.find(face);
    if (it != faces.end()) {
        return &it->second;
    }
    return nullptr;
}

void Planet::SetFaceData(PlanetFace face, int tileGridId, int objectGridId, int resourceArrayId) {
    faces[face] = PlanetFaceData(tileGridId, objectGridId, resourceArrayId);
}

void Planet::SetFaceData(PlanetFace face, const PlanetFaceData& data) {
    faces[face] = data;
}

bool Planet::HasFace(PlanetFace face) const {
    return faces.find(face) != faces.end();
}

int Planet::GetTileGridId(PlanetFace face) const {
    const PlanetFaceData* data = GetFaceData(face);
    return data ? data->tileGridId : -1;
}

int Planet::GetObjectGridId(PlanetFace face) const {
    const PlanetFaceData* data = GetFaceData(face);
    return data ? data->objectGridId : -1;
}

int Planet::GetResourceArrayId(PlanetFace face) const {
    const PlanetFaceData* data = GetFaceData(face);
    return data ? data->resourceArrayId : -1;
}

