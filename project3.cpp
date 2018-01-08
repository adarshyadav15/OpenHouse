#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>

typedef GLfloat point3[3];

GLdouble orbitDegrees = 0.0;
int groundPolygonCount = 1;
GLfloat sunRotation = 0.0f;
float rotationStep = 0.04f;
float complementary = 0.0f;
float sunlightStrength = 0.7f * sunRotation / 90.0f;

/* sun properties */
const GLfloat sunlight_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat sunlight_diffuse[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat sunlight_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat sunlight_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };

/* spotlight properties */
const GLfloat spotlight_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat spotlight_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat spotlight_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat spotlight_position[] = { 5.0f, 10+(float) sqrt(75.0), 0.5f, 1.0f };
const GLfloat sd[] = {0.0f, -1.0f, 0.0f, 0.0f};

/* sun-sphere properties */
const GLfloat sphere_emission[]   = { 1.0f, 1.0f, 0.0f, 1.0f };
const GLfloat sphere_ambient[]    = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat sphere_diffuse[]    = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat sphere_specular[]   = { 0.0f, 0.0f, 0.0f, 1.0f };

/* house properties */
const GLfloat house_emission[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat house_ambient[]    = { 89.0f/255.0f, 38.0f/255.0f, 9.0f/255.0f, 1.0f };
const GLfloat house_diffuse[]    = { 89.0f/255.0f, 38.0f/255.0f, 9.0f/255.0f, 1.0f };
const GLfloat house_specular[]   = { 0.0f, 0.0f, 0.0f, 1.0f };

/* door properties */
const GLfloat door_emission[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat door_ambient[]    = { 102.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 1.0f };
const GLfloat door_diffuse[]    = { 102.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 1.0f };
const GLfloat door_specular[]   = { 0.0f, 0.0f, 0.0f, 1.0f };

/* roof properties */
const GLfloat roof_emission[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat roof_ambient[]    = { 128.0f/255.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f };
const GLfloat roof_diffuse[]    = { 128.0f/255.0f, 128.0f/255.0f, 128.0f/255.0f, 1.0f };
const GLfloat roof_specular[]   = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };
const GLfloat def_shininess[] = { 20.0f };

/* ground properties */
const GLfloat ground_emission[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat ground_ambient[]    = { 124.0f/255.0f, 252.0f/255.0f, 0.0f, 1.0f };
const GLfloat ground_diffuse[]    = { 124.0f/255.0f, 252.0f/255.0f, 0.0f, 1.0f };
const GLfloat ground_specular[]   = { 0.0f, 0.0f, 0.0f, 1.0f };



point3 u1;
point3 u2;
point3 normal;
float length;

/* In this method we do some initializations once at the start of the program */
void myinit(void)
{
    /* spotlight properties initialization */
    glLightfv(GL_LIGHT1, GL_AMBIENT,  spotlight_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  spotlight_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spotlight_specular);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 30.0f);
    glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, 1.0f);

    glEnable(GL_BLEND);

    /* front square */

    glNewList(1, GL_COMPILE);
    point3 a = {0.0f, 0.0f, 0.0f};
    point3 b = {10.0f, 0.0f, 0.0f};
    point3 c = {10.0f, 10.0f, 0.0f};
    point3 d = {0.0f, 10.0f, 0.0f};

    /* u1 = c - a ; u2 = d - b */
    u1[0] = c[0]-a[0];
    u1[1] = c[1]-a[1];
    u1[2] = c[2]-a[2];
    u2[0] = d[0]-b[0];
    u2[1] = d[1]-b[1];
    u2[2] = d[2]-b[2];
    /* we calculate the cross product of v1 and v2 */
    normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
    normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
    normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

    /* normalise the normal */
    if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
        length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
        if (length > 0) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
        }
    }
    glBegin(GL_POLYGON);
    glNormal3fv(normal);
    glVertex3fv(a);
    glNormal3fv(normal);
    glVertex3fv(b);
    glNormal3fv(normal);
    glVertex3fv(c);
    glNormal3fv(normal);
    glVertex3fv(d);
    glEnd();
    glEndList();

    /* side rectangle */

    glNewList(2, GL_COMPILE);
    point3 e = {0.0f, 0.0f,0.0f};
    point3 f = {0.0f, 10.0f,0.0f};
    point3 g = {0.0f, 10.0f,-20.0f};
    point3 h = {0.0f,0.0f,-20.0f};

    /* u1 = g - e ; u2 = h - f */
    u1[0] = g[0]-e[0];
    u1[1] = g[1]-e[1];
    u1[2] = g[2]-e[2];
    u2[0] = h[0]-f[0];
    u2[1] = h[1]-f[1];
    u2[2] = h[2]-f[2];
    /* we calculate the cross product of u1 and u2 */
    normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
    normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
    normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

    /* normalise the normal */
    if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
        length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
        if (length > 0) {
        normal[0] /= length;
        normal[1] /= length;
        normal[2] /= length;
        }
    }
    glBegin(GL_POLYGON);
    glNormal3fv(normal);
    glVertex3fv(e);
    glNormal3fv(normal);
    glVertex3fv(f);
    glNormal3fv(normal);
    glVertex3fv(g);
    glNormal3fv(normal);
    glVertex3fv(h);
    glEnd();
    glEndList();

    /* front triangle */

    glNewList(3, GL_COMPILE);
    point3 i = {0.0f, 10.0f, 0.0f};
    point3 j = {10.0f, 10.0f, 0.0f};
    point3 k = {5.0f, 10.0f + (float) sqrt(75.0), 0.0f};

    /* u1 = j - i ; u2 = k - i */
    u1[0] = j[0]-i[0];
    u1[1] = j[1]-i[1];
    u1[2] = j[2]-i[2];
    u2[0] = k[0]-i[0];
    u2[1] = k[1]-i[1];
    u2[2] = k[2]-i[2];
    /* we calculate the cross product of u1 and u2 */
    normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
    normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
    normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

    /* normalise the normal */
    if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
        length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
        if (length > 0) {
        normal[0] /= length;
        normal[1] /= length;
        normal[2] /= length;
        }
    }
    glBegin(GL_TRIANGLES);
    glNormal3fv(normal);
    glVertex3fv(i);
    glNormal3fv(normal);
    glVertex3fv(j);
    glNormal3fv(normal);
    glVertex3fv(k);
    glEnd();
    glEndList();

    /* roof */

    glNewList(4, GL_COMPILE);
    point3 l = {0.0f, 10.0f, 0.0f};
    point3 m = {5.0f, 10+(float) sqrt(75.0), 0.0f};
    point3 n = {5.0f,10+(float) sqrt(75.0),-20.0f};
    point3 o = {0.0f, 10.0f, -20.0f};

    /* u1 = n - l ; u2 = o - m */
    u1[0] = n[0]-l[0];
    u1[1] = n[1]-l[1];
    u1[2] = n[2]-l[2];
    u2[0] = o[0]-m[0];
    u2[1] = o[1]-m[1];
    u2[2] = o[2]-m[2];
    /* we calculate the cross product of u1 and u2 */
    normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
    normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
    normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

    /* normalise the normal */
    if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
        length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
        if (length > 0) {
        normal[0] /= length;
        normal[1] /= length;
        normal[2] /= length;
        }
    }
    glBegin(GL_POLYGON);
    glNormal3fv(normal);
    glVertex3fv(l);
    glNormal3fv(normal);
    glVertex3fv(m);
    glNormal3fv(normal);
    glVertex3fv(n);
    glNormal3fv(normal);
    glVertex3fv(o);
    glEnd();
    glEndList();

    /* door */

    glNewList(5, GL_COMPILE);
    point3 p = {4.0f,0.0f,0.0005f};
    point3 q = {6.0f,0.0f,0.0005f};
    point3 r = {6.0f,5.0f,0.0005f};
    point3 s = {4.0f,5.0f,0.0005f};

    /* u1 = r - p ; u2 = s - q */
    u1[0] = r[0]-p[0];
    u1[1] = r[1]-p[1];
    u1[2] = r[2]-p[2];
    u2[0] = s[0]-q[0];
    u2[1] = s[1]-q[1];
    u2[2] = s[2]-q[2];
    /* we calculate the cross product of v1 and v2 */
    normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
    normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
    normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

    /* normalise the normal */
    if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
        length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
        if (length > 0) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
        }
    }
    glBegin(GL_POLYGON);
    glNormal3fv(normal);
    glVertex3fv(p);
    glNormal3fv(normal);
    glVertex3fv(q);
    glNormal3fv(normal);
    glVertex3fv(r);
    glNormal3fv(normal);
    glVertex3fv(s);
    glEnd();
    glEndList();
}

void drawTriangle(point3 v1, point3 v2, point3 v3){
    glBegin(GL_TRIANGLES);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glEnd();
}

/* recursive subdivision method */
void subdivide(point3 v1, point3 v2, point3 v3, int level){
    if (level == 0) {
        drawTriangle(v1, v2, v3);
    } else{
        //first edge
        point3 v12;
        v12[0] = 0.5f * (v1[0] + v2[0]);
        v12[1] = 0.5f * (v1[1] + v2[1]);
        v12[2] = 0.5f * (v1[2] + v2[2]);
        //normalisation to move the vertex on the sphere
        float s = 1.0f / sqrt(v12[0] * v12[0] + v12[1] * v12[1] + v12[2] * v12[2]);
        v12[0] *= s;
        v12[1] *= s;
        v12[2] *= s;

        //second edge
        point3 v13;
        v13[0] = 0.5f * (v1[0] + v3[0]);
        v13[1] = 0.5f * (v1[1] + v3[1]);
        v13[2] = 0.5f * (v1[2] + v3[2]);
        //normalisation to move the vertex on the sphere
        s = 1.0f / sqrt(v13[0] * v13[0] + v13[1] * v13[1] + v13[2] * v13[2]);
        v13[0] *= s;
        v13[1] *= s;
        v13[2] *= s;

        //third edge
        point3 v23;
        v23[0] = 0.5f * (v2[0] + v3[0]);
        v23[1] = 0.5f * (v2[1] + v3[1]);
        v23[2] = 0.5f * (v2[2] + v3[2]);
        //normalisation to move the vertex on the sphere
        s = 1.0f / sqrt(v23[0] * v23[0] + v23[1] * v23[1] + v23[2] * v23[2]);
        v23[0] *= s;
        v23[1] *= s;
        v23[2] *= s;

        subdivide(v1, v12, v13, level - 1);
        subdivide(v12, v2, v23, level - 1);
        subdivide(v13, v23, v3, level - 1);
        subdivide(v12, v23, v13, level - 1);

    }
}

/* In this method we create the sun sphere and assign properties to the sphere and also the light source */
void make_sun(){
    glLightfv(GL_LIGHT0, GL_AMBIENT,  sunlight_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  sunlight_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunlight_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, sunlight_position);

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  sphere_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   sphere_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   sphere_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  sphere_specular);

    //initial tetrahedron
    point3 v[]={{0.0, 0.0, 1.0}, {0.0, 0.942809, -0.33333},
                {-0.816497, -0.471405, -0.333333}, {0.816497, -0.471405, -0.333333}};
    int subdivision_level = 4;

    subdivide(v[0], v[1], v[2], subdivision_level);
    subdivide(v[0], v[3], v[1], subdivision_level);
    subdivide(v[0], v[2], v[3], subdivision_level);
    subdivide(v[1], v[3], v[2], subdivision_level);
}

/* In this method we create the ground plane and assign its properties */
void make_ground(){

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  ground_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   ground_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   ground_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  ground_specular);

    if (groundPolygonCount == 1) {

        point3 a = {200.0f, -0.00005f, 200.0f};
        point3 b = {200.0f, -0.00005f, -200.0f};
        point3 c = {-200.0f, -0.00005f, -200.0f};
        point3 d = {-200.f, -0.000005f, 200.0f};

        /* u1 = c - a ; u2 = d - b */
        u1[0] = c[0]-a[0];
        u1[1] = c[1]-a[1];
        u1[2] = c[2]-a[2];
        u2[0] = d[0]-b[0];
        u2[1] = d[1]-b[1];
        u2[2] = d[2]-b[2];
        /* we calculate the cross product of u1 and u2 */
        normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
        normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
        normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

        /* normalise the normal */
        if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
            length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
            if (length > 0) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
            }
        }

        glBegin(GL_POLYGON);
        glNormal3fv(normal);
        glVertex3fv(a);
        glNormal3fv(normal);
        glVertex3fv(b);
        glNormal3fv(normal);
        glVertex3fv(c);
        glNormal3fv(normal);
        glVertex3fv(d);
        glEnd();

    } else if (groundPolygonCount == 100) {


        for (float z=-200.0f; z<200.0f; z+=40.0f){
            glBegin(GL_QUAD_STRIP);
            for (float x=-1200.0f; x<200.0f; x+=40.0f){

                point3 a = {x, -0.00005f, z};
                point3 b = {x, -0.00005f, z+40.0f};
                point3 c = {x+40.0f, -0.000005f, z};
                point3 d = {x+40, -0.00005f, z+40.0f};

                /* u1 = d - a ; u2 = c - b */
                u1[0] = d[0]-a[0];
                u1[1] = d[1]-a[1];
                u1[2] = d[2]-a[2];
                u2[0] = c[0]-b[0];
                u2[1] = c[1]-b[1];
                u2[2] = c[2]-b[2];
                /* we calculate the cross product of u1 and u2 */
                normal[0] = u1[1] * u2[2] - u1[2] * u2[1];
                normal[1] = u1[2] * u2[0] - u1[0] * u2[2];
                normal[2] = u1[0] * u2[1] - u1[1] * u2[0];

                /* normalise the normal */
                if ((normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]) != 1) {
                    length = sqrt((double) (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]));
                    if (length > 0) {
                    normal[0] /= length;
                    normal[1] /= length;
                    normal[2] /= length;
                    }
                }
                if (x==-200.0f) { // this is so that we don't draw the same vertexes twice
                    glNormal3fv(normal);
                    glVertex3fv(a);
                    glNormal3fv(normal);
                    glVertex3fv(b);
                }

                glNormal3fv(normal);
                glVertex3fv(c);
                glNormal3fv(normal);
                glVertex3fv(d);
            }
            glEnd();
        }
    }
}

/* GLUT callback Handlers */

static void resize(int width, int height)
{

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-150, 150, -150, 150, -150, 150);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* here we position the observer */
    gluLookAt(0, 50, 70, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotated(orbitDegrees, 0, 1, 0);

    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS, def_shininess);

    /* here we create the sun and position it correctly on its orbit */
    glPushMatrix();
        glRotatef(-1.0f*sunRotation, 0, 0, 1);
        glTranslatef(-50.0f, 0.0f, 0.0f); //initial sun position
        glScalef(3.0f, 3.0f, 3.0f);
        make_sun();
    glPopMatrix();

    /* here we create the ground */
    make_ground();

    /* this translate positions the house in the center of the scene (0,0,0) */
    glTranslatef(-5.0f, 0.0f,10.0f);

    /* here we assign the material properties for the house */
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  house_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   house_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   house_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  house_specular);

    glCallList(1);
    glCallList(2);
    glCallList(3);

    /* here we position the spotlight */
    glLightfv(GL_LIGHT1, GL_POSITION, spotlight_position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, sd);

    glPushMatrix();
        glTranslatef(10.0f, 0.0f, -20.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glCallList(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(10.0f, 0.0f, -20.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glCallList(2);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(10.0f, 0.0f, -20.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glCallList(3);
    glPopMatrix();

    /* here we assign the material properties for the door */
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  door_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   door_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   door_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  door_specular);

    glCallList(5);

    /* here we assign the material properties for the roof */
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  roof_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   roof_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   roof_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  roof_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);

    glCallList(4);
    glPushMatrix();
        glTranslatef(5.0f, 10 + (float) sqrt(75.0), -20.0f);
        glRotatef(180, 0.0f, 1.0f, 0.0f);
        glTranslatef(-5.0f,-(10+(float) sqrt(75.0)), 0.0f);
        glCallList(4);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

/* main popup menu. All the items are submenus, so this method is empty. */
void popup_menu(int id)
{

}

void ground_submenu(int id)
{
    if(id==1) {
        groundPolygonCount = 1;
        glutPostRedisplay();
    }
    if(id==2) {
        groundPolygonCount = 100;
        glutPostRedisplay();
    }
}

void spotlight_submenu(int id)
{
    if(id==1) {
        glEnable(GL_LIGHT1);
        glutPostRedisplay();
    }
    if(id==2) {
        glDisable(GL_LIGHT1);
        glutPostRedisplay();
    }
}

void shading_submenu(int id)
{
    if(id==1) {
        glShadeModel(GL_SMOOTH);
        glutPostRedisplay();
    }
    if(id==2) {
        glShadeModel(GL_FLAT);
        glutPostRedisplay();
    }
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':
            orbitDegrees+=1;
            break;
        case 'd':
            orbitDegrees-=1;
            break;
    }

    glutPostRedisplay();
}

static void mouse_func(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        orbitDegrees+=9;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        orbitDegrees-=9;
    }
}

static void idle(void)
{
    /* calculating sun's "orbit" */
    sunRotation+=rotationStep;
    if (sunRotation > 180.0) {
        sunRotation = 0;
    }
    /* calculating the intensity of the sunlight */
    if (sunRotation <= 90.0f) {
        sunlightStrength = 0.7f * (sunRotation / 90.0f);
    } else if (sunRotation <= 180.0f) {
        complementary = sunRotation - 90.0f;
        sunlightStrength = 0.7f * ((sunRotation - complementary) / 90.0f);
    }
    int i;
    for (i=0;i<3;i++) {
        sunlight_diffuse[i] = 0.3f+sunlightStrength;
        sunlight_specular[i] = 0.3f+sunlightStrength;
    }

    glutPostRedisplay();
}

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("project the third");

    /* setting callback functions */
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse_func);
    glutIdleFunc(idle);

    glClearColor(0,0,0,1);

    /* setting the value for the ambient lighting */
    const GLfloat amb[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT ,amb);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);

    /* creating the menu */
    int groundMenu = glutCreateMenu(ground_submenu);
    glutAddMenuEntry("single polygon",1);
    glutAddMenuEntry("hundred polygons",2);

    int spotlightMenu = glutCreateMenu(spotlight_submenu);
    glutAddMenuEntry("on",1);
    glutAddMenuEntry("off",2);

    int shadingMenu = glutCreateMenu(shading_submenu);
    glutAddMenuEntry("smooth",1);
    glutAddMenuEntry("flat",2);

    glutCreateMenu(popup_menu);
    glutAddSubMenu("ground polygons count", groundMenu);
    glutAddSubMenu("spotlight", spotlightMenu);
    glutAddSubMenu("shading model", shadingMenu);
    glutAttachMenu(GLUT_MIDDLE_BUTTON );

    myinit();

    glutMainLoop();

    return EXIT_SUCCESS;
}
