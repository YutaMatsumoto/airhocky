#include "texture.h"

struct Obj {

  std::vector<Point> data;
  size_t num_triangles; // keep size here so that glDrawArrays can be used without vector<Point>
  glm::mat4 world_matrix;
  Assimp::Importer importer;
  GLuint vbo;

  std::string name;
  Texture* tex;

  Obj(
    const std::string& fname, 
    const glm::mat4& modelMatrix, 
    const std::string& texfname,
    )
    : rigidbody(NULL), bco(NULL), tex(NULL)
  {
    name = "name;
    gWVP = modelMatrix; 

    // load mesh object
    createPoints(fname);
    // colorizeByVertexPosition();

    // create ogl vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(Point), data.data(), GL_STATIC_DRAW);

    // physics object
    //createBulletObject(shape);

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

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
    glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,texture)); // fixme color to textcoord
    glVertexAttribPointer(Normal,   3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,normal));

    // m_pTexture->Bind(GL_TEXTURE0); // fixme

    // light->SetWorldMatrix( Matrix4f(this->gWVP) );

    if (tex) {
      tex->Bind(GL_TEXTURE0);//m
    }
    else {
      std::cout << "No Texture Bound To " << name << std::endl; 
    }

    glDrawArrays(GL_TRIANGLES, 0, data.size() );  //mode, starting index, count // ORIGINAL

    glDisableVertexAttribArray(Position);
    glDisableVertexAttribArray(TexCoord);
    glDisableVertexAttribArray(Normal); 

                          
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
