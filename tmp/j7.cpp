// Search For 
// HERE FIXME

// TODO 
// Model Texture
// Texture Mapping
// disable colorizeByVertexPosition
// rotate
// load dragon -> slow
// 

/* TODO 
// pause / restart
// clear predicate and message
// menu
// mouse
// fix rotation
// fix restitution
// reshape
// quit does not work if ball has gone into the hole
// Makefile cleanup
// auto lib build
// should specify objects (ball and board when it runs on the command line)
// configure
// toggle pause and resume in menu
// start and finish
// 
// texture
// lighting
*/
/* BUG 
//
// Was using convexhull for board but the wall and the floor of the board makes
// it closed cube, but the inside of it must be empty, and should not make collisions in there.
// [せっかくだから俺はプログラマの道を選ぶぜ@wiki - Bullet その３　応用形状編](http://www11.atwiki.jp/darui_program/pages/328.html)
// [btCollisionShape from CC3MeshNode? « cocos2d for iPhone](http://www.cocos2d-iphone.org/forum/topic/19508)
// cause: was using convex hull for board with wall and floor
// 
*/
/* Task
// color or texture 
// 
// added body
// apply force and fall ball in idle()
// make board as a kinematic body and support the ball (ball should not fall)
  //sub make board first rotate and let it fall with the ball
  // NOT DONE
// make board rotate
// set friction bouncing coeff and so forth
// make board rotate
  // angular velocity
  // quaranteen
*/
/* References
[Collision Shapes - Physics Simulation Wiki](http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Shapes)
Bullet User Manual : constructor from triangle mesh : 32
[Box2D：４．b2Bodyで力・トルクを加える](http://flashjp.com/api/box2d_b2Body1.php)
[BtContactSolverInfo - Physics Simulation Wiki](http://bulletphysics.org/mediawiki-1.5.8/index.php/BtContactSolverInfo)
[Bullet Collision Detection & Physics Library: btBvhTriangleMeshShape Class Reference](http://bulletphysics.com/Bullet/BulletFull/classbtBvhTriangleMeshShape.html)
[Bullet Collision Detection & Physics Library: btConvexTriangleMeshShape Class Reference](http://bulletphysics.com/Bullet/BulletFull/classbtConvexTriangleMeshShape.html#_details)
[Bullet Collision Detection & Physics Library: btRigidBody Class Reference](http://bulletphysics.com/Bullet/BulletFull/classbtRigidBody.html#ac6e5f94e998150e37ca8962c851d10a8)
[Bullet Collision Detection & Physics Library: btTransform Class Reference](http://www.bulletphysics.com/Bullet/BulletFull/classbtTransform.html)
[Bullet Collision Detection & Physics Library: btTriangleMesh Class Reference](http://bulletphysics.com/Bullet/BulletFull/classbtTriangleMesh.html)
[Bullet Survival Kit – 4: Kinematics - OOgtech.org](http://www.oogtech.org/content/2011/09/07/bullet-survival-kit-4-the-motion-state/)
[Collision Callbacks and Triggers - Physics Simulation Wiki](http://bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Callbacks_and_Triggers)
[せっかくだから俺はプログラマの道を選ぶぜ@wiki - Bullet その３　応用形状編](http://www11.atwiki.jp/darui_program/pages/328.html)
[Collision Shapes - Physics Simulation Wiki](http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Shapes)
*/
/* To Consider/Do
// 
// Search for future extension by "FE :"
// 
// Make importer reference within OglObj ?
// Triangle should specify position_indices instead of posisions
// Program class which have shader handle and also drawing method
// Not sure centerOfMassOfTriangles() is correct
//
// Obj should have a physics bullet object and should updateMatrix from the matrix without communicating to the outside world
*/
/* Issues
// Vertex not yet to component of vbo
// Vertex should be another type that should be composed in vboComponent
// problem related to coordinate managements like center of mass of objects
// cannot use glm properly ; 
//
// Board is not static nor kinematic
// current implementation does not rotate board if the board is static (mass = 0)
//
// global matrices 
// 
// Using glDrawArrays instead of using IBO index buffer object
//
// Obj class assumes shader program is already uploaded and works
// 
// Colorizing things directly in fragment shader instead of using lighting and material.
// 
// Rotation problem
//
// Kinematic Object instead of dynamic 
//
// rotating board outside of physics simulation
*/ 
/* Past Problems
//
// Was using glUsePrograms for each vbo then nothing is drawn if two or more vbo's exist.
// 
// 
  Obj::data should not be a member but a temporary variable inside of the Obj private function
*/ 


#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <sstream>

// texture
#include "texture.h"

// Bullet
#include "../bullet/src/btBulletDynamicsCommon.h" // fixme
#include "../bullet/src/BulletCollision/CollisionShapes/btTriangleShape.h"

#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

// Assimp
#include "Importer.hpp"	
#include "postprocess.h"
#include "scene.h"
#include "DefaultLogger.hpp"
#include "LogStream.hpp"

#include "InitShader.h"

// function prototypes 
glm::mat4 getOglTransformMatrix(btRigidBody* body);

// global handels
float boardMass=1000;
float rotationAngle=0.1;

std::vector<btRigidBody*> board_rigid_bodies;
btRigidBody* ballbody;

// std::string ball_fname = "obj/ball.obj";
// std::string board_fname = "obj/board.obj";

GLuint h_program; // GLSL h_program handle
GLint h_mvp  = -1;    
GLint h_vpos = -1;
GLint h_vcolor = -1;

glm::mat4 ball_model_matrix;
glm::mat4 board_model_matrix;

glm::mat4 gProj, gWVP, gWorld;
glm::mat4 view;
// glm::mat4 view;
// glm::mat4 projection;
// glm::mat4 mvp; // projection * view * gWVP 

// other globals
// int w = 640, h = 480; // Window size
GLfloat rangle = 10;      // angle to rotate
int w = 500, h = 500;     // Window size
struct Obj;
Obj* pball=NULL;
Obj* pboard=NULL;
glm::vec3 max_coords = glm::vec3(10,10,10);
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

float n = 30;
// glm::vec3 look_eyepos = glm::vec3(n, 30.0, n);    // Eye Position
glm::vec3 look_eyepos = glm::vec3(0, 20.0, 0);    // Eye Position
float world_gravity= -5;
float ball_mass = 1;
float board_mass = 100;
float ball_initial_height = 1; // 0 5
float oldX, oldZ;
bool mouseMoved = false;

// struct MyVBO;
// std::vector<MyVBO> vbos;

struct Obj;
std::vector<Obj> objs;

struct RotateHelp;

// Bullet Globals
btDiscreteDynamicsWorld* dynamicsWorld;
btRigidBody* bulletBallBody = NULL;
btRigidBody* bulletBoardBody = NULL;


float ballRadius = -1;
GLint h_gWorld, h_gWVP;
GLint h_normal;


// NewBelow
// TODO : merge bind_shader_variables2 && init_shader_variables

float lightoff = 20;
float lightoffset_x = 0;
float lightoffset_y = 0;
float lightoffset_z = 0;

// game settings
bool game_done = false;
bool game_pause = false;
float grestitution = 0.8;
float gfriction = 0.3;
float rotationFactor = 0.1;
float addrotation = 0.1/180.0*M_PI;
float rotationHorizontal = 0, rotationVertical = 0;
float rotation = 0; 

std::string tex1 = "textures/test.png";
std::string tex2 = "textures/images.jpeg";
std::string tex3 = "textures/redTex.png";
std::string tex4 = "textures/blackTex.png";

// Handles to opengl variables
GLint 
  h_Position,
  h_TexCoord,
  h_Normal,
  h_LightPosition,
  h_AmbientProduct,
  h_DiffuseProduct,
  h_SpecularProduct,
  h_Shineness,
  h_gSampler;

GLint GetUniformLocation(GLint h_prog, const char* pUniformName)
{
    GLint Location = glGetUniformLocation(h_prog, pUniformName);

    if (Location == 0xFFFFFFFF)
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint GetAttribLocation(GLint h_prog, const char* variable_name)
{
    GLint Location = glGetAttribLocation(h_prog, variable_name);

    if (Location == 0xFFFFFFFF)
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", variable_name);
    }

    return Location;
}

void printLightPos() 
{

  std::cout 
    << "(" 
    << lightoffset_x << "," << lightoffset_y << "," << lightoffset_z
    << ")"
    << std::endl; 
}

enum GlslHandle { Position=0, TexCoord, Normal };
float end_height = 10;
enum Shape { None=0, ConvexHull, TriangleMesh };

bool is_game_complete(btRigidBody* body) 
{
  if (body) {
    glm::mat4 m = getOglTransformMatrix(body) ;
    // std::cout << m[3][1] << std::endl;
    // std::cout << "true condition" << ( m[3][1] < -20) << std::endl;
    // std::cout << m[3][1] << std::endl;  
    // std::cout << end_height << std::endl;  

    // this -20 should be parametrized in terms of the length of the board
    if ( end_height < m[3][1] ) {
      return true ;
    }
    else 
      return false ;
  }
  return false;
}

/* struct RotateHelp { 
  static int max_num_to_rotate;
  static float incremental_rotation;
  int num_to_rotate;
  int num_rotated;

  RotateHelp(int nrot) 
    : num_rotated(0)
    {
      num_to_rotate = (nrot%2==1) ? nrot+1 : nrot ; // make it even
    }

  RotateHelp& operator+=(const RotateHelp& o)
  {
    num_to_rotate += o.num_to_rotate;
    num_rotated += o.num_rotated;
    return *this;
  }

  float rotate_angle(int increments)
  {
    // Should not rotate more than specified in num_to_rotate
    int remaining = num_to_rotate - num_rotated;
    if (increments >  remaining) {
      increments = remaining;
    }
    // 
    int half = num_to_rotate/2;
    int angle = 0;
    for (int i = 0; i < increments; i++) {
      angle += (  ( num_rotated % (half) ) == num_rotated  )
        ? (num_rotated+1)*incremental_rotation
        : (  half - (num_rotated%half)  )*incremental_rotation;
        ;
    }
    std::cout << "angle: "<< angle<< std::endl; 
    return angle;
  }

};
int RotateHelp::max_num_to_rotate = 100;
float RotateHelp::incremental_rotation = 0.24 ;
*/
/* void rotateBody( btRigidBody* rb, const btScalar& yaw, const btScalar& roll, const btScalar& pitch)
// yaw roll pitch in radian
{

  // current transform
  btTransform trans;
  if (!rb) {
    trans = rb->getCenterOfMassTransform();
  }
  else 
    return;
  
  // rotation transform
  btTransform rotation;
  rotation.setIdentity();
  btQuaternion quat;
  quat.setEuler( yaw, roll, pitch); //or quat.setEulerZYX depending on the ordering you want
  rotation.setRotation(quat);

  // trans*=rotation;
  rotation *= trans;

  if (!bulletBoardBody )
    bulletBoardBody->setCenterOfMassTransform(rotation);

}
*/

float getDT()
{
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

btVector3 aiVector3TobtVector3(const aiVector3t<float>& av)
{
  return btVector3( av.x, av.y, av.z );
}

glm::mat4 oglMatrixToGLMMatrix4x4(btScalar* m)
{
	return glm::mat4(
    m[0]  , m[1]  , m[2]  , m[3]  , 
    m[4]  , m[5]  , m[6]  , m[7]  , 
    m[8]  , m[9]  , m[10] , m[11] , 
    m[12] , m[13] , m[14] , m[15]
  ); 
}

glm::mat4 getOglTransformMatrix(btRigidBody* body)
{

  btScalar m[16]; // transform matrix
  if ( body && body->getMotionState() ) {
    btTransform trans; 
    btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
    motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
  }
  else {
    std::cerr << "getOglTransformMatrix() error" << std::endl; 
    exit(1);
  }
  return oglMatrixToGLMMatrix4x4(m);
}

btVector3 glmvec3_to_btVector3(const glm::vec3& v)
{
  return btVector3( v.x, v.y, v.z );
}

float distance(glm::vec3 u, glm::vec3 v)
{
  return sqrt(
    ( u.x - v.x )*( u.x - v.x )+
    ( u.y - v.y )*( u.y - v.y )+
    ( u.z - v.z )*( u.z - v.z )
  );
}

// Printing

void debug_print_bullet_body_transform_matrix(btRigidBody* body)
{

  // Get Transform Matrix 
  btScalar m[16]; // transform matrix
  if (body && body->getMotionState()) {
    btTransform trans; 
    btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
    motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
  }
  else {
    std::cout << "body* passed is null" << std::endl; 
    // obj->getWorldTransform().getOpenGLMatrix(m);
  }
  // vbos[Ball].updateModelMatrix(oglMatrixToGLMMatrix4x4(m));
  // print transform matrix 
  std::cout << "matrix: " << std::endl;
  for (size_t i = 0; i < 4 ; i++) {
    for (size_t j = 0; j < 4 ; j++) {
      std::cout << m[i*4+j] << " "; 
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const aiVector3t<float>& v)
{
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const btVector3& v)
{
  os << "(" << v.getX() << "," << v.getY() << "," << v.getZ() << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat4& m)
{
  for(size_t i=0; i < m.row_size(); i++) { 
    os << '[';
    for(size_t j=0; j<m.col_size()-1; j++) { 
      os << m[i][j] << ',';
    }
    os << m[i][m.col_size()-1];
    os << ']';
  }
  return os; 
}


// Structures ----------------------------------------------------------

enum { Ball, Board, Paddle2, Puck }; 

struct Point {

  // HERE can glm::vec3 be used for Faces ? glm::vec3 not floats?
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

struct Obj {

  std::vector<Point> data;
  size_t num_triangles; // keep size here so that glDrawArrays can be used without vector<Point>
  btTriangleMesh bt_triangles;
  btRigidBody* rigidbody;
  glm::mat4 world_matrix;
  Assimp::Importer importer;
  GLuint vbo;
  // btCollisionShape bcs;
  btCollisionObject* bco;
  std::string name;
  Texture* tex;
  float xPos, zPos;

  Obj(
    const std::string& fname, 
    const glm::mat4& modelMatrix, 
    const std::string& texfname,
    btDiscreteDynamicsWorld* btworld,
    Shape shape=None
    )
    : rigidbody(NULL), bco(NULL), tex(NULL)
  {

    //modelMatrix = glm::scale(glm::vec4(1.0f,1.0), glm::vec3(10.0f));

    gWVP = modelMatrix; 
    xPos = 0;
    zPos = 0;

    // load mesh object
    createPoints(fname);
    // colorizeByVertexPosition();

    // create ogl vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Point), data.data(), GL_STATIC_DRAW);

    // physics object
    createBulletObject(shape);

    // texture

    if ( ! set_texture(texfname) )
    {
      std::cout << texfname << " could not be opened." << std::endl;   
    }

    // data.clear();

  }

  bool set_texture(const std::string& FileName, GLenum TextureTarget=GL_TEXTURE_2D)
  {
    tex = new Texture(GL_TEXTURE_2D, FileName.c_str());
    return (!tex->Load()) ? false : true ;
  }

  void set_name(const std::string& n) 
  {
    name = n;
  }

  void createBulletObject(Shape shape)
  {
    switch (shape) {
      case ConvexHull : {
        btConvexHullShape* bcs = new btConvexHullShape();
        for (size_t i=0; i<data.size(); i++) {
          glm::vec3& v = data[i].vertex;
          bcs->addPoint( glmvec3_to_btVector3(v) );
        }
        btScalar mass(ball_mass);
        btVector3 localInertia(0,0,0);
        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        // position and motion
        btTransform ballTransform;
        ballTransform.setIdentity();
        ballTransform.setOrigin(btVector3(0,ball_initial_height,0));
        btDefaultMotionState* bulletMotionState = new btDefaultMotionState(btTransform(ballTransform));
        // ball rigidbody info
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,bulletMotionState,bcs,localInertia);
        // tweak rigidbody info
        rbInfo.m_restitution = grestitution;
        rbInfo.m_friction = gfriction;
        // add ball to the world
        rigidbody = new btRigidBody(rbInfo);
        if(!rigidbody) std::cout << "bulletBallBody pointer null" << std::endl; 
        dynamicsWorld->addRigidBody(rigidbody);
        break;
      }
      case TriangleMesh : { 
        // Shape
        btBvhTriangleMeshShape* boardShape = new btBvhTriangleMeshShape( &bt_triangles, true );

        btScalar mass(board_mass);
        btVector3 localInertia(0,0,0);

        // transform : default position
        btTransform boardTransform;
        boardTransform.setIdentity();
        boardTransform.setOrigin(btVector3(0,0,0));
        btDefaultMotionState* bulletMotionState = new btDefaultMotionState(btTransform(boardTransform));
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,bulletMotionState,boardShape,localInertia);

        // tweak rigidbody properties
        rbInfo.m_restitution = grestitution;
        rbInfo.m_friction = gfriction;
        // add to the world
        rigidbody = new btRigidBody(rbInfo);
        if(!rigidbody) std::cout << "bulletBoardBody pointer null" << std::endl; 
        rigidbody->setActivationState(DISABLE_DEACTIVATION);
        dynamicsWorld->addRigidBody(rigidbody);
        break; 
      }
      case None : {
        break;
      }
    } // end switch
    
    

  }
  
  void move( float x, float z )
  {
    btVector3 temp = btVector3( x*getDT(), 0.0, z*getDT() );
    
    //rigidbody->translate( temp );
    
    rigidbody->applyForce(temp, btVector3(0.0, 0.0, 0.0));
  }

  // void updateModelMatrix(const glm::mat4 updatemodel)
  void updateModelMatrix()
  {
    if (rigidbody) {
      world_matrix = getOglTransformMatrix( this->rigidbody );
    }
    // std::cout << name << " : wm : " << world_matrix << std::endl; 
  }

  void createPoints(const std::string& fname)
  {
    
    // const aiScene* gWVP = importer.ReadFile(fname.c_str(), aiProcess_Triangulate | aiProcess_SortByPType);

    // const aiScene* gWVP = importer.ReadFile(fname.c_str(), aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    const aiScene* gWVP = importer.ReadFile(fname.c_str(), aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals );

    glm::vec3 vertex; 
    glm::vec3 face;
    glm::vec3 normal;
    glm::vec2 texture;
    
    //for each mesh loaded -->there could be multiple meshes in one obj file
    for(unsigned int i = 0; i < gWVP->mNumMeshes; i ++) {
      const aiMesh* mesh = gWVP->mMeshes[i]; //aiMesh is a struct

      /* get the vertices */
      for(unsigned int j = 0; j < mesh->mNumVertices; j ++) { 
          data.push_back(Point());
          aiVector3D temp_vertex = mesh->mVertices[j];       
          vertex.x = temp_vertex.x;
          vertex.y = temp_vertex.y;
          vertex.z = temp_vertex.z;
          data[j].vertex = vertex;
          data[j].texture = glm::vec2(vertex.x, vertex.z);  // hackish
          // vertices.push_back(vertex);
      } 

      /* get the faces */
      for(unsigned int j = 0; j < mesh->mNumFaces; j ++) {
          //aiProcess_Triangulate creates triangles out of the vertices and so
          //there are 3 indices per faces -- you can check for the number of
          //indices also with "mNumIndices" - data member aiFace 
          aiFace Face = mesh->mFaces[j];  
          btVector3 i,j,k;
          i = aiVector3TobtVector3(  mesh->mVertices[ Face.mIndices[0] ]  );
          j = aiVector3TobtVector3(  mesh->mVertices[ Face.mIndices[1] ]  );
          k = aiVector3TobtVector3(  mesh->mVertices[ Face.mIndices[2] ]  );
          bt_triangles.addTriangle(i,j,k);
          // faces.push_back(face);
      } 
      /* get the faces
      for(unsigned int j = 0; j < mesh->mNumFaces; j ++) {
          //aiProcess_Triangulate creates triangles out of the vertices and so
          //there are 3 indices per faces -- you can check for the number of
          //indices also with "mNumIndices" - data member aiFace 
          aiFace Face = mesh->mFaces[j];  
          face.x = Face.mIndices[0];      
          face.y = Face.mIndices[1];
          face.z = Face.mIndices[2];
          // faces.push_back(face);
      } */

      /* normals */
      // std::cout << "normal exists: " << mesh->HasNormals() << std::endl; 
      if(mesh->HasNormals()) {
        for(unsigned int j = 0; j < mesh->mNumVertices; j ++) { //each vertex has a normal
          aiVector3D temp_vertex = mesh->mNormals[j];
          normal.x = temp_vertex.x;
          normal.y = temp_vertex.y;
          normal.z = temp_vertex.z;
          data[j].normal = normal;
          // normals.push_back(normal);
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
      c.x =  v.x/max_x;
      c.y =  v.y/max_y;
      c.z =  v.z/max_z;
      // std::cout << "color : " << c << std::endl; 
    }

  }

  void draw() // fixme take handles as parameters
 {

    gWorld = world_matrix;
    gWVP = gProj * view * gWorld; 
    glUniformMatrix4fv(h_gWorld,  1, GL_FALSE, glm::value_ptr(gWorld) );
    glUniformMatrix4fv(h_gWVP,    1, GL_FALSE, glm::value_ptr(gWVP)   );

    glEnableVertexAttribArray(h_Position);
    glEnableVertexAttribArray(h_TexCoord);
    glEnableVertexAttribArray(h_Normal);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(h_Position, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
    glVertexAttribPointer(h_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,texture)); // fixme color to textcoord
    glVertexAttribPointer(h_Normal,   3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,normal));

    // m_pTexture->Bind(GL_TEXTURE0); // fixme

    // light->SetWorldMatrix( Matrix4f(this->gWVP) );

    if (tex) {
      tex->Bind(GL_TEXTURE0);//m
    }
    else {
      std::cout << "No Texture Bound To " << name << std::endl; 
    }

    glDrawArrays(GL_TRIANGLES, 0, data.size() );  //mode, starting index, count // ORIGINAL

    glDisableVertexAttribArray(h_Position);
    glDisableVertexAttribArray(h_TexCoord);
    glDisableVertexAttribArray(h_Normal); 

                          
  }


  void rotateBody(const btScalar& yaw, const btScalar& roll, const btScalar& pitch)
  // yaw roll pitch in radian
  {
    btRigidBody* rb = rigidbody;

    // current transform
    btTransform trans;
    trans = rb->getCenterOfMassTransform();
    
    // rotation transform
    btTransform rotation;
    rotation.setIdentity();
    btQuaternion quat;
    quat.setEuler( yaw, roll, pitch); //or quat.setEulerZYX depending on the ordering you want
    rotation.setRotation(quat);

    // update transform
    rotation *= trans;

    //  update COM
    rigidbody->setCenterOfMassTransform(rotation);

  }

};

// Function Implementations -------------------------------------------

// Callbacks
void display()
{
    //clear the screen
    // glClearColor(0.0, 0.0, 0.2, 1.0);
    glClearColor(1.0,1.0,0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //enable the shader program
    glUseProgram(h_program);

    gWVP = gProj * gWorld; 

    /* Draw Objects */
    for (size_t i = 0; i < objs.size(); i++) {
      objs[i].draw();
    }

    //swap the buffers
    glutSwapBuffers();
}

void idle()
{

  // objs[Ball].rigidbody->setDamping(0.1,0.1);

  // check game conditions
  game_done = is_game_complete(objs[Ball].rigidbody);

  // check 
  if ( game_pause || game_done ) {
    // std::cout << "inside" << std::endl;
    return;
  }

  if (mouseMoved)
    {
     glutWarpPointer((w/2), (h/2));
     mouseMoved = false;
     for(int i = 0; i < objs.size(); i++ )
       {
        objs[i].rigidbody->clearForces();
        if( i != 1 ) {
          objs[i].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
         }
        else {
          objs[i].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
         }
       }
    }

  /* rotate */
  rotationHorizontal -= rotationHorizontal*rotationFactor;
  rotationVertical   -= rotationVertical*rotationFactor;
  objs[Board].rotateBody( 0, 0, rotationHorizontal);
  objs[Board].rotateBody( 0, rotationVertical, 0);

  // activate 
  // objs[Board].rigidbody->setCollisionFlags( objs[Board].rigidbody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
  // objs[Board].rigidbody->activate(true); // or try… bulletBoardBody->forceActivationState(ACTIVE_FLAG)
  
  // bulletBallBody->setCollisionFlags( bulletBallBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
  // bulletBallBody->activate(true); // or try… bulletBallBody->forceActivationState(ACTIVE_FLAG)

  // Bullet Simulation 
  // apply force to ball and board 

  // bulletBallBody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
  // bulletBoardBody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );

  // Do Simulation
  dynamicsWorld->stepSimulation(1.f/60.f,10);

  // apply force and update gWVP matrices 
  if ( objs.size() >= 4 ) {
    objs[Ball].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
    objs[Paddle2].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
    objs[Puck].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
    objs[Board].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
  }

  for (size_t i = 0; i <objs.size(); i++) {
    objs[i].updateModelMatrix();
  }

  
  // debug
  // std::cout << "ball matrix"<< std::endl;  
  // glm::mat4 m = getOglTransformMatrix(bulletBallBody) ;
  // std::cout << m << std::endl; 


  glutPostRedisplay();

}

void reshape(int myW, int myH)
{ 
    w = myW;
    h = myH;

    oldX = w/2;
    oldZ = h/2;
    
    glViewport( 0, 0, w, h );
}

// Misc Functions
bool bind_shader_variables2(GLuint h_program)
{

  h_gWVP   = GetUniformLocation(h_program, "gWVP");
  // h_gWorld = GetUniformLocation(h_program, "gWorld");

  h_Position = GetAttribLocation( h_program, "Position");
  h_TexCoord = GetAttribLocation( h_program, "TexCoord");
  h_Normal   = GetAttribLocation( h_program, "Normal");

  h_AmbientProduct  = GetUniformLocation( h_program, "AmbientProduct");
  h_DiffuseProduct  = GetUniformLocation( h_program, "DiffuseProduct");
  h_SpecularProduct = GetUniformLocation( h_program, "SpecularProduct");
  h_LightPosition   = GetUniformLocation( h_program, "LightPosition");
  h_Shineness       = GetUniformLocation( h_program, "Shineness");
  h_gSampler        = GetUniformLocation( h_program, "gSampler");

  //enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return ( (h_gWorld && h_gWVP) ) ? true : false;

}

void init_shader_variables()
{

  glm::mat4 id = glm::mat4(1);

  view = glm::lookAt( 
    look_eyepos,
    glm::vec3(0.0, 0.0, 0.0),     // Focus point / Center
    glm::vec3(0.0, 0.0, -1.0)     // Up
  ); 

  gProj = glm::perspective(90.0f, float(w)/float(h), 1.0f, 100.0f);
  gWVP    = gProj * view * id;
  gWorld  =         view * id;

  glUniformMatrix4fv(h_gWVP,    1, GL_FALSE, glm::value_ptr(gWVP)   );
  glUniformMatrix4fv(h_gWorld,  1, GL_FALSE, glm::value_ptr(gWorld) );

  glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
  glUniform4f( h_AmbientProduct , 0.3, 0.3, 0.3, 1);
  glUniform4f( h_DiffuseProduct , 0.8, 0.8, 0.8, 1);
  glUniform4f( h_SpecularProduct, 0.8, 0.8, 0.8, 1);
  glUniform4f( h_LightPosition  , 0.8, 0.8, 0.8, 1);
  glUniform1f( h_Shineness      , 0.5 );

  glUniform1i( h_gSampler, 0);

}

// Keyboard Callbacks
void keyDown(unsigned char key, int x, int y)
{
  // float rotation = 10.0/180*M_PI;
  
  if (game_pause || game_done ) {
    return;
  }

  switch(key) {
    case 'Q' : exit(0);
    case 'q' : exit(0);
      break;

    /* rotate board with bullet directly */
    case 'h' : { // left 
      lightoffset_x+=lightoff;
      // objs[Board].rotateBody(0, 0, rotation);
      std::cout << "h hit h"<< std::endl; 
      break;
    }
    case 'l' : { // right
      lightoffset_x-=lightoff;
      std::cout << "h hit h"<< std::endl; 
      break;
    }
    case 'j' : { // up 
      lightoffset_z -= lightoff;
      std::cout << "j hit"<< std::endl; 
      break;
    }
    case 'k' : { // down
      lightoffset_z += lightoff;
      std::cout << "k hit"<< std::endl; 
      break;
    } 
    default:  {
      break;
    }

  }
  glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
  std::cout 
    << "(" 
    << h_LightPosition  << "," << lightoffset_x << "," << lightoffset_y << "," << lightoffset_z
    << ")"
    << std::endl; 

 mouseMoved = false;
 for(int i = 0; i < objs.size(); i++ )
       {
        objs[i].rigidbody->clearForces();
        if( i != 1 ) {
          objs[i].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
         }
        else {
          objs[i].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
         }
       }
}
void keyUp(unsigned char key, int x, int y)
{
 mouseMoved = false;
 for(int i = 0; i < objs.size(); i++ )
       {
        objs[i].rigidbody->clearForces();
        if( i != 1 ) {
          objs[i].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
         }
        else {
          objs[i].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
         }
       }
}
void keySpecialDown(int key, int x, int y)
{

  /*if (game_pause || game_done ) {
    return;
  }*/
  
  /* Rotate Camera */
  switch (key) {
    case GLUT_KEY_LEFT: { // translation : left 
      look_eyepos.x += -1.0;
      break;
    }
    case GLUT_KEY_RIGHT : { // translation : right
      look_eyepos.x += 1.0;
      break;
    }
    case GLUT_KEY_DOWN : { // translation : up 
      look_eyepos.z += -1.0;
      break;
    }
    case GLUT_KEY_UP : { // translation : down
      look_eyepos.z += 1.0;
      break;
    } 
    default:  {
      break;
    }
  }
  
  // bound camera movement
  if( look_eyepos.x > 15.0 ) {
    look_eyepos.x = 15.0;
  }
  else if( look_eyepos.x < -15.0 ) {
    look_eyepos.x = -15.0;
  }
  if( look_eyepos.z > 15.0 ) {
    look_eyepos.z = 15.0;
  }
  else if( look_eyepos.z < -15.0 ) {
    look_eyepos.z = -15.0;
  }
  
  // std::cout << "rotation" << rotationHorizontal << std::endl; 
  view = glm::lookAt( 
    look_eyepos, 
    glm::vec3(0.0, 0.0, 0.0),     // Focus point / Center
    glm::vec3(0.0, 0.0, -1.0)     // Up
   ); 
  
  mouseMoved =false;
  for(int i = 0; i < objs.size(); i++ )
       {
        objs[i].rigidbody->clearForces();
        if( i != 1 ) {
          objs[i].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
         }
        else {
          objs[i].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
         }
       }
}
void keySpecialUp(int key, int x, int y)
{
 mouseMoved =false;
 for(int i = 0; i < objs.size(); i++ )
       {
        objs[i].rigidbody->clearForces();
        if( i != 1 ) {
          objs[i].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
         }
        else {
          objs[i].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
         }
       }
}

void mouse(int button, int state, int x, int y)
{
   // Wheel reports as button 3(scroll up) and button 4(scroll down)
   if (button == 4) {
    look_eyepos.y += 1.0;
   }
   else if(button == 3){  
    look_eyepos.y += -1.0;
   }
   
   if( look_eyepos.y > 35.0 ) {
    look_eyepos.y = 35.0;
   }
   else if( look_eyepos.y < 10.0 ) {
    look_eyepos.y = 10.0;
   }
   
   view = glm::lookAt( 
    look_eyepos, 
    glm::vec3(0.0, 0.0, 0.0),     // Focus point / Center
    glm::vec3(0.0, 0.0, -1.0)     // Up
   ); 
   
   mouseMoved = false;
   for(int i = 0; i < objs.size(); i++ )
       {
        objs[i].rigidbody->clearForces();
        if( i != 1 ) {
          objs[i].rigidbody->applyForce ( btVector3( 0,world_gravity,0 ), btVector3(0,0,0) );
         }
        else {
          objs[i].rigidbody->applyForce ( btVector3( 0,-world_gravity,0 ), btVector3(0,0,0) );
         }
       }
}

void mouseMove( int x, int y )
{
    if( oldX - x > 0 )
       {
        //objs[Ball].xPos = 10.0;
        objs[Ball].move( 10.0, 0.0 );
       }
    else if( oldX - x < 0 )
       {
        //objs[Ball].xPos = -10.0;
        objs[Ball].move( -10.0, 0.0 );
       }
       
    if( oldZ - y > 0 )
       {
        //objs[Ball].zPos = 10.0;
        objs[Ball].move( 0.0, 10.0 );
       }
    else if( oldZ - y < 0 )
       {
        //objs[Ball].zPos = -10.0;
        objs[Ball].move( 0.0, -10.0 );
       }
       
       
    mouseMoved = true;
    oldX = w/2;
    oldZ = h/2;   
}

// 
void glutStuff(int argc, const char *argv[])
{

	glutInitWindowSize(900,600);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	// glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE );
	glutInit(&argc, const_cast<char**>(argv));

	glutCreateWindow("J7");


	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
  glutKeyboardFunc(keyDown);        // key down 
  glutKeyboardUpFunc(keyUp);        // key up  
  glutSpecialFunc(keySpecialDown);  // special key down 
  glutSpecialUpFunc(keySpecialUp);  // special key up
  glutPassiveMotionFunc(mouseMove); // for moving the mouse
  glutMouseFunc(mouse); // for scroll wheel
  
    glutSetCursor(GLUT_CURSOR_NONE); // hide cursor
  	glutWarpPointer((w/2), (h/2));

}



/* GLuint initGPU(const std::string& vshader, const std::string& fshader)
{
  // h_program = InitShader("../src/shader.vert", "../src/shader.frag"); 
  h_program = InitShader(vshader.c_str(), fshader.c_str()); 
  if(!h_program) {
    std::cerr << "h_program invalid" << std::endl; 
    exit(1);
  }
  // bind_shader_variables(h_program);
  return h_program;
}
*/

// ---
enum { Quit=1, Pause, Resume, XPlus, XMinus, YPlus, YMinus, ZPlus, ZMinus };
void menu(int id){
  switch (id) {
    case Quit : {
      exit(0);
      break;
    }
    case Pause : {
      game_pause = true;
      break;
    }
    case Resume : {
      game_pause = false;
      break;
    }
    case XPlus: {
      lightoffset_x+=lightoff;
      glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
      printLightPos();
      break;
    }
    case XMinus:{
      lightoffset_x-=lightoff;
      glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
      printLightPos();
      break;
                
    }
    case YPlus:{
      lightoffset_y+=lightoff;
      glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
      printLightPos();
      break;
    }
    case YMinus:{
      lightoffset_y-=lightoff;
      glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
      printLightPos();
      break;
    }
    case ZPlus:{
      lightoffset_z+=lightoff;
      glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
      printLightPos();
      break;
    }
    case ZMinus:{
      lightoffset_z-=lightoff;
      glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
      printLightPos();
      break;
    }
  }
  glutPostRedisplay();
} 

void createMenu(void)
{     
  glutCreateMenu(menu);
  // submenu_id = glutCreateMenu(menu);
  // glutAddMenuEntry("Sphere", 1);
  // menu_id = glutCreateMenu(menu);
  glutAddMenuEntry("Pause", Pause);
  glutAddMenuEntry("Resume", Resume);
  glutAddMenuEntry("Quit", Quit);
  glutAddMenuEntry("XPlus", XPlus);
  glutAddMenuEntry("XMinus", XMinus);
  glutAddMenuEntry("YPlus", YPlus);
  glutAddMenuEntry("YMinus", YMinus);
  glutAddMenuEntry("ZPlus", ZPlus);
  glutAddMenuEntry("ZMinus", ZMinus);
  // glutAddSubMenu("Draw", submenu_id);
  // glutAddMenuEntry("Quit", 0);     
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void print_opengl_version()
{
std::cout << glGetString(GL_VERSION) << std::endl;
}

// Main ---------------------------------------------------------------------
int main(int argc, const char *argv[])
{

  std::string vshader = argv[3];
  std::string fshader = argv[4];

  // RotateHelp boardrot(3);
  // std::cout << "rotate angle: " << boardrot.rotate_angle(5) << std::endl;  

  glutStuff(argc, argv);
	glewInit();
  // GLuint h_program = initGPU(argv[3], argv[4]); // specify vshader and fshader
  print_opengl_version();
  init_shader_variables();
  createMenu();


  // objs.push_back(dragon);

  // Bullet ---------------------------------------------------------------------
  /* Bullet Settings */
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
  btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
  btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
  /* Bullet World */
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,0,0));
	// btContactSolverInfo& info = dynamicsWorld->getSolverInfo();


  // Game Object ------------------------------------------------------------------
  glm::mat4 identity = glm::mat4();
  Obj ball(argv[1]             , identity, tex3, dynamicsWorld, ConvexHull); // CH
  Obj board(argv[2]            , identity, tex2, dynamicsWorld, TriangleMesh); // CH
  Obj paddle2("obj/Paddle.obj" , identity, tex3, dynamicsWorld, TriangleMesh);
  Obj puck("obj/Puck.obj"      , identity, tex4, dynamicsWorld, TriangleMesh);

  ball.set_name("Ball");
  board.set_name("Board");
  paddle2.set_name("Paddle2");
  puck.set_name("Puck");

  objs.push_back(ball);
  objs.push_back(board);
  objs.push_back(paddle2);
  objs.push_back(puck);

  objs[Paddle2].move(10.0/getDT(), 10.0/getDT());


  // Lighting --------------------------------------------------------------------
  // light = new LightingTechnique();
  // if ( ! light->Init( vshader.c_str(), fshader.c_str() ) ) { std::cerr << "LightingTechnique Init Failed "<< std::endl; }
  // init_shader_variables(light);
  // light->Enable(); // enable shader

  // Pipeline --------------------------------------------------------------------
  h_program = InitShader( vshader.c_str(), fshader.c_str() );
  bind_shader_variables2(h_program);
  init_shader_variables();

  
  glutMainLoop();


  return 0;
}

