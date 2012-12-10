#ifndef BOXWALL_H
#define BOXWALL_H

// the class that holds the info for a WALL
class boxWall
{

    public:   
        boxWall( float, float,float,float,float,float,float,float );
        bool checkIfInside( float x, float z );
        void changeCoords( float, float, float, float );
        int  whatSideHit( float oldx, float oldz );

    private:

        // 0 is TOP LEFT
        // 1 is TOP RIGHT
        // 2 is BOTTOM LEFT
        // 3 is BOTTOM RIGHT
        float topLeft[2];
        float topRight[2];
        float bottomLeft[2];
        float bottomRight[2];
};

#endif
