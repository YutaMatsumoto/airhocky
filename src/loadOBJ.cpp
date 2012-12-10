#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <string>
#include <vector>

#include "Vertex.cpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp" //Makes passing matrices to shaders easier

using namespace std;

bool loadOBJ(   const char * , 
                vector < Vertex > & , 
                vector < glm::vec2 > & ,
                vector < glm::vec3 > & ,
                int,
                float,
                float,
                float
);

bool loadOBJ(   const char * path, 
                vector < Vertex > & out_vertices, 
                vector < glm::vec2 > & out_uvs,
                vector < glm::vec3 > & out_normals,
                int type,
                float r,
                float g,
                float b
)
{
    vector<float> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;



    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ){
 
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
     
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 )
           {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
           }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
           {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
           }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
           {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
           }
        else if ( strcmp( lineHeader, "f" ) == 0 )
           {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

            if( type == 0 )
               {

                int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2] );
                if (matches != 3)
                   {
                    printf("File can't be read by this simple parser");
                    return false;
                   }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
               }
            else if( type == 1 )
               {
            
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9)
                   {
                    printf("File can't be read by this simple parser");
                    return false;
                   }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);   
               }
           }

       }

cout << "VertexIndices size: " << vertexIndices.size();
    // For each vertex of each triangle
    for( unsigned int i=0; i < vertexIndices.size(); i++ )
       {
        // for each vertice number
        unsigned int vertexIndex = vertexIndices[i];

        // get the corresponding x,y,z
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];

        // create a temp vertex with that x,y,z and color
        Vertex temp = { {vertex.x, vertex.y, vertex.z}, { r, g, b } };

        // push that vertice onto the out_vertice vector ( the vector being returned )
        out_vertices.push_back( temp );
       }

cout << "         Normal size: " << normalIndices.size() << endl;
    // For each normal of each triangle
    for( unsigned int i=0; i < normalIndices.size(); i++ )
       {
        // for each normal number
        unsigned int normalIndex = normalIndices[i];

        // get the corresponding normal numbers
        glm::vec3 normal = temp_normals[ normalIndex-1 ];

        // push that normal onto the out_normal vector ( the normal being returned )
        out_normals.push_back( normal );
       }



    return true;

}










































