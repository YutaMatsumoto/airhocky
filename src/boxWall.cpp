#include "boxWall.h"

// the class that holds the info for a WALL

   
    boxWall::boxWall( float tl0 = 0, float tl1 = 0,float tr0=0,float tr1=0,float bl0=0,float bl1=0,float br0=0,float br1=0 ){
        // set the boundaries of the wall
        topLeft[ 0 ] = tl0;
        topLeft[ 1 ] = tl1;

        topRight[ 0 ] = tr0;
        topRight[ 1 ] = tr1;

        bottomLeft[ 0 ] = bl0;
        bottomLeft[ 1 ] = bl1;

        bottomRight[ 0 ] = br0;
        bottomRight[ 1 ] = br1;
    }

    bool boxWall::checkIfInside( float x, float z ){
    // check to see if the inputed coords are in this wall ( inside the boundaries )
        // if the new x and z are inside a wall ( return false )
        //          check x first        
        if( x - 2 <= topLeft[0] and x + 2 >= topRight[0] )
           {
            // check z's
            if( z - 2 <= topLeft[1] and z + 2 >= bottomRight[1] )
               {
                    // return false
                    return true;
               }
           }

        return false;
    }

    //                                  1           2            3              4
    void boxWall::changeCoords( float leftX, float topY, float rightX, float bottomY )
       {
        // change the correct coords
        topLeft[0] = leftX;
        topRight[0] = rightX;
        topLeft[1] = topY;
        bottomRight[1] = bottomY;
       }

    // function to return on what side the ball hit
    int boxWall::whatSideHit( float oldx, float oldz )
       {
        if( oldz - 2 < topLeft[1] and oldz + 2 > bottomRight[1] )
           {
            // the ball is on a side of the wall
            //  return this with 0 ( stop horizontal movement )
            return 0;
           }
        else
           {
            // the ball is either on the top or the bottom
            return 1;
           }
       }














