#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp" //Makes passing matrices to shaders easier

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "texture.h"

using namespace std;

struct Point {

  // Information for a point (Vertex)
  glm::vec3 vertex; 
  glm::vec3 face; // fixme ; need this ? 
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 texture;

  Point() {}

  Point(const glm::vec3& v, const glm::vec3& f, const glm::vec3& n) 
    // : vertex(v), face(f), normal(n)
  {
    vertex = v;
    face = f;
    normal = n;
  }

};


struct Master{

// variables
std::vector<Point> data;


const char *  fileName;
const char *  name;
const char *  texName;

Assimp::Importer importer;

GLuint vbo;

Texture* tex;





// constructor
Master( const char* fName, const char* myName, const char* tName )
   {
    // grab variables
    fileName = fName;
    name = myName;
    texName = tName;

    cout << "My name is: " << name << endl;    

    // get the object's points
    loadObject();

    // get a color for every vertex
    colorizeByVertexPosition();

    // create vertex-buffer-object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Point), data.data(), GL_STATIC_DRAW);
    
    // texture 
    if ( !set_texture(texName) )
       {
        std::cout << texName << " could not be opened." << std::endl;   
       }

   } 


void resetTexture()
   {
/*
    // reset texture 
    if ( !set_texture(texName) )
       {
        std::cout << texName << " could not be opened." << std::endl;   
       }
*/
    tex->Bind( GL_TEXTURE_2D );
   }

bool set_texture(const char* FileName, GLenum TextureTarget=GL_TEXTURE_2D )
   {
    tex = new Texture(GL_TEXTURE_2D, FileName);
    return (!tex->Load()) ? false : true ;
   }

aiVector3D max_coords() 
// Return max x,y,z within all the vertices
{
    // get the scene
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals );

	aiVector3D max = scene->mMeshes[0]->mVertices[0];       
	for(unsigned int i = 0; i < scene->mNumMeshes; i ++) {
		const aiMesh* mesh = scene->mMeshes[i]; //aiMesh is a struct
		for(unsigned int j = 0; j < mesh->mNumVertices; j ++) { 
			aiVector3D t = mesh->mVertices[j];       
			if (t.x > max.x) max.x = t.x;
			if (t.y > max.y) max.y = t.y;
			if (t.z > max.z) max.z = t.z;
		} 
	}
	return max;
}


// load the object's verts and normals
void loadObject()
  {
    // get the scene
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals );

    glm::vec3 vertex; 
    glm::vec3 face;
    glm::vec3 normal;
    glm::vec2 texture;

    auto maxcoords = max_coords();
    
    cout << "The loader found: " << scene->mNumMeshes << " Meshes!" << endl;

    //for each mesh loaded -->there could be multiple meshes in one obj file
    for(unsigned int i = 0; i < scene->mNumMeshes; i ++) {
      const aiMesh* mesh = scene->mMeshes[i]; //aiMesh is a struct

      /* get the vertices */
      for(unsigned int j = 0; j < mesh->mNumVertices; j ++) { 
          data.push_back(Point());
          aiVector3D temp_vertex = mesh->mVertices[j];       
          vertex.x = temp_vertex.x;
          vertex.y = temp_vertex.y;
          vertex.z = temp_vertex.z;
          data[j].vertex = vertex;

          // data[j].texture = glm::vec2(  abs(vertex.x/maxcoords.x) , abs(vertex.z/maxcoords.z) );  
          data[j].texture = glm::vec2(  (vertex.x+maxcoords.x)/(2*maxcoords.x) , (vertex.z+maxcoords.z)/(maxcoords.z*2) );  

//cout << "tex x coord: " << data[j].texture.x << endl;
//cout << "tex y coord: " << data[j].texture.y << endl;

          //data[j].texture = glm::vec2(vertex.x, vertex.z);  // hackish
          // vertices.push_back(vertex);
      } 

      /* normals */
      if(mesh->HasNormals()) {
        for(unsigned int j = 0; j < mesh->mNumVertices; j ++) { //each vertex has a normal
          aiVector3D temp_vertex = mesh->mNormals[j];
          normal.x = temp_vertex.x;
          normal.y = temp_vertex.y;
          normal.z = temp_vertex.z;

          data[j].normal = normal;
        }
      }

    }

  }

void colorizeByVertexPosition()
  {
    Point* p = &data[data.size()-1];
    glm::vec3& v = p->vertex;
    GLfloat max_x, max_y, max_z;

    typedef std::vector<Point>::iterator piter;

    // find max x y z
    max_x = v.x;
    max_y = v.y;
    max_z = v.z;
    for (piter p = data.begin()+1; p != data.end(); p++) {
      glm::vec3& v = p->vertex;
      if (max_x < v.x) max_x = v.x;
      if (max_y < v.y) max_y = v.y;
      if (max_z < v.z) max_z = v.z;
    }

    // put color normalized w.r.t max_x max_y max_z
    for (piter p = data.begin()+1; p != data.end(); p++) {
      glm::vec3& v = p->vertex;
      glm::vec3& c = p->color;

      //c = glm::vec3( 0.0, 0.0, 1.0 );

      c.x =  v.x/max_x;
      c.y =  v.y/max_y;
      c.z =  v.z/max_z;

      //cout << "color : " << c.x << "," << c.y << "," << c.z << std::endl; 
    }

  }






};















