/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "Cylinder.h"

namespace brayns
{

Cylinder::Cylinder(
    const size_t materialId,
    const Vector3f& center,
    const Vector3f& up,
    const float radius,
    const float timestamp)
    : Primitive(materialId)
    , _center(center)
    , _up(up)
    , _radius(radius)
    , _timestamp(timestamp)
{
    _geometryType = GT_CYLINDER;
}

size_t Cylinder::serializeData(floats& serializedData)
{
    floats data;
    data.push_back(_center.x());
    data.push_back(_center.y());
    data.push_back(_center.z());
    data.push_back(_up.x() );
    data.push_back(_up.y() );
    data.push_back(_up.z() );
    data.push_back(_radius );
    data.push_back(_timestamp );
    serializedData.insert(std::end(serializedData), std::begin(data), std::end(data));
    return data.size();
}

}