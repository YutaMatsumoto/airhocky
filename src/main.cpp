#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <GL/glx.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <chrono>
#include <math.h>
#include <vector>
#include <random>
//#include "Vertex.cpp"  

#include "loadOBJ.cpp"
#include "boxWall.cpp"
#include "master.cpp"

#include "InitShader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp" //Makes passing matrices to shaders easier

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags




using namespace std;

//--Evil Global variables
int w = 640, h = 480;// Window size

string shader_frag = "src/shader2.frag";// "src/fs.txt";
string shader_vert = "src/shader2.vert";// "src/vs.txt";

GLuint program;// The GLSL program handle
GLuint program2;// The GLSL program handle

// VBO's for all objects ( Vertex Buffer Object )
GLuint vbo_Wall;
GLuint vbo_Sphere;
GLuint vbo_Floor;
GLuint vbo_Start;
GLuint vbo_Dragon;

// UVB's for all objects ( Uniform Vertex Buffer ? )

// NBO's for all objects ( Normal Buffer Object )
GLuint nbo_Floor;

// camera position
glm::vec3 cameraPosition( 0.0, 60.0, -0.1 );
glm::vec3 cameraTarget( 0.0, 1.0, 0.0 );
glm::vec3 cameraUp( 0.0, 0.0, 1.0 );

float perspective_near = 1;
float perspective_far  = 1000.0;

// Light vars
glm::vec3 lightPos( 0.0, 40.0, 0.0 );

//uniform locations
GLuint MVP_ID;  // MVP Matrix Handle
GLuint View_ID;  // View Matrix Handle
GLuint Model_ID;  // Model Matrix Handle

//attribute locations
GLint loc_position;
GLint loc_normal;
GLint loc_color;

/////////////////////////////////////////////////////////////////////
// handles for program 2

/*
GLuint MVP_ID2;

GLint loc_position2;
GLint loc_normal2;
GLint loc_color2;
GLint loc_tex;

GLuint loc_LightPosition; 
GLuint loc_AmbientProduct;   
GLuint loc_DiffuseProduct; 
GLuint loc_SpecularProduct;
GLuint loc_Shineness;    
GLuint loc_Sampler; 
GLuint loc_SpotDirection; 
GLuint loc_LightCutOff; 
*/
GLuint h_gWVP; // previously MVP_ID2
GLint h_gWorld;
GLint 
  h_Position,
  h_TexCoord,
  h_Normal,
  h_LightPosition,
  h_AmbientProduct,
  h_DiffuseProduct,
  h_SpecularProduct,
  h_Shineness,
  h_gSampler,
  h_Color
;

//////////////////////////////////////////////////////////////////

// Rotation variables
float ROTATIONX = 0.0, TEMPX = 0.0;
float ROTATIONZ = 0.0, TEMPZ = 0.0;
float X_COUNT = 0.0;
float Z_COUNT = 0.0;
float MAX_ROT = 20.0;
float MAX_BALL_X = 9.5;
float MAX_BALL_Z = 19.5; // 17.5
float xMove, yMove, zMove;
float oldXAngle = 0.0, oldYAngle = 0.0;
bool Paused = false, RESET = false;
bool showPos = false, dead = false;
float MIN_DISTANCE = 2.5;
float puckSpeed = 20.0;
float puckVel[2] = { 1.0, puckSpeed };
float ballTimer;
bool enemyMoveLeft = true;
float enemySpeed = 20;
int uPoints =0, ePoints =0;

float oldMouse[2] = { 0, 0 };
bool MOUSE_MOVING = false;
bool dragonToggle = false;
bool won = false;
bool softReset = false;

// setup the array of walls
vector< boxWall > listOfWalls;
vector< Master > objs;

//transform matrices
glm::mat4 board;//obj->world each object should have its own board matrix
glm::mat4 ball;//obj->world each object should have its own ball matrix
glm::mat4 floorMat;//obj->world each object should have its own floor matrix
glm::mat4 Start;//obj->world each object should have its own Start matrix
glm::mat4 Dragon;//obj->world each object should have its own Dragon matrix

glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvpWall;//premultiplied boardviewprojection
glm::mat4 mvpBall;//premultiplied ballviewprojection
glm::mat4 mvpFloor;//premultiplied floorviewprojection
glm::mat4 mvpStart;//premultiplied startviewprojection
glm::mat4 mvpDragon;//premultiplied finishviewprojection

glm::mat4 gProj, gWVP, gWorld;

// Lighting
float lightoff = 20;
float lightoffset_x = 0;
float lightoffset_y = 10;
float lightoffset_z = 1;


//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void keyup(unsigned char key, int x_pos, int y_pos);
void keySpecialDown( int, int, int );
void mouseMove( int x, int y );
void mouseWheel(int,int,int,int);

bool bind_shader_variables2(GLuint h_program);
void init_shader_variables();
GLint GetUniformLocation(GLint h_prog, const char* pUniformName);
GLint GetAttribLocation(GLint h_prog, const char* variable_name);

//--Resource management
bool initialize();
void cleanUp();

//--Random time things
float getDT();
  chrono::time_point<  chrono::high_resolution_clock> t1,t2;

// read file function
char *textFileRead( const char *);

bool DoTheImportThing( const std::string& pFile);

// load the obj file function
Vertex* loadOBJ( char* , float , float , float );

// menu prototype
void menu( int id ); 

// distance function
float dist( float, float, float, float );

// load the dragon file
bool loadDragonFile( const string& fileName );

// analyze dragon scene taken from  assimp readFile (loadDragonFile function )
void  processDragon( const aiScene* );

// function to setup the list of walls
void setupWalls();

// check the ball against where the walls are
bool checkForWalls( float, float, float &, float & );

// load the shaders
void createShaders( const char* , const char*, GLuint &, GLuint & );

// initialize the shader variables
//void initShaderVariables();

// check for paddle collision
int checkForCollisions( float xPos, float zPos, float &pX, float &pZ, float eX, float eZ );

// draw text to the screen
void renderBitmapCharacter(float x, float y, float z, void *font, string str);

// get the approprate printable string
string getPrintableString( int num );


// Printing
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


//--Implementations
void render()
{
    //--Render the scene

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);


    //clear the screen
    glClearColor(0.6, 0.1, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    mvpWall = projection * view * board;
    mvpBall = projection * view * ball;
    mvpFloor = projection * view * floorMat;
    mvpStart = projection * view * Start;
    mvpDragon = projection * view * Dragon;

    //enable the shader program
    glUseProgram(program2);

    glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );

    // update the shader variables
    //initShaderVariables();

    // do the rendering for all objects
    for( unsigned int w = 0; w < objs.size(); w++ )
       {
        // determine which MVP matrix to use
        switch( w ){
            case 0:
                glUniformMatrix4fv(h_gWVP, 1, GL_FALSE, glm::value_ptr(mvpBall));
                break;
            case 1:
                glUniformMatrix4fv(h_gWVP, 1, GL_FALSE, glm::value_ptr(mvpWall));
                break;
            case 2:
                glUniformMatrix4fv(h_gWVP, 1, GL_FALSE, glm::value_ptr(mvpFloor));
                break;
            case 3:
                glUniformMatrix4fv(h_gWVP, 1, GL_FALSE, glm::value_ptr(mvpStart));
                break;
            case 4:
                glUniformMatrix4fv(h_gWVP, 1, GL_FALSE, glm::value_ptr(mvpDragon));
                break;         
            }

        // enable attributes
        glEnableVertexAttribArray(h_Position);
        glEnableVertexAttribArray(h_Normal);
        glEnableVertexAttribArray(h_Color);
        glEnableVertexAttribArray(h_TexCoord); 

        //reset the texture
        objs[w].resetTexture();

        // bind the buffer
        glBindBuffer(GL_ARRAY_BUFFER, objs[w].vbo ); 

        glVertexAttribPointer(h_Position, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
        glVertexAttribPointer(h_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,texture));
        glVertexAttribPointer(h_Normal,   3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,normal));
        glVertexAttribPointer(h_Color,   3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point,color));


        if( w!= 5 )
           {
                glDrawArrays(GL_TRIANGLES, 0, 10000);//mode, starting index, count
           }
        else
           {    
                if ( dragonToggle == true )
                    glDrawArrays(GL_TRIANGLES, 0, 1000000);//mode, starting index, count
           }

        if( w == 1 )
           {
            // print the player's score
            string temp = getPrintableString( uPoints );
            renderBitmapCharacter( 10, 10, 10, GLUT_BITMAP_TIMES_ROMAN_24, temp );
           }

        glDisableVertexAttribArray(h_Position);
        glDisableVertexAttribArray(h_Normal);
        glDisableVertexAttribArray(h_Color);
        glDisableVertexAttribArray(h_TexCoord);
       }

//////////////////////////////////////////////////////////////////////////


    // draw the scores to the screen
//	glPushMatrix();
//    glRasterPos2f( 10.0, 10 );
//    glRasterPos3f( 10.0, 11.8, 10.0 );
    string temp = getPrintableString( ePoints );
    renderBitmapCharacter( 10, 10, 10, GLUT_BITMAP_TIMES_ROMAN_24, temp );
//	glPopMatrix();
                           
    //swap the buffers
    glutSwapBuffers(); 
}

void update()
{

    //variables
    static float xPos = 20.5;
    static float zPos = -10.0;
    static float yPos = 0;
    static float pX = 0.0;
    static float pZ = -10.0;
    static float eX = 0.0;
    static float eZ = 15.0;
    float dt = getDT();// if you have anything moving, use dt.
    int status = 0;

    // if reset in the menu is selected
    if( RESET )
       {
        cout << endl << "============" << endl;
        cout << "GAME RESET" << endl;
        cout << "============" << endl << endl;

        // reset all the positions of the models
        board = glm::translate(     glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0) );
        ball = glm::translate(      glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0) );
        floorMat = glm::translate(  glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0) );
    
        // reset all the angles and values
        xPos = 20.5;
        zPos = -20.0;
        eX = 0.0;
        eZ = 15.0;
        pX = 1.8;
        pZ = 0;

        ROTATIONZ = 0;
        ROTATIONX = 0;
        X_COUNT   = 0;
        Z_COUNT   = 0;

        puckVel[0] =  ( rand() %10 ) + 1;
        puckVel[1] = puckSpeed;

        uPoints = 0;
        ePoints = 0;

        RESET = false;
        Paused = false;
        softReset = false;
        dead = false;
        won = false;
       }

    if( softReset )
       {
        // reset all the angles and values
        pX = 1.8;
        pZ = 0.0;
        
        if( uPoints > ePoints )
           {
            puckVel[0] =  ( rand() %10 ) + 1;
            puckVel[1] = -puckSpeed;
           }
        else
           {
            puckVel[0] =  ( rand() %10 ) + 1;
            puckVel[1] = puckSpeed;
           }

        ballTimer -= 1;

        if( ballTimer <= 0 )
            softReset = false;
       }



    if( !Paused and !dead and !won and !softReset )
       {
        //
        xMove =  float(X_COUNT/3);
        zMove =  float(Z_COUNT/3);
        
        X_COUNT = X_COUNT / 2;
        Z_COUNT = Z_COUNT / 2;
   

        xMove = -dt*xMove*8;
        zMove = dt*zMove*8;
        
        // play with future positions
        float futureX = pX + puckVel[0]*dt;
        float futureZ = pZ + puckVel[1]*dt;
        
        // if the ball would move into a wall, dont let it
        // returns:
        // 1 for player collision
        // 2 for enemy collision
        // 3 for wall collision  ( vel is changed on this one )
        // 4 for you scoring
        // 5 for the enemy scoring
        int index = checkForCollisions( xPos, zPos, futureX, futureZ, eX, eZ );
        bool changeVel = false;
        float tempX, tempZ;
                
        switch( index ){
        
            case 1:                
                tempX = xPos;
                tempZ = zPos;  
                changeVel = true;
                break;
            case 2:
                tempX = eX;
                tempZ = eZ;
                //puckVel[ 0 ] *= 2;
                //puckVel[ 1 ] *= 2;
                changeVel = true;
                break;
            case 3:
                pX = futureX;
                pZ = futureZ;
                ballTimer = -1;
                break;
            case 4:
                // you scored
                uPoints++;
                softReset = true;
                ballTimer = 100;
                cout << "You Scored!" << endl;
                return;
                break;
            case 5:
                // you scored
                ePoints++;
                softReset = true;
                ballTimer = 100;
                cout << "The AI Scored!" << endl;
                return;
                break;
                
        }
        
        
        // move the puck in the opposite direction
        if( changeVel and !(ballTimer > 0) )
           {
            float angle = atan2( ( tempZ - pZ ) , ( tempX - pX ) ) * 180 / 3.14;
            puckVel[0] = puckSpeed * cos(angle);
            puckVel[1] = puckSpeed * sin(angle);
            ballTimer = 45;
           }

        if( ballTimer > 0 )
           {
            ballTimer -= 1;
           }
                
                
        // update positions
        pX = pX + puckVel[0]*dt;
        pZ = pZ + puckVel[1]*dt;
        
        xPos += xMove;
        zPos += zMove;


        if( xPos > MAX_BALL_X + 2.0 )
           {
            xPos = MAX_BALL_X + 2.0;
           }
        else if( xPos < -MAX_BALL_X + 1.9 )
           {
            xPos = -MAX_BALL_X + 1.9;
           }
        if( zPos > -1 ) // MAX_BALL_Z / 4
           {
            zPos = -1;
           }
        else if( zPos < -MAX_BALL_Z )
           {
            zPos = -MAX_BALL_Z;
           }


        // move the enemy
        if( enemyMoveLeft )
           {
            eX += enemySpeed;
           }
        else
            eX -= enemySpeed;

        // if he has hit the left wall, move him right/versa visa
        if( eX > MAX_BALL_X + 2.0 )
           {
            eX = MAX_BALL_X + 1.8;
            enemyMoveLeft = false;
            enemySpeed = 2*19891*getDT()*(1 + (rand() % 4) );
            while( enemySpeed == 0 )
                enemySpeed = 2*19891*getDT()*(1 + (rand() % 4) );
           }
        else if( eX < -MAX_BALL_X + 1.9 )
           {
            eX = -MAX_BALL_X + 2.1;
            enemyMoveLeft = true;
            enemySpeed = 2*19891*getDT()*(1 + (rand() % 4) );
            while( enemySpeed == 0 )
                enemySpeed = 2*19891*getDT()*(1 + (rand() % 4) );
           }

        ball = glm::translate( glm::mat4(1.0f), glm::vec3(pX, 0, pZ) );
        board = glm::translate( glm::mat4(1.0f), glm::vec3(xPos, 0, zPos) );

        floorMat = glm::translate( glm::mat4(1.0f), glm::vec3(0, 0, 0) );
        Start = glm::translate( glm::mat4(1.0f), glm::vec3(eX, 0, eZ) );
        Dragon = glm::translate( glm::mat4(1.0f), glm::vec3(0, 5, 0) );
        
  
        if ( status <= -1 )
           {
            cout << "YOU ARE DEAD..." << endl;
            dead = true;
            yPos = 0.0;
            //ball = glm::translate( board, glm::vec3(xPos, 120, zPos) );
           }

        if ( MOUSE_MOVING == true )
           {
            glutWarpPointer( w/2, h/2);
            MOUSE_MOVING = false;
           }
        
       }

    if( ePoints == 7 and !dead )
       {
        dead = true;
        cout << "The AI has won. Better luck next time." << endl << endl;
       }

    if( dead )
       {
        yPos += -dt*32;

       }

    if( uPoints == 7 and !won )
       {
        won = true;
        cout << "You have won. Congratulations!" << endl << endl;
       }

    if( won )
       {
        yPos += dt*16;


       }

    if( showPos )
       {
        cout << "Pos-- X: " << xPos << "  Z: " << zPos << endl;
        cout << "Vel-- X: " << xMove << "  Z: " << zMove << endl;
        showPos = false;
       }

    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}


void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;
    //Change the viewport to be correct
    glViewport( 0, 0, w, h);
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos)
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }
    
    if( key == 32 ) // for showing current Position
    {
     showPos = true;
    }

    // rotate the global variable depending on arrow key pressed
    switch( key ) 
       {
        case 100: 
        case  68:
            // D - RIGHT
            lightoffset_x-=lightoff;
            break;
        case 97: 
        case 65:
            // A - LEFT
            lightoffset_x+=lightoff;
            break;
        case 119:
        case  87: 
            // W - UP
            lightoffset_z += lightoff;
            break;
        case 115:
        case  83:
            // S - DOWN
            lightoffset_z -= lightoff;
            break;

        case 'q':
        case 'Q':
            // Q - Vertically Up
            lightoffset_y += lightoff;
            break;

        case 'E':
        case 'e':
            // E - Vertically Down
            lightoffset_y -= lightoff;
            break;
       }

    if( lightoffset_y < 10 )
      {
       lightoffset_y = 10;
      }

    glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
    MOUSE_MOVING = false;
}

void keyup(unsigned char key, int x_pos, int y_pos)
{
    // turn off the rotation if the key is let go
    switch( key ) 
       {
        case 100: 
        case  68: 
            // Rotate           RIGHT
            ROTATIONX = 0.0;
            break;
        case 97: 
        case 65: 
            // Rotate           LEFT
            ROTATIONX = 0.0;
            break;
        case 119:
        case  87:  
            // Rotate           UP
            ROTATIONZ = 0.0;
            break;
        case 115:
        case  83:
            // Rotate           DOWN 
            ROTATIONZ = 0.0;
            break;

        // toggle the dragon
        case   9:
            dragonToggle = !dragonToggle;
            //cout << "changed" << endl;
            break;
       }
    MOUSE_MOVING = false;
}

void keySpecialDown(int key, int x, int y )
{
  /* Rotate Camera */
  switch (key) {
    case GLUT_KEY_LEFT: { // translation : left 

      glm::vec3 temp = glm::cross(cameraTarget, cameraUp );
      glm::vec3 leftVec = glm::normalize(temp);
      cameraPosition += leftVec;
      break;
    }
    case GLUT_KEY_RIGHT : { // translation : right

      glm::vec3 temp = glm::cross( cameraUp, cameraTarget );
      glm::vec3 rightVec = glm::normalize(temp);
      cameraPosition += rightVec;
      break;
    }
    case GLUT_KEY_DOWN : { // translation : up 
      cameraPosition.z += -1.0;
      break;
    }
    case GLUT_KEY_UP : { // translation : down
      cameraPosition.z += 1.0;
      break;
    } 
    default:  {
      break;
    }
  }

  // bound camera movement
  if( cameraPosition.x > 20.0 ) {
    cameraPosition.x = 20.0;
  }
  else if( cameraPosition.x < -20.0 ) {
    cameraPosition.x = -20.0;
  }
  if( cameraPosition.z > 20.0 ) {
    cameraPosition.z = 20.0;
  }
  else if( cameraPosition.z < -20.0 ) {
    cameraPosition.z = -20.0;
  }
 
  // std::cout << "rotation" << rotationHorizontal << std::endl; 
  view = glm::lookAt( 
    cameraPosition, 
    cameraTarget,     // Focus point / Center
    cameraUp     // Up
   ); 
   
  MOUSE_MOVING = false; 
}

void mouseWheel(int button, int state, int x, int y)
{
   // Wheel reports as button 3(scroll up) and button 4(scroll down)
   if (button == 4) {
    cameraPosition += cameraTarget;
   }
   else if(button == 3){  
    cameraPosition -= cameraTarget;
   }
   
   if( cameraPosition.y > 80.0 ) {
    cameraPosition.y = 80.0;
   }
   else if( cameraPosition.y < 40.0 ) {
    cameraPosition.y = 40.0;
   }
   
   view = glm::lookAt( 
    cameraPosition, 
    cameraTarget,     // Focus point / Center
    cameraUp    // Up
   ); 
       
  MOUSE_MOVING = false;
}

bool initialize()
{

    // pre-initialize the shader variables
    //initShaderVariables(); 

    // MAYBE ADD VERTEXARRAY........                               #####################
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

///// LOAD OBJ FILES ///////////////////////////////////////////////////////////////////

    // load in .obj files
    // do this for all objects

    // load the sphere
    const char* ballFile = "obj/Puck.obj";
    const char* ballName = "ball"; 
    const char* ballTex  = "textures/orangeTex.png";
    Master ball( ballFile, ballName, ballTex ); 

    // load the player's paddle
    const char* wallFile = "obj/Paddle.obj"; //"../Meshes/Maze-NoFloor.obj"
    const char* wallName = "paddle"; 
    const char* wallTex  = "textures/blueTex.png";
    Master paddle( wallFile, wallName, wallTex ); 

    // load the floor
    const char* floorFile = "obj/tableShort2.obj"; //"../Meshes/Maze-Floor2.obj"
    const char* floorName = "floor"; 
    const char* floorTex  = "textures/hack2.PNG";
    Master floor( floorFile, floorName, floorTex );

    // load the other paddle
    const char* startFile = "obj/Paddle.obj";
    const char* startName = "paddle2"; 
    const char* startTex  = "textures/redTex.png";
    Master paddle2( startFile, startName,startTex ); 

    // push all the objects onto the object vector
    objs.push_back( ball );
    objs.push_back( paddle );
    objs.push_back( floor );
    objs.push_back( paddle2 );

    // Compile Shaders   
    program2 = InitShader( shader_vert.c_str(), shader_frag.c_str() );

    // Shader Variables
    bind_shader_variables2(program2);
    init_shader_variables();

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // remove cursor and snap it to the screen
    glutSetCursor( GLUT_CURSOR_NONE );
    glutWarpPointer( (w/2), (h/2) );

    // setup the list of walls
    setupWalls();

    // output a simple buffer zone from debug text to actual game output
    cout << endl << "======================================================" << endl;

    //and its done
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_Wall);
    glDeleteBuffers(1, &vbo_Sphere);
    glDeleteBuffers(1, &vbo_Floor);
}

//returns the time delta
float getDT()
{
    float ret;
    t2  =   chrono::high_resolution_clock::now();
    ret =   chrono::duration_cast<   chrono::duration<float> >(t2-t1).count();
    t1  =   chrono::high_resolution_clock::now();
    return ret;
}


void mouseMove( int x, int y )
{

    if( oldMouse[0] - x > 0 )
       {
        X_COUNT = -10;
        MOUSE_MOVING = true;
       }
    else if( oldMouse[0] - x < 0 )
       {
        X_COUNT = 10;
        MOUSE_MOVING = true;
       }
//    else
//        ROTATIONX = 0.0;

    if( oldMouse[1] - y > 0 )
       {
        Z_COUNT = 10;
        MOUSE_MOVING = true;
       }
    else if( oldMouse[1] - y < 0 )
       {
        Z_COUNT = -10;
        MOUSE_MOVING = true;
       }
//    else
//        ROTATIONZ = 0.0;


    
    oldMouse[0] = w/2;
    oldMouse[1] = h/2;
}

// read the shader files
char *textFileRead( const char *fn ) 
{
 
    FILE *fp;
    char *content = NULL;
 
    int count=0;
 
    if (fn != NULL) {
        fp = fopen(fn,"rt");
 
        if (fp != NULL) {
 
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);
 
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

void menu( int id )
{
    switch( id )
       {
        case 1:
            RESET = true;
            break;

        case 2:
            Paused = !Paused;
            break;

        case 3:
            exit(0);
            break;
       }
}


float dist( float x1, float y1, float x2, float y2 )
{
    // do distance squared calculations
    float temp1 = ( x1 - x2 );
    float temp2 = ( y1 - y2 );


    return ( ( temp1 * temp1 ) + ( temp2 * temp2 ) );
}


bool loadDragonFile( const string& fileName )
{
      // Create an instance of the Importer class
      Assimp::Importer importer;

      // And have it read the given file with some example postprocessing
      // Usually - if speed is not the most important aspect for you - you'll 
      // propably to request more postprocessing than we do in this example.
      const aiScene* scene = importer.ReadFile( fileName, 
            aiProcess_CalcTangentSpace       | 
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);
      
      // If the import failed, report it
      if( !scene)
      {
        cout << endl << endl << importer.GetErrorString() << endl << endl;
        return false;
      }

      // Now we can access the file's contents. 
      processDragon( scene );

      // We're done. Everything will be cleaned up by the importer destructor
      return true;

}

void  processDragon( const aiScene* scene )
{
    //cout << "here" << endl;
    //cout << "NUM meshes: " << (*scene).mNumMeshes << endl;
    cout << "# of Faces              : " << (*scene).mMeshes[0][0].mNumFaces << endl;
    cout << "# of Vertices/coords    : " << (*scene).mMeshes[0][0].mNumVertices << endl;
    cout << "First Vertice           : " << (*scene).mMeshes[0][0].mVertices[0].x << " " << (*scene).mMeshes[0][0].mVertices[0].y << " " << (*scene).mMeshes[0][0].mVertices[0].z << endl;
}

void setupWalls(){
    // for first wall: 

    // create a temp wall
    boxWall temp( 17.8,-14.1,  15.1, -13.9  , 17.8, -22.5 ,  15.3, -22.5  );
    // add it to the vector of walls
    listOfWalls.push_back( temp );


    // wall1
    temp.changeCoords( 15.4, -13.9, 8.4, -16.35 );
    listOfWalls.push_back( temp ); 
 
    // wall2 
    temp.changeCoords( 16.7, -6.0, 8.5, -8.2 );
    listOfWalls.push_back( temp );

    // wall3
    temp.changeCoords( 22.5, 1.1, 8.25, -1.45 );
    listOfWalls.push_back( temp );

    // wall4
    temp.changeCoords( 17.7, 17.5, 15.5, 6.8 ); 
    listOfWalls.push_back( temp );

    // wall5
    temp.changeCoords( 15.6, 9.5, 7.1, 6.8 );
    listOfWalls.push_back( temp );

    // wall6
    temp.changeCoords( 9.5, 17.7, 6.8, 9.2 );
    listOfWalls.push_back( temp );

    // wall7
    temp.changeCoords( 1.28, -8.2, -1.1, -22.5 );
    listOfWalls.push_back( temp );

    // wall8
    temp.changeCoords( 1.38, -5.7, -9.68, -8.1  );
    listOfWalls.push_back( temp );

    // wall9
    temp.changeCoords( -6.6, -8.2, -9.35, -13.9 );
    listOfWalls.push_back( temp );

    // wall10
    temp.changeCoords( -6.6, -14.1, -15.3, -16.6  );
    listOfWalls.push_back( temp );

    // wall11
    temp.changeCoords( -14.4, 1.0, -17.6, -16.6  );
    listOfWalls.push_back( temp );

    // wall12
    temp.changeCoords( -9.5, 1.0, -14.7, -1.2  );
    listOfWalls.push_back( temp );

    // wall13
    temp.changeCoords( -7.0, 18.3, -9.7, -1.2 );
    listOfWalls.push_back( temp );

    // wall14
    temp.changeCoords( -1.2, 9.9, -7.0, 6.8  );
    listOfWalls.push_back( temp );

    // wall15
    temp.changeCoords( -15.2, 17.8, -17.5, 6.8  );
    listOfWalls.push_back( temp );

    // wall16
    temp.changeCoords( -17.2, 9.2, -22.5, 6.8 );
    listOfWalls.push_back( temp );  
}

bool checkForWalls( float oldx, float oldz, float &xMove, float &zMove ){
 
    // determine projected location
    float x, z;
    x = oldx + xMove;  
    z = oldz + zMove;

    // for every wall
    for( unsigned int i = 0; i < listOfWalls.size(); i++ )
       {
        // check to see if the x and z are inside the walls
        if( listOfWalls[i].checkIfInside( x, z ) )
           {

            // check again for another collision
            for( unsigned int p = i+1; p< listOfWalls.size(); p++ )
               {
                // if u are in another wall, stop all movement
                if( listOfWalls[p].checkIfInside( x, z ) )   
                   {             
                    xMove = 0.0;
                    zMove = 0.0;
                    return true;
                   }
               }

            // if no other collision is found, then you continue as normal
                // stop one movement

            // if true stop the correct movement (create sliding)
            int side = listOfWalls[i].whatSideHit( oldx, oldz );

            if( side == 0 )
               {
               xMove = 0.0;
               }
            else
               {
                zMove = 0.0;
               }            

            return true;
            

           }
       }    

    return false;
}

void createShaders( const char* vName, const char* fName, GLuint &vShader, GLuint &fShader )
   {

    //compile the shaders
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream(vName, std::ios::in);
        if(VertexShaderStream.is_open()){
                std::string Line = "";
                while(getline(VertexShaderStream, Line))
                        VertexShaderCode += "\n" + Line;
                VertexShaderStream.close();
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fName, std::ios::in);
        if(FragmentShaderStream.is_open()){
                std::string Line = "";
                while(getline(FragmentShaderStream, Line))
                        FragmentShaderCode += "\n" + Line;
                FragmentShaderStream.close();
        }

        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vName);
        char const * VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(vShader, 1, &VertexSourcePointer , NULL);
        glCompileShader(vShader);

    // Check Vertex Shader
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(vShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);



    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fName);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(fShader, 1, &FragmentSourcePointer , NULL);
    glCompileShader(fShader);


    // Check Fragment Shader
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(fShader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

   }

/*
void initShaderVariables()
   {
    // add light info ( -zlight ) 
    glUniform4f( loc_LightPosition  , lightPos.x, lightPos.y, lightPos.z, 1 );
    glUniform4f( loc_AmbientProduct , 0.5, 0.5, 0.5, 1);
    glUniform4f( loc_DiffuseProduct , 0.3, 0.3, 0.3, 1);
    glUniform4f( loc_SpecularProduct, 0.3, 0.3, 0.3, 1);
    glUniform1f( loc_Shineness      , 0.5 );
    glUniform1i( loc_Sampler        , 0); 
    glUniform3f( loc_SpotDirection  , 0.0, -1.0, 0.0 );
    glUniform1f( loc_LightCutOff    , 0.001 );  // in degrees

   }
*/

void init_shader_variables()
{

  glm::mat4 id = glm::mat4(1);

  view = glm::lookAt( 
    cameraPosition,
    cameraTarget,     // Focus point / Center
    cameraUp     // Up
  ); 

  gProj = glm::perspective(90.0f, float(w)/float(h), perspective_near, perspective_far );
      // 1.0f, 100.0f);
  gWVP    = gProj * view * id;
  gWorld  =         view * id;

  glUniformMatrix4fv(h_gWVP,    1, GL_FALSE, glm::value_ptr(gWVP)   );
  glUniformMatrix4fv(h_gWorld,  1, GL_FALSE, glm::value_ptr(gWorld) );

  glUniform4f( h_LightPosition  , lightoffset_x, lightoffset_y, lightoffset_z, 1 );
  glUniform4f( h_AmbientProduct , 0.3, 0.3, 0.3, 1);
  glUniform4f( h_DiffuseProduct , 0.8, 0.8, 0.8, 1);
  glUniform4f( h_SpecularProduct, 0.8, 0.8, 0.8, 1);
  glUniform1f( h_Shineness      , 0.5 );

  glUniform1i( h_gSampler, 0);

}

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


GLint GetUniformLocation(GLint h_prog, const char* pUniformName)
{
    GLint Location = glGetUniformLocation(h_prog, pUniformName);
    signed int test = 0xFFFFFFFF;

    if (Location == test)
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint GetAttribLocation(GLint h_prog, const char* variable_name)
{
    GLint Location = glGetAttribLocation(h_prog, variable_name);
    signed int test = 0xFFFFFFFF;

    if (Location == test)
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", variable_name);
    }

    return Location;
}

int checkForCollisions( float xPos, float zPos, float &pX, float &pZ, float eX, float eZ )
{
    // check for collision with the puck and the player
    //    (if the puck is on that half of the board)
    if( dist( xPos, zPos, pX, pZ ) < 9.5 )
       {
        // return 1 ( if collided with player )
        return 1;
       }

    // check for collision with the puck and the enemy
    //    (if the puck is on that half of the board)
    if( dist( eX, eZ, pX, pZ ) < 9.0 )
       {
        // return 2 ( if collided with enemy )
        return 2;
       }

    // check for collision with goals
    if( pZ >= MAX_BALL_Z + 1.5 )
       {
        if( pX <= 5.4 and pX >= -1.0 )
           {
            // you scored
            return 4;
           }
       }
    else if( pZ <= -MAX_BALL_Z )
       {
        if( pX <= 5.4 and pX >= -1.0 )
           {
            // the enemy scored
            return 5;
           }
       }

    // check wall collisions
            // return 3 ( if collidid with walls )
    if( pX <= -MAX_BALL_X + 1 )
       {
        puckVel[ 0 ] *= -1;
        pX = -MAX_BALL_X + 1.2;
        return 3;
       }
    else if( pX >= MAX_BALL_X + 3.5 )
       {
        puckVel[ 0 ] *= -1; 
        pX = MAX_BALL_X + 2.3;  
        return 3;     
       }
    if( pZ <= -MAX_BALL_Z - 1 )
       {
        puckVel[ 1 ] *= -1; 
        pZ = -MAX_BALL_Z - 0.8;  
        return 3;     
       }
    else if( pZ >= MAX_BALL_Z + 2 )
       {
        puckVel[ 1 ] *= -1;        
        pZ = MAX_BALL_Z + 1.8;
        return 3;
       }




    // return nothing (0 for no collisions)
    return 0;
}

void renderBitmapCharacter(float x, float y, float z, void *font, string str)
{
  
  glColor3f( 5.0f, 1.0f, 0.5f );
  glRasterPos3f(x, y,z);

  for( unsigned int i = 0; i < str.length(); i++ )
   {
    glutBitmapCharacter(font, str[i]);
   }
}

string getPrintableString( int num )
{
    switch( num ){
        case 0:
            return "    0";
            break;
        case 1:
            return "    1";
            break;
        case 2:
            return "    2";
            break;
        case 3:
            return "    3";
            break;
        case 4:
            return "    4";
            break;
        case 5:
            return "    5";
            break;
        case 6:
            return "    6";
            break;
        case 7:
            return "    7";
            break;
        default:
            return " ";
            break;

    }
}

// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    // Name and create the Window
    glutCreateWindow("'Better Than Your Game' Game");

    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
          cerr << "[F] GLEW NOT INITIALIZED: ";
          cerr << glewGetErrorString(status) <<   endl;
        return -1;
    }

    // Set all of the callbacks to GLUT that we need
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input
    glutKeyboardUpFunc(keyup);// Called if there is keyboard input
    glutSpecialFunc(keySpecialDown); // called for special characters
    glutPassiveMotionFunc(mouseMove);// Called if the mouse is moved
    glutMouseFunc(mouseWheel);

    // setup the menu
    glutCreateMenu( menu );
    glutAddMenuEntry( "Start", 1);
    glutAddMenuEntry( "Pause/Resume", 2);
    glutAddMenuEntry( "Quit", 3);
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if(init)
    {
        t1 =   chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}
