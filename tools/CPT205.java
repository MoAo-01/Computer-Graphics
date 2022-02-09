package tools;

import java.awt.*;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.PrintStream;

public class CPT205 {
    public static void main(String[] args) throws FileNotFoundException {
        PrintStream ps = new PrintStream("C:\\Users\\77007\\Desktop\\CPT205\\AS1\\words.cpp");
        System.setOut(ps);//把创建的打印输出流赋给系统。即系统下次向 ps输出


        Picture test = new Picture("C:\\Users\\77007\\Desktop\\CPT205\\AS1\\words.png");

        int h = test.height(), w = test.width();
        // preparation
        System.out.println("#define FREEGLUT_STATIC");
        System.out.println("#include <GL/freeglut.h>");
        System.out.println("#define V2 glVertex2f");
        System.out.println("using namespace std;");
        System.out.println("void CB();");

        // general main function here
        System.out.println("int main(int argc, char** argv)");
        System.out.println("{");
        System.out.println("    glutInit(&argc, argv);");
        System.out.println("    int w = " + w  + ", h = " + h + ";");
        System.out.println("    glutInitWindowSize(w, h);");
        System.out.println("    glutInitWindowPosition(0, 0);");
        System.out.println("    glutCreateWindow(\"XJTLU 15 anniversary\");");
        System.out.println("    glutDisplayFunc(CB);");
        System.out.println("    glutMainLoop();");
        System.out.println("}");


        // general assignment1 function here
        System.out.println("void CB()");
        System.out.println("{");
        System.out.println("    glClearColor(1, 1, 1, 1);");
        System.out.println("    glClear(GL_COLOR_BUFFER_BIT);");
        System.out.println("    glMatrixMode(GL_PROJECTION);");
        System.out.println("    glLoadIdentity();");
        System.out.println();

        System.out.println("    gluOrtho2D(" + 0 + ", " + w + ", " + 0 + ", " + h + ");");
        System.out.println("    glLineWidth(1.0);");
        System.out.println("    glColor3f(0.0, 0.0, 0.0);");

        System.out.println();
        System.out.println("    glBegin(GL_POINTS);");
        int v = 30;// 扣背景，阈值
        Picture ans = new Picture(w, h);
        int cnt = 0;
        System.out.print("    ");
        for (int col = 0; col < w; col++) {
            for (int row = 0; row < h; row++) {
                Color c = test.getColor(col, row);
                int r = c.getRed(), g = c.getGreen(), b = c.getBlue();
                if (!(r>=255-v && g>=255-v && b>=255-v)) {

                    ans.setColor(col, row, c);
                    //System.out.println("    glColor3d(" + r + ", " + g + ", " + b + ");");
                    System.out.print("V2(" + col + ", " + (h - row) + ");");
                    if ((++cnt) % 7 == 0) System.out.print("\n    ");

                }

            }
        }
        System.out.println("    glEnd();");
        System.out.println();
        System.out.println("    glFlush();");
        System.out.println("}");
        ans.show();

        //System.out.println(h + " " + w);
    }
}