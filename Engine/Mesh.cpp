#include "Precompiled.h"
#include "Mesh.h"
#include <vector>
#include "../include/glm/glm.hpp"
#include <iostream>
#include <cmath>
#define PI 3.14159265
struct Vertex
{
    Vertex(glm::vec3 p, glm::vec3 norm, glm::vec2 tc)
        :p(p),n(norm),tc(tc) {}
    Vertex(glm::vec3 p, glm::vec3 norm)
        :p(p), n(norm), tc(0.f) {}
    glm::vec3 p;
    glm::vec3 n;
    glm::vec2 tc;
};

Mesh::Mesh(unsigned int id, unsigned int iCount)
    :ID(id), indexCount(iCount)
{
}

const unsigned int Mesh::GetNumTriangles() const
{
    return indexCount / 3;
}

void Mesh::Draw(unsigned int mode)
{
    glBindVertexArray(ID);
    glDrawElements(mode, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int textureID, unsigned int textureSlot, unsigned int textureType, unsigned int mode)
{
    glBindVertexArray(ID);
    glActiveTexture(textureSlot);
    glBindTexture(textureType, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

std::unique_ptr<Mesh> Mesh::CreateQuad(const unsigned int size)
{ 
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> quadVert;

    const auto s = size * 0.5f;
    quadVert.push_back(Vertex({ s, s, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f }));
    quadVert.push_back(Vertex({ s, -s, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f }));
    quadVert.push_back(Vertex({ -s, -s, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f }));
    quadVert.push_back(Vertex({ -s, s, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f }));

    unsigned int indices[] =
    {
        0U, 1U, 3U,
        1U, 2U, 3U
    };
 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, quadVert.size() * sizeof(Vertex), quadVert.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_READ);
 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL); 
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tc));
    glBindVertexArray(0);

    std::unique_ptr<Mesh> mesh(new Mesh(VAO, _countof(indices)));
    return mesh;
}

std::unique_ptr<Mesh> Mesh::CreateCircle(const unsigned int vertCount, const float radius)
{
    //I like cout with breakpoints for debugging, so I will just leave output debugging code commented out
    //I use nvidia nsight for GPU debugging
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> circleVert;//Would be better to use static arrays for better performance, could be a TODO for another time, probably not a huge performance difference either
    std::vector<unsigned int> indices;
    //Lets calculate radians pr vertex
    const float radPrVert = (2.f * (float)PI) / (float)vertCount;
    //std::cout << "Rad pr vertex: " << radPrVert << std::endl;
    //Circle centre
    circleVert.push_back(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
    for (unsigned int i = 1, j = 2; i < vertCount; i++, j++)
    {      
        //I have to invert cos because I need to do a clockwise order to discard all back faces
        Vertex temp({ -cosf(radPrVert * (float)i) * radius, sinf(radPrVert * (float)i) * radius, 0.f }, glm::vec3(0.f, 0.f, 1.f));
        circleVert.push_back(temp); 
        indices.push_back(0U);
        indices.push_back(i);
        indices.push_back(j);
    }
    //For the last triangle, goes from the last vertex count back to 1, this is as optimized as it can be I think, however there might be a way to get everything inside the loop, but w.e doesn't really matter, I wont get better performance by doing that
    Vertex temp({ -cosf(radPrVert * (float)vertCount) * radius, sinf(radPrVert * (float)vertCount) * radius, 0.f }, glm::vec3(0.f, 0.f, 1.f));
    circleVert.push_back(temp);
    indices.push_back(0);
    indices.push_back(circleVert.size() - 1);
    indices.push_back(1);
  
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, circleVert.size() * sizeof(Vertex), circleVert.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));
    glBindVertexArray(0);
    
    std::unique_ptr<Mesh> mesh(new Mesh(VAO, indices.size() * 3U));
    //std::cout << "Indices size:" << indices.size()<< std::endl;
    return mesh;
    
}

std::unique_ptr<Mesh> Mesh::CreateGrid(const unsigned int gridX, const unsigned int gridY, const float xSize, const float ySize)
{
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> gridVert;
    std::vector<unsigned int> indices;

    const auto halfSizeX = (xSize*gridX) * 0.5f;
    const auto halfSizeY = (ySize*gridY) * 0.5f;

    //Gotta add 1 to draw the last line
    for (int y = 0, const columnSize = gridX + 1, const rowSize = gridY + 1, f = columnSize, const sum = columnSize * rowSize; y <= gridY; y++, f--)
    { 
        indices.push_back(columnSize * y);
        indices.push_back((columnSize + columnSize * y) - 1);
        indices.push_back(y);
        indices.push_back(sum - f);
        for (int x = 0; x <= gridX; x++)
        {        
            const auto xPos = ((float)x * xSize) - halfSizeX;
            const auto yPos = ((float)y * ySize) - halfSizeY;
            gridVert.push_back(Vertex({ xPos, yPos, 0.f }, glm::vec3(0.f, 0.f, 1.f)));
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, gridVert.size() * sizeof(Vertex), gridVert.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, n));
    glBindVertexArray(0);

    std::unique_ptr<Mesh> mesh(new Mesh(VAO, indices.size()));
    return mesh;
}


std::unique_ptr<Mesh> Mesh::CreateSkyBox(const float s)
{
    unsigned int VAO, VBO;
    const float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)NULL);
    glBindVertexArray(0);

    std::unique_ptr<Mesh> mesh(new Mesh(VAO, 36));
    return mesh;
    return std::unique_ptr<Mesh>();
}
