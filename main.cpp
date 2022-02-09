#include "main.h"

// import the header from main.h document

int POS_X, POS_Y;

string model_name1 = "data/model_no_texture/miniEarth.obj";
string model_name2 = "data/model/miniEarth.obj";
// loading the models and their material (texture)

float pos_x, pos_y, pos_z;
float angle_x = 30.0f, angle_y = 0.0f;
// window's initial position and the model's ratoate angles

int x_old = 0, y_old = 0;
int current_scroll = 5;
float zoom_per_scroll;
// some variable quantities

bool is_holding_mouse = false;
bool is_updated = false;
bool is_shift_with_or_without_texture = true;
// boolean variables to decide and change the display methods

Model model1;
Model model2;
// declare two model with or with out the texture

GLfloat light_pos[] = { -10.0f, 10.0f, 100.00f, 1.0f };
GLfloat angle = 110.0;
GLfloat lightambient[] = { 0.7,0.7,0.7,1.0 };
GLfloat light_direction[] = { -1.0, -1.0, 0.0 };
GLfloat light_position1[] = { 100.0, 100.0, -30.0, 1.0 };
GLfloat light_diffuse1[] = { 0.9, 0.9, 0.9, 0.0 };
// set two lights' attributes

void init() {
    /*
    * initial the basic attributes of different methods.
    * such as light, color, viewpoint, coordinate and so on.
    */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightambient);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction);

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, 1.0, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    model1.load(model_name1.c_str());
    model2.load(model_name2.c_str());

    pos_x = model1.pos_x+0.4f;
    pos_y = model1.pos_y;
    pos_z = model1.pos_z-1.0f;

    zoom_per_scroll = -model1.pos_z / 10.0f;
}

void display() {
    /*
    * display function: initial the OpenGL's matrix and
    * call the draw function to display the model
    */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(pos_x, pos_y, pos_z);
    glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
    glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
    (is_shift_with_or_without_texture)?model1.draw():model2.draw();
    glutSwapBuffers();
}

void timer(int value) {
    /*
    * wait for any time if the user change the state of the mouse
    */
    if (is_updated) {
        is_updated = false;
        glutPostRedisplay();
    }
    glutTimerFunc(INTERVAL, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    /*
    * keyboard's interactions, press 'Esc' to quit
    * or, press 's' / 'S' to change the display's modle with or without texture
    */
    if (key == 27) exit(0);
    else if (key == 'S' || key == 's') is_shift_with_or_without_texture ^= 1;
    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
    /*
    * mouse's interactions:
    * x-coordinate to control the x-plane rotate
    * y-coordinate to control the y-plane rotate
    */
    is_updated = true;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            x_old = x;
            y_old = y;
            is_holding_mouse = true;
        } else
            is_holding_mouse = false;
    } else if (state == GLUT_UP) {
        switch (button) {
            case 3:
                if (current_scroll > 0) {
                    current_scroll--;
                    pos_z += zoom_per_scroll;
                }
            break;
            case 4:
                if (current_scroll < MAX_SCROLL_VALUE) {
                    current_scroll++;
                    pos_z -= zoom_per_scroll;
                }
            break;
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    /*
    * mouse's motion
    * get the current movement state of the mouse
    */
    if (is_holding_mouse) {
        is_updated = true;

        angle_y += (x - x_old);
        x_old = x;
        if (angle_y > 360.0f)   angle_y -= 360.0f;  else 
        if (angle_y < 0.0f)     angle_y += 360.0f;

        angle_x += (y - y_old);
        y_old = y;
        if (angle_x > 90.0f)    angle_x = 90.0f;    else
        if (angle_x < -90.0f)   angle_x = -90.0f;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    /*
    * mian function
    * load the display window in the center of the midlle of the screen
    * named "CPT205 Assessment 2: 3D Modelling Project"
    */
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutSetOption(GLUT_MULTISAMPLE, 8);
    POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
    POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
    glutInitWindowPosition(POS_X, POS_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("CPT205 Assessment 2: 3D Modelling Project");
    init();
    
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(0, timer, 0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
