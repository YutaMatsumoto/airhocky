glm::vec3 centerOfMassOfSimpleClosedPolygon(const std::string& fname)
{ 
  using namespace std;

  std::ifstream t(fname.c_str());
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  istringstream iss(str);
  string s; 
  float x,y,z;
  float a,b,c;
  int count = 0;
  x=y=z=0;
   
  while ( !iss.eof() ) {
    iss >> s;
    if (s=="v") {
      iss >> a >> b >> c;
      x += a;
      y += b;
      z += c;
      count++;
      
     // cout << "("<< a << ","<< b << ","<< c << ")"<< endl;
    }
  }

  x/=count;
  y/=count;
  z/=count;

  return glm::vec3(x,y,z);

  /*
  ifstream is;
  is.open (fname.c_str() );

  // get length of file:
  is.seekg (0, std::ios::end);
  long length = is.tellg();
  is.seekg (0, std::ios::beg);

  // allocate memory:
  char *buffer = new char [length];

  // read data as a block:
  is.read (buffer,length);

  // create string stream of memory contents
  // NOTE: this ends up copying the buffer!!!
  istringstream iss( string( buffer ) );

  // delete temporary buffer
  delete [] buffer;

  // close filestream
  is.close();

  // ==================================
  //  Use iss to access data
  string s;
  iss >> s;
  cout << s << endl;
  */

  /*
  std::ifstream file( fname.c_str() );
  if ( file ) {
    // associate buffer to the file
    std::istringstream buf;
    
    file.close();
    // 
    while ( 1 ) {
      std::string s;
      float  f;
      buf >> s;
      buf >> f;
      std::cout << s << " " << f << std::endl;
    }
  }
  else {
    std::cerr << fname << " could not be opened" << std::endl;
  }
  */

}


/*
template <class T> struct Vertex {
  T pos[3];
  T color[3];

  Vertex()
  {
    size_t i;
    for (i = 0; i < sizeof(pos)/sizeof(T); i++) {
      pos[i] = 0;
    }
    for (i = 0; i < sizeof(color)/sizeof(T); i++) {
      color[i] = 0;
    }
  }

  Vertex operator+(const Vertex<T>& o) 
  {
    Vertex v;
    for (size_t i = 0; i < sizeof(pos)/sizeof(T); i++) {
      v.pos[i] = pos[i] + o.pos[i];
    }
    return v;
  }

  Vertex(const Vertex& v) 
  {
    size_t i;
    for (i = 0; i < sizeof(pos)/sizeof(T); i++) {
      pos[i] = v.pos[i];
    }
    for (i = 0; i < sizeof(color)/sizeof(T); i++) {
      color[i] = v.color[i];
    }
  }

  Vertex& operator=(const Vertex& v)
  {
    if (this == &v) return *this;

    size_t i;
    for (i = 0; i < sizeof(pos)/sizeof(T); i++) {
      pos[i] = v.pos[i];
    }
    for (i = 0; i < sizeof(color)/sizeof(T); i++) {
      color[i] = v.color[i];
    }
    return *this;
  }

  btVector3 posToBulletVector3() const
  {
    return btVector3(pos[0], pos[1], pos[2]);  
  }

  glm::vec3 posToGLMVec3() const
  {
    return glm::vec3(pos[0], pos[1], pos[2]);
  }
  
  void debug_print() const
  {
    std::cout  
          << "pos=(" 
            << pos[0] << ", " 
            << pos[1] << ", " 
            << pos[2] 
          << "), "
          << "color=(" 
            << color[0] << ", "    
            << color[1] << ", "    
            << color[2]
          << ")" 
    << std::endl;

  }


};

typedef Vertex<GLfloat> VertFloat;

template <class T> struct Triangle {
  Vertex<T> verts[3];

  Triangle(const Triangle& t)
  {
    for (size_t i = 0; i < sizeof(verts)/sizeof(Vertex<T>); i++) {
        verts[i] = t.verts[i];
    }
  }

  Vertex<T> centerOfMass() 
  {
    Vertex<T> v;
    for (size_t i = 0; i < sizeof(verts)/sizeof(Vertex<T>); i++) {
      v = v + verts[i];
    }
    return v;
  }

  Triangle& operator=(const Triangle& t)
  {
    if (this == &t) return *this;

    size_t i;
    for (i = 0; i < sizeof(verts)/sizeof(Vertex<T>); i++) {
      verts[i] = t.verts[i];
    }
    return *this;
  }

  Triangle() 
  { 
    for (size_t i = 0; i < sizeof(verts)/sizeof(Vertex<T>); i++) {
      verts[i] = Vertex<T>();
    }
    // verts = { 0,0,0};
    // verts[0] = 0;
    // verts[1] = 0;
    // verts[2] = 0;
  }
  void debug_print()
  {
    for (size_t i = 0; i < 3; i++) {
        verts[i].debug_print();
    }
  }

};

typedef Triangle<GLfloat> TrigFloat;

typedef std::vector<Triangle<GLfloat> > Triangles;
*/

btVector3 centerOfMassOfTriangles(const Triangles& t) // Should NOT work for non-spherical objects
{
  size_t i,j;
  btVector3 centerOfMass(0,0,0);
  for (i = 0; i < t.size(); i++) {
    for (j = 0; j < 3; j++) {
      centerOfMass += t[i].verts[j].posToBulletVector3();
    }
  }
  // std::cerr << "centerOfMassTOtal: " << centerOfMass.getX() << ", " << centerOfMass.getY() << ", " << centerOfMass.getZ() << ", " << std::endl;
  return centerOfMass/(t.size()*3); 
}
/*
struct Drawer {
  // FE : this class
  GLuint vbo;
  GLuint h_program;
};

struct MyVBO {
  // FE : This class should have reference to std::vector<Triangle<?> > or some class instead of numTriangles
  // FE : This class should not have draw() method
  GLuint vbo;
  GLuint vbosize;
  GLuint h_program;
  GLuint numTriangles;
  glm::mat4 model; 
  // Drawer drawer;

  MyVBO(GLuint h_program, const std::vector<Triangle<GLfloat> >& triangles) 
  {
    vbosize = triangles.size()*(sizeof(Triangle<GLfloat>));
    numTriangles = triangles.size();
    // std::cerr << "trig vbosize: " << triangles.vbosize() << std::endl;
    // std::cerr << "sizeof Triangle: " << sizeof(Triangle<GLfloat>) << std::endl;
    // std::cerr << "sizeof Vertex: " << sizeof(Vertex<GLfloat>) << std::endl;
    // std::cerr << "sizeof Vertex: " << sizeof(GLfloat)*6 << std::endl;
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vbosize, triangles.data(), GL_STATIC_DRAW);

    model = glm::mat4(1); // set to identity
  }

  void updateModelMatrix(const glm::mat4 updatemodel)
  {
    model = updatemodel;
  }

  void draw()
  {

    // glUseProgram(h_program);
    glUniformMatrix4fv(h_model, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(h_view, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(h_projection, 1, GL_FALSE, glm::value_ptr(projection));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(h_vpos);
    glEnableVertexAttribArray(h_vcolor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( h_vpos,//location of attribute
                            3,//number of elements
                            GL_FLOAT,//type
                            GL_FALSE,//normalized?
                            sizeof(Vertex<GLfloat>),//stride
                            0 //offset
                            );

    glVertexAttribPointer( h_vcolor, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex<GLfloat>), (void*)offsetof(Vertex<GLfloat>,color));

    // Draw
    // glDrawArrays(GL_TRIANGLES, 0, triangles.vbosize()*3);//mode, starting index, count // ORIGINAL
    // std::cerr << "numT" << numTriangles << std::endl;
    glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);//mode, starting index, count // ORIGINAL

    //clean up
    glDisableVertexAttribArray(h_vpos);
    glDisableVertexAttribArray(h_vcolor);
                          
  }
  
};
*/
/* old */ struct MyAssimp {
  aiScene* scene;
  Assimp::Importer importer;
  // std::vector<Triangle<GLfloat> > triangles;
MyAssimp(const std::string& fname, unsigned int pFlags) 
  {
    scene = const_cast<aiScene*>( importer.ReadFile( fname.c_str(), pFlags));
    if (!scene) { 
      std::cout << "Scene Error" << " in the file : " << fname << " " << std::endl; 
    }
  }
~MyAssimp() 
{
  std::cerr << "~MyAssimp()" << std::endl;
}
std::vector<Triangle<GLfloat> > createTriangles()
{
  std::vector<Triangle<GLfloat> > triangles;
  createTrianglesRecursive(triangles, scene, scene->mRootNode);
  return triangles; 
}
static btVector3 aiVector3TObtVector3(const aiVector3t<float>& in) 
{
  return btVector3( in.x, in.y, in.z); 
}

  void createTrianglesRecursive(std::vector<Triangle<GLfloat> >& triangles, const struct aiScene *sc, const struct aiNode* node)
  { 
    // Sanko: assimp SimpleOpenGL recursive render

    // aiScene has : Node -> Meshes > Facees > Vertices
    

    // Process Meshes 
    // mind = mesh index
    for (unsigned int mind=0; mind < node->mNumMeshes; ++mind) {
		  const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[mind]];
      std::cout << "numFaces : " << mesh->mNumFaces << std::endl;
      // Process faces in a mesh
      // find == face index
      for (unsigned int find = 0; find < mesh->mNumFaces; ++find) {
        const struct aiFace* face = &mesh->mFaces[find];
        // Error Handling
        if (face->mNumIndices!=3) {
          std::cerr << "face has more than 3 vertices" << std::endl;
          exit(1);
        }
        // Store vertices and colors of a face/triangle'
        // vind = vertex index
        triangles.push_back(Triangle<GLfloat>());
        for (unsigned int vind = 0; vind < face->mNumIndices; vind++) {
          // dbeug
          // std::cerr
          //   << "face->mIndices[vind]: " 
          //   << face->mIndices[vind]
          //   << std::endl; 
          // vertices
          triangles[triangles.size()-1].verts[vind].pos[0] = mesh->mVertices[ face->mIndices[vind] ].x ;
          triangles[triangles.size()-1].verts[vind].pos[1] = mesh->mVertices[ face->mIndices[vind] ].y ;
          triangles[triangles.size()-1].verts[vind].pos[2] = mesh->mVertices[ face->mIndices[vind] ].z ;
          // colors
          triangles[triangles.size()-1].verts[vind].color[0] = abs( (mesh->mVertices[ face->mIndices[vind] ].x - max_coords.x) / max_coords.x ); // x color
          triangles[triangles.size()-1].verts[vind].color[1] = abs( (mesh->mVertices[ face->mIndices[vind] ].y - max_coords.y) / max_coords.y ); // y color
          triangles[triangles.size()-1].verts[vind].color[2] = abs( (mesh->mVertices[ face->mIndices[vind] ].z - max_coords.z) / max_coords.z ); // z color
          // triangles[triangles.size()-1].verts[vind].color[1] = 1; // z color constant
        }
      }
    }

    // process all children
    for (unsigned int mind = 0; mind < node->mNumChildren; ++mind) {
      createTrianglesRecursive(triangles, sc, node->mChildren[mind]);
    }

  }

  aiVector3t<float> centerOfMass()
  { // Calculates the approximate center of mass from the vertices assuming the
    // mass is uniformly distributed. 
    //
    // Works if all the vertices owned by meshes are unique. If two meshes
    // share one vertex or more, then the center of mass is not calculated
    // correctly.

    if (!scene) {
      std::cerr << "error scene is null " << std::endl;
    }

    aiVector3D total(0,0,0);
    unsigned int cnt = 0;
    aiNode* node = scene->mRootNode;

    /* Process Mesh
    for (unsigned int mind=0; mind < node->mNumMeshes; ++mind) {
		  const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[mind]];
      // std::cout << "numFaces : " << mesh->mNumFaces << std::endl;
      // Process faces in a mesh
      // find == face index
      for (unsigned int find = 0; find < mesh->mNumFaces; ++find) {
        const struct aiFace* face = &mesh->mFaces[find];
        // Process Vertices 
        // vind = vertex index
        for (unsigned int vind = 0; vind < face->mNumIndices; vind++) {
          aiVector3t<float> local(
              mesh->mVertices[ face->mIndices[vind] ].x, 
              mesh->mVertices[ face->mIndices[vind] ].y,
              mesh->mVertices[ face->mIndices[vind] ].z
          );
          total += local;
          cnt++;
        }
      }
    }
    */
    /* Process Mesh 2 */
    std::cerr << "mNumMeshes: " << node->mNumMeshes << std::endl;
    for (unsigned int mind=0; mind < node->mNumMeshes; ++mind) {
		  const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[mind]];
      for (unsigned int vind = 0; vind < mesh->mNumVertices; ++vind) {
        const aiVector3D v = mesh->mVertices[vind];
        total += v; 
        cnt++;
      }
    }

    std::cerr << "cnt: " << cnt << std::endl;
    if (cnt==0) {
      total/=cnt;
      return total;
    }
    else {
      return total;
    }
  }

  /*
  void debug_print()
  {
    // std::cerr << "vbo: " << vbo << std::endl; 
    for (size_t i = 0; i < triangles.size(); i++) {
      triangles[i].debug_print();
    }
    std::cout << "-------------------------- tri size: "<< triangles.size() << std::endl;
  }
  */

  // btRigidBody toBulletSphere() {}
  // btRigidBody toBulletTriangles() {}
};

  // [3. OpenGL Keyboard Interaction (Version 2.0) – Swiftless Tutorials - Game Programming and Computer Graphics Tutorials](http://www.swiftless.com/tutorials/opengl/keyboard.html)
  /* rotate models 
  switch (key) {
    case GLUT_KEY_LEFT: {
      model = glm::rotate( model, rangle, glm::vec3(0.0f, 0.0f, 1.0f) );
      break;
    }
    case GLUT_KEY_RIGHT: {
      model = glm::rotate( model, -rangle, glm::vec3(0.0f, 0.0f, 1.0f) );
      break;
    }
    case GLUT_KEY_DOWN: {
      model = glm::rotate( model, rangle, glm::vec3(1.0f, 0.0f, 0.0f) );
      break;
    }
    case GLUT_KEY_UP: {
      model = glm::rotate( model, -rangle, glm::vec3(1.0f, 0.0f, 0.0f) );
      break;
    }
    default: {
      break;
    }
  }
  */
  // [Physics Simulation Forum • View topic - Rotations of Objects](http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=5182)
  // btTransform T(btQuaternion(btVector3(0,1,0),btRadians(60)),btVector3(0.0,0.5,0));
    /* 
    case GLUT_KEY_LEFT: {
      for (size_t i = 0; i < board_rigid_bodies.size(); i++) {
        btRigidBody* body = board_rigid_bodies[i];
        btMotionState* motion = body->getMotionState();
        btTransform T;
        motion->getWorldTransform(T);

        btQuaternion rotation = T.getRotation();
        // std::cout << "Quarternian: " << rotation.getAxis() << std::endl;
        btVector3 offset = T.getOrigin();
        btQuaternion rotOff( btVector3(0,0,1), M_PI/180.0*rotationAngle );
        rotation *= rotOff;
        T.setOrigin(btVector3(0., 0., 0.));
        T.setRotation(rotation);
        T.setOrigin(offset);

        motion->setWorldTransform(T);
      }
      break;
    }
    case GLUT_KEY_RIGHT: {
    */
    // [Physics Simulation Forum • View topic - Translating and rotating objects by mouse](http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6282)
    // FIXME do btTransform::setRotation to set the axis of rotation
      /* rotation scheme 1
      for (size_t i = 0; i < board_rigid_bodies.size(); i++) {
        btRigidBody* body = board_rigid_bodies[i];
        btMotionState* motion = body->getMotionState();
        btTransform T;
        motion->getWorldTransform(T);

        btQuaternion rotation = T.getRotation();
        // std::cout << "Quarternian: " << rotation.getAxis() << std::endl;
        btVector3 offset = T.getOrigin();
        rotation.setRotation( btVector3(0,0,-1), rotation.getAngle() + M_PI/180.0*rotationAngle );
        T.setOrigin(btVector3(0., 0., 0.));
        T.setRotation(rotation);
        T.setOrigin(offset);

        motion->setWorldTransform(T);
      }
      */
      /* rotation scheme 2 
      for (size_t i = 0; i < board_rigid_bodies.size(); i++) {
        btRigidBody* body = board_rigid_bodies[i];
        btMotionState* motion = body->getMotionState();
        btTransform T;
        motion->getWorldTransform(T);

        btQuaternion rotation = T.getRotation();
        // std::cout << "Quarternian: " << rotation.getAxis() << std::endl;
        btVector3 offset = T.getOrigin();
        btQuaternion rotOff( btVector3(0,0,-1), M_PI/180.0*rotationAngle );
        rotation *= rotOff;
        T.setOrigin(btVector3(0., 0., 0.));
        T.setRotation(rotation);
        T.setOrigin(offset);

        motion->setWorldTransform(T);
      }
      break;
    }
    case GLUT_KEY_DOWN: {
      for (size_t i = 0; i < board_rigid_bodies.size(); i++) {
        btRigidBody* body = board_rigid_bodies[i];
        btMotionState* motion = body->getMotionState();
        btTransform T;
        motion->getWorldTransform(T);

        btQuaternion rotation = T.getRotation();
        // std::cout << "Quarternian: " << rotation.getAxis() << std::endl;
        btVector3 offset = T.getOrigin();
        btQuaternion rotOff( btVector3(1,0,0), M_PI/180.0*rotationAngle );
        rotation *= rotOff;
        T.setOrigin(btVector3(0., 0., 0.));
        T.setRotation(rotation);
        T.setOrigin(offset);

        motion->setWorldTransform(T);
      }
      break;
    }
    case GLUT_KEY_UP: {
      for (size_t i = 0; i < board_rigid_bodies.size(); i++) {
        btRigidBody* body = board_rigid_bodies[i];
        btMotionState* motion = body->getMotionState();
        btTransform T;
        motion->getWorldTransform(T);

        btQuaternion rotation = T.getRotation();
        // std::cout << "Quarternian: " << rotation.getAxis() << std::endl;
        btVector3 offset = T.getOrigin();
        btQuaternion rotOff( btVector3(-1,0,0), M_PI/180.0*rotationAngle );
        rotation *= rotOff;
        T.setOrigin(btVector3(0., 0., 0.));
        T.setRotation(rotation);
        T.setOrigin(offset);

        motion->setWorldTransform(T);
      }
      break;
    }
    default: {
      break;
    }
      */

idle()
  // bulletBoardBody->applyForce( btVector3( 0,-h*4,0 ), btVector3(t,0,0) );
  // bulletBoardBody->applyForce( btVector3( 0,h*4,0 ), btVector3(t,0,0) );
  // bulletBoardBody->applyForce( btVector3( 0,-h*4,0 ), btVector3(0,0,t*10) );
  // bulletBoardBody->applyForce( btVector3( 0,h*4,0 ), btVector3(0,0,0) );
    // bulletBoardBody->applyTorque( btVector3( 0,1000,100 ) );
  // bulletBoardBody->activate();
  // bulletBoardBody->applyTorque( btVector3(100000000000000,-10000,0) );
  // btVector3 yawrollpitch(1.0/180*M_PI,0,0);

  // bulletBoardBody->applyForce( btVector3( 0,t*4,0 ), btVector3(t,0,0) );
  // bulletBoardBody->applyForce( btVector3( t*4,0,0 ), btVector3(t,0,0) );
  // bulletBoardBody->applyTorque( btVector3( h*4000000,0,0 ) );
  /* Update Ball
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[Ball];
  btRigidBody* body = btRigidBody::upcast(obj);
  */
  // body->setGravity(btVector3(0,-10,0));
  /* Get Transform Matrix
  if (body && body->getMotionState()) {
    btTransform trans; 
    btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
    motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
  }
  else {
    obj->getWorldTransform().getOpenGLMatrix(m);
  }
  vbos[Ball].updateModelMatrix(oglMatrixToGLMMatrix4x4(m));
  */
  // Update Board : FIXME Only One Trianle needs to be updated instead of all the triangles?
  /*
  for (size_t i = 0; i < board_rigid_bodies.size() ; i++) {
    // btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[Board];
    // btRigidBody* body = btRigidBody::upcast(obj);
    btRigidBody* body = board_rigid_bodies[i];
    // Get Transform Matrix
    if (body && body->getMotionState()) {
      btTransform trans; 
      btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
      motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
    }
    else {
      obj->getWorldTransform().getOpenGLMatrix(m);
    }
  }
  // form model matrix
  vbos[Board].updateModelMatrix(oglMatrixToGLMMatrix4x4(m));
  */
  /* debug print */
  // debug_print_bullet_body_transform_matrix(bulletBallBody);
  /* print transform matrix
  std::cerr << "matrix: " << std::endl;
  for (size_t i = 0; i < 4 ; i++) {
    for (size_t j = 0; j < 4 ; j++) {
      std::cout << m[i*4+j] << " "; 
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  */

  /* Bullet Simulation : Make Ball Fall 
		dynamicsWorld->stepSimulation(1.f/60.f,10);
    // std::cout << getMatrix( bulletBallBody->getCenterOfMassTransform() ) << std::cout;

    btScalar m[16]; // trnsform matrix
		//print positions of all objects
		for (int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--) {
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
      // Get Transform Matrix
			if (body && body->getMotionState()) {
				btTransform trans;
        btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
        motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
			}
      else {
        obj->getWorldTransform().getOpenGLMatrix(m);
      }
      // print transform matrix
      std::cerr << "matrix: " << std::endl;
      for (size_t i = 0; i < 4 ; i++) {
        for (size_t j = 0; j < 4 ; j++) {
          std::cout << m[i*4+j] << " "; 
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }
    // form model matrix
    model = oglMatrixToGLMMatrix4x4(m);
  */
  /* Bullet Simulation : Make Only Ball Fall 
  dynamicsWorld->stepSimulation(1.f/60.f,10);
  btScalar m[16]; // trnsform matrix
  btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[Ball];
  btRigidBody* body = btRigidBody::upcast(obj);
  // Get Transform Matrix
  if (body && body->getMotionState()) {
    btTransform trans; 
    btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
    motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
  }
  else {
    obj->getWorldTransform().getOpenGLMatrix(m);
  }
  // print transform matrix
  std::cerr << "matrix: " << std::endl;
  for (size_t i = 0; i < 4 ; i++) {
    for (size_t j = 0; j < 4 ; j++) {
      std::cout << m[i*4+j] << " "; 
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  // form model matrix
  vbos[Ball].updateModelMatrix(oglMatrixToGLMMatrix4x4(m));
  */

keydown()
    /* translation NOT WORKING : FIXME
    case 'h' : { // translation : left 
      btVector3 translate_parallel_amount_left(-1,0,0);
      for (size_t i = 0; i < board_rigid_bodies.size(); i++) {
        board_rigid_bodies[i]->translate(translate_parallel_amount_left);
      }
      break;
    }
    case 'l' : { // translation : right
      btVector3 translate_parallel_amount_right(1,0,0);
      for (size_t i = 0; i < board_rigid_bodies.size(); i++)
        board_rigid_bodies[i]->translate(translate_parallel_amount_right);
      break;
    }
    case 'j' : { // translation : up 
      btVector3 translate_parallel_amount_up(1,0,0);
      for (size_t i = 0; i < board_rigid_bodies.size(); i++)
        board_rigid_bodies[i]->translate(translate_parallel_amount_up);
      break;
    }
    case 'k' : { // translation : down
      btVector3 translate_parallel_amount_down(1,0,0);
      for (size_t i = 0; i < board_rigid_bodies.size(); i++)
        board_rigid_bodies[i]->translate(translate_parallel_amount_down);
      break;
    }
    */
    /* rotate board with bullet by [Physics Simulation Forum • View topic - applyForce](http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=2995)
    case 'h' : { // translation : left 
      btVector3 force(0,10000,0);
      btVector3 rel_pos(0,10,0);
      bulletBoardBody->applyCentralForce(force);
      btTransform rotate_with_body;
      rotate_with_body.setIdentity();
      rotate_with_body.setRotation( bulletBoardBody->getCenterOfMassTransform().getRotation() );
      // bulletBoardBody->applyTorque(rotate_with_body(rel_pos).cross(force)*m_angularFactor);
      bulletBoardBody->applyTorque(rotate_with_body(rel_pos).cross(force)*bulletBoardBody->getAngularFactor());
      std::cout << "h hit h"<< std::endl; 
      break;
    }
    case 'l' : { // translation : right
      break;
    }
    case 'j' : { // translation : up 
      break;
    }
    case 'k' : { // translation : down
      break;
    } 
    default:  {
      break;
    }
    */
    /* rotate board with bullet by applying force 
    case 'h' : { // translation : left 
      bulletBoardBody->applyForce( btVector3(0,-10000,0), btVector3(0,3,0) );
      std::cout << "h hit h"<< std::endl; 
      break;
    }
    case 'l' : { // translation : right
      break;
    }
    case 'j' : { // translation : up 
      break;
    }
    case 'k' : { // translation : down
      break;
    } 
    default:  {
      break;
    }
    */
    /* rotate board with bullet by applying torque
    case 'h' : { // translation : left 
      bulletBoardBody->applyTorque( btVector3(100000000000000,-10000,0) );
      std::cout << "h hit h"<< std::endl; 
      break;
    }
    case 'l' : { // translation : right
      break;
    }
    case 'j' : { // translation : up 
      break;
    }
    case 'k' : { // translation : down
      break;
    } 
    default:  {
      break;
    }
    */
    /* rotate board with bullet by setAngularVelocity
    case 'h' : { // translation : left 
      bulletBallBody->setAngularVelocity(btVector3(10000,0,0));
      std::cout << "h hit h"<< std::endl; 
      break;
    }
    case 'l' : { // translation : right
      break;
    }
    case 'j' : { // translation : up 
      break;
    }
    case 'k' : { // translation : down
      break;
    } 
    default:  {
      break;
    }
    */
main

  // Create Objects 
  /*
  unsigned int pFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
  MyAssimp ball(ball_fname, pFlags);    // ball
  MyAssimp board(board_fname, pFlags);  // board
  */
  /*
  // MyAssimp ball("obj/bunny.obj", pFlags);
  // MyAssimp ball("obj/ball.obj", pFlags);
  // Obj ball("obj/ball.obj", pFlags, PhysObj() );
  // Obj ball("obj/bunny.obj", pFlags, PhysObj() );
  // Obj ball("obj/airboat.obj", pFlags, PhysObj() );
  // Obj ball("obj/bunny.obj", pFlags, PhysObj() );
  */
  /*
  std::vector<Triangle<GLfloat> > ball_triangles = ball.createTriangles();
  std::vector<Triangle<GLfloat> > board_triangles = board.createTriangles();

  vbos.push_back( MyVBO(h_program, ball_triangles ) );
  vbos.push_back( MyVBO(h_program, board_triangles ) );
  */
  /*
  // btVector3 ballCenterOfMass = centerOfMassOfTriangles(ball_triangles);
  // ballCenterOfMass += btVector3(0,1,0);

  // Vertex<float> v; v.pos[0] = 1;
  // std::cout << "v: "<< v.posToBulletVector3() << std::endl;
  
  // Create center of mass and radius of ball
  // btVector3 t = ball_triangles[0].verts[0].posToBulletVector3();

  
  // btVector3 t = ball_triangles[0].verts[0].posToBulletVector3();
  glm::vec3 v = ball_triangles[0].verts[0].posToGLMVec3();                    // sphere : one point on the sphere
  glm::vec3 ballCenterOfMass_glm = centerOfMassOfSimpleClosedPolygon(ball_fname); // sphere : cenetr of mass 
  ballRadius = distance( ballCenterOfMass_glm, v );
  btVector3 ballCenterOfMass = btVector3(ballCenterOfMass_glm.x, ballCenterOfMass_glm.y, ballCenterOfMass_glm.z);
  std::cerr << "ballRadius: " << ballRadius << std::endl; 
  std::cerr << "centerofmass: " << ballCenterOfMass << std::endl; 
  */

  /* center of mass asssimp NOT WORKING 
  std::cout << "centerofmass assimp: " << ball.centerOfMass() << std::endl;
  */
  /* debug : centerOfMassOfSimpleClosedPolygon 
  std::cout << "centerOfMassOfSimpleClosedPolygon:ball : "<< centerOfMassOfSimpleClosedPolygon("obj/ball.obj") << std::endl;
  std::cout << "centerOfMassOfSimpleClosedPolygon:board : "<< centerOfMassOfSimpleClosedPolygon("obj/board.obj") << std::endl;
  */
  /* debug : centerOfMassOfTriangles
  std::cout << "ballCenterOfMass: " << ballCenterOfMass << std::endl;
  */
  /* debug : center of sphere
  
  btVector3 b = centerOfMassOfTriangles( ball. createTriangles() );
  std::cout << "center of sphere"
     << '(' 
    << b.getX() << ","
    << b.getY() << ","
    << b.getZ() << ","
    << ')' 
    << std::endl;
    */
  /* debug : ball triangles
    Triangles bt = ball.createTriangles();
    for (size_t i = 0; i < bt.size(); i++) bt[i].debug_print();
  */
  /* debug : triangles
    for (size_t i = 0; i < t.size(); i++) t[i].debug_print();
  */

  // dynamicsWorld->debugDrawWorld();

  /* Bullet Simulation : Print Transformation Matrix for Each Object in the Dynamic World 
	for (size_t i=0;i<20;i++) {
		dynamicsWorld->stepSimulation(1.f/60.f,10);
    // std::cout << getMatrix( bulletBallBody->getCenterOfMassTransform() ) << std::cout;

		//print positions of all objects
		for (int j=dynamicsWorld->getNumCollisionObjects()-1; j>=0 ;j--) {
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
      // Get Transform Matrix
      btScalar m[16];
 			if (body && body->getMotionState()) {
				btTransform trans;
				// body->getMotionState()->getWorldTransform(trans);
        btDefaultMotionState* motion = (btDefaultMotionState*)body->getMotionState();
        motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
				// printf("world pos = %f,%f,%f\n",float(trans.getOrigin().getX()),float(trans.getOrigin().getY()),float(trans.getOrigin().getZ()));
			}
      else {
        obj->getWorldTransform().getOpenGLMatrix(m);
      }
      std::cerr << "matrix: " << std::endl;
      for (size_t i = 0; i < 4 ; i++) {
        for (size_t j = 0; j < 4 ; j++) {
          std::cout << m[i*4+j] << " "; 
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
		}
	}*/

/* int main(int argc, const char *argv[])
{
  // DoTheImportThing("obj/bunny.obj");

  glutStuff(argc, argv);

	glewInit();

  // Create Objects 
  unsigned int pFlags = 
    aiProcess_CalcTangentSpace | aiProcess_Triangulate | 
    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
  // Ball
  Obj ball("obj/bunny.obj", pFlags, PhysObj() );
  // Obj ball("obj/ball.obj", pFlags, PhysObj() );
  // Obj ball("obj/bunny.obj", pFlags, PhysObj() );
  // Obj ball("obj/airboat.obj", pFlags, PhysObj() );
  // Obj ball("obj/bunny.obj", pFlags, PhysObj() );
	pball = &ball;
	// Board
	// Obj board("obj/board.obj", pFlags, PhysObj() );
	// pboard = &board;
  // board.debug_print();

  initGPU();

  // Bullet ---------------------------------------------------------------------
  // Bullet Settings
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
  btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
  btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,world_gravity,0));




  // 
  glutMainLoop();


  return 0;
}
*/

// 10-28
// Old
/* Bullet Board 
  // { 
    // Shape
    // btBvhTriangleMeshShape* boardShape = new btBvhTriangleMeshShape( &(board.bt_triangles), true );

		// btScalar mass(board_mass);
		// btVector3 localInertia(0,0,0);

    // // transform : default position
		// btTransform boardTransform;
		// boardTransform.setIdentity();
		// boardTransform.setOrigin(btVector3(0,0,0));
		// btDefaultMotionState* bulletMotionState = new btDefaultMotionState(btTransform(boardTransform));
		// btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,bulletMotionState,boardShape,localInertia);

		// // tweak rigidbody properties
		// rbInfo.m_restitution = grestitution;
		// rbInfo.m_friction = gfriction;
		// // add to the world
		// bulletBoardBody = new btRigidBody(rbInfo);
		// if(!bulletBoardBody) std::cout << "bulletBoardBody pointer null" << std::endl; 
    // bulletBoardBody->setActivationState(DISABLE_DEACTIVATION);
    // dynamicsWorld->addRigidBody(bulletBoardBody);

  // }
*/
/* Bullet Ball 
{ 
  // Shape Creation
  btConvexHullShape* ballShape = new btConvexHullShape();
  for (size_t i=0; i<ball.data.size();i++) {
    glm::vec3& v = ball.data[i].vertex;
    ballShape->addPoint( glmvec3_to_btVector3(v) );
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
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,bulletMotionState,ballShape,localInertia);
  // tweak rigidbody info
  rbInfo.m_restitution = grestitution;
  rbInfo.m_friction = gfriction;
  // add ball to the world
  bulletBallBody = new btRigidBody(rbInfo);
  if(!bulletBallBody) std::cout << "bulletBallBody pointer null" << std::endl; 
  dynamicsWorld->addRigidBody(bulletBallBody);
} */
