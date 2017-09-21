//
// Created by xingkong on 2017/8/14.
//

#ifndef NATIVEGL_DEMO2_GLRENDERS_H
#define NATIVEGL_DEMO2_GLRENDERS_H

#include "GLRenderer.h"
#include "gl_params.h"
class GLrenderS : public GLRenderer{

private:
    GLuint tProgram;

    GLuint uTextureUnitLocation;
    GLuint aPositionLocation ;
    GLuint aTextureCoordinatesLocation;

    GLuint textureId;

    GLuint texture_buffer_id[2];

    GLfloat gTextureVertices[4*POINTS]; //纹理图上采点坐标
    GLfloat gScreenVertices[4*POINTS];  //屏幕上采点坐标

public:
    GLrenderS();
    ~GLrenderS();
    void loadTexture();
    void cal_pixel();
    void SurfaceCreate();
    void SurfaceChange(int width, int height);
    void DrawFrame();
};


#endif //PHASEDARRAY2_0_GLRENDERS_H
