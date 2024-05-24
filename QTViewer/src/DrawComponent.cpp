#include "DrawComponent.h"

#include "gl/freeglut.h"
#include <chrono>
#include <queue>
#include <set>

#include "pmp/io/io.h"
#include "pmp/algorithms/normals.h"

void DrawComponent::Init()
{
    //1. Load file or Create mesh.
    //정육면체 그리기 (W10)
    InitMyMesh();
    
    //고양이 (W11)
    //InitLoadFile();

    //2. Add attributes.
    AddAttributes();

    pmp::write(mesh, "output.obj");
}

void DrawComponent::InitLoadFile()
{
    pmp::read(mesh, "obj\\kitten.obj");
    pmp::vertex_normals(mesh);
    pmp::face_normals(mesh);
}

void DrawComponent::InitMyMesh()
{

    const auto v0 = mesh.add_vertex(pmp::Point(0, 0, 0));
    const auto v1 = mesh.add_vertex(pmp::Point(2, 0, 0));
    const auto v2 = mesh.add_vertex(pmp::Point(2, 2, 0));
    const auto v3 = mesh.add_vertex(pmp::Point(0, 2, 0));
    const auto v4 = mesh.add_vertex(pmp::Point(0, 0, 2));
    const auto v5 = mesh.add_vertex(pmp::Point(2, 0, 2));
    const auto v6 = mesh.add_vertex(pmp::Point(2, 2, 2));
    const auto v7 = mesh.add_vertex(pmp::Point(0, 2, 2));

    mesh.add_quad(v4, v5, v6, v7);
    mesh.add_quad(v0, v3, v2, v1);
    mesh.add_quad(v0, v4, v7, v3);
    mesh.add_quad(v7, v6, v2, v3);
    mesh.add_quad(v2, v6, v5, v1);
    mesh.add_quad(v1, v5, v4, v0);
    
    std::cout << "vertices: " << mesh.n_vertices() << std::endl;
    std::cout << "edges: " << mesh.n_edges() << std::endl;
    std::cout << "faces: " << mesh.n_faces() << std::endl;

    pmp::vertex_normals(mesh);
    pmp::face_normals(mesh);
}

void DrawComponent::Draw()
{
    auto vn = mesh.get_vertex_property<pmp::Normal>("v:normal");
    auto fn = mesh.get_face_property<pmp::Normal>("f:normal");
    auto fc = mesh.get_face_property<pmp::Color>("f:color");

    //1. Basic drawing
    if (false)
        for (auto f : mesh.faces()) {
            glNormal3dv(fn[f].data());
            DrawFace(f);
        }

    //2. Color with normals
    if (true)
        for (auto f : mesh.faces())
        {
            glColor3dv(fc[f].data());
            DrawFace(f);
        }

    //3. Traverse the mesh
    if (false)
    {
        glLineWidth(1.0);
        glPointSize(20.0);

        //3-1. Draw the mesh with wireframe.
        glColor3f(0, 0, 0);
        for (auto f : mesh.faces()) {
            DrawFace(f, true);
        }

        //3-2. Draw a vertex which index is 0 with red.
        pmp::Vertex startV(0);
        glColor3f(1, 0, 0);
        glBegin(GL_POINTS);
        glVertex3dv(mesh.position(startV).data());
        glEnd();

        //3-3. Draw face(s) which has vertex(0) with blue.
        for (auto f : mesh.faces(startV)) {
            glColor3f(0, 0, 1);
            DrawFace(f);
        }

        std::set<pmp::Vertex> vertices1; // 0-ring vertices
        std::set<pmp::Vertex> vertices2; // 1-ring vertices
        std::set<pmp::Vertex> vertices3; // 2-ring vertices

        //3-4. Draw vertices which are connected with vertex(0) with green.
        glColor3f(0, 1, 0);
        glBegin(GL_POINTS);
        for (auto v : mesh.vertices(startV)) {
            auto p = mesh.position(v);
            vertices1.insert(v);
            glVertex3dv(p.data());
        }
        glEnd();

        if (true) {
            //3-5. Draw faces which has green vertices but no red vertex,
            //							   with yellow.
            glColor3f(1, 1, 0);

            std::set<pmp::Face> visitedFaces;
            for (auto f : mesh.faces(startV))
                visitedFaces.insert(f);

            for (auto v : mesh.vertices(startV)) {
                for (auto f : mesh.faces(v)) {
                    if (visitedFaces.find(f) == visitedFaces.end())
                    {
                        DrawFace(f);
                    }
                }
            }
        }

        if (true) {
            // 2-ring face
            std::set<pmp::Face> visitedFaces;
            glColor3f(0, 1, 0); // Green

            for (auto v1 : vertices1) {
                for (auto v : mesh.vertices(v1))
                    if (v != startV && vertices1.find(v) == vertices1.end())
                        vertices2.insert(v);

                for (auto f : mesh.faces(v1))
                    visitedFaces.insert(f);
            }

            for (auto v : vertices2) {
                for (auto f : mesh.faces(v)) {
                    if (visitedFaces.find(f) == visitedFaces.end())
                    {
                        DrawFace(f);
                    }
                }
            }
        }

        if (true) {
            // 3-ring face
            glColor3f(0.7, 0.33, 0.64); // Purple
            std::set<pmp::Face> visitedFaces;

            for (auto v2 : vertices2) {
                for (auto v : mesh.vertices(v2))
                    if (vertices1.find(v) == vertices1.end() && vertices2.find(v) == vertices2.end())
                        vertices3.insert(v);

                for (auto f : mesh.faces(v2))
                    visitedFaces.insert(f);
            }

            for (auto v : vertices3) {
                for (auto f : mesh.faces(v)) {
                    if (visitedFaces.find(f) == visitedFaces.end())
                    {
                        DrawFace(f);
                    }
                }
            }


        }
    }
}

void DrawComponent::DrawFace(pmp::Face f, bool isWire) {
    glBegin(isWire ? GL_LINE_LOOP : GL_POLYGON);
    for (auto v : mesh.vertices(f))
        glVertex3dv(mesh.position(v).data());
    glEnd();
}

void DrawComponent::AddAttributes()
{
    mesh.add_face_property<pmp::Color>("f:color");

    // compute vertex normals
    pmp::vertex_normals(mesh);

    // compute face normals
    pmp::face_normals(mesh);

    // colorize faces based on normals
    auto fn = mesh.get_face_property<pmp::Normal>("f:normal");
    auto fc = mesh.get_face_property<pmp::Color>("f:color");

    for (auto f : mesh.faces())
    {
        auto n = fn[f];
        pmp::Color c(std::abs(n[0]), std::abs(n[1]), std::abs(n[2]));
        fc[f] = c;
    }
}
