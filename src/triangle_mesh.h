#pragma once

class TriangleMesh {
public:
    TriangleMesh();
    void draw() const;
    ~TriangleMesh();

private:
    unsigned int VBO{}, VAO{}, vertex_count;
};