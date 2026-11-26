// Copyright (C) 2026 Ilya Shishkin cortl@yandex.ru
// https://github/cortl0/MaxDoF
// Licensed under the GNU Affero General Public License v3.0 or later
// See LICENSE file in the project root for full license information

#pragma once

#include "Common/Types/LevelGraph.h"
#include "Primitive.h"

 namespace MaxDoF::Common::LevelGraphs
{
    class Pipe : public Primitive
    {
    public:
        //using Vertices = std::array<Types::Level::Graph::Triangle::Vertices, 2>;
        //using TriangleVertexKeys = std::array<Types::Level::Graph::Triangle::VertexKeys, 2>;
        using OrientedTriangleKeys = std::array<Types::Level::Graph::Triangle::Key, 2>;
        using OrientedTriangleTypes = std::array<Types::Level::Graph::OrientedTriangle::Type, 2>;
        using InvertInterfaces = std::array<bool, 2>;

        struct Config
        {
            struct Side
            {
                Types::Level::Graph::Triangle::Key key;
                Types::Level::Graph::OrientedTriangle::Type type;
                bool flip;
            };

            bool flip;
            Side begin;
            Side end;
        };

        void FillGraph(Types::Level::Graph&, const Config&);
    };
}
