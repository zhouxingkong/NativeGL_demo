//
// Created by xingkong on 2017/8/14.
//

#include "GLrenderS.h"

//S扫texture用的shader
auto tVertexShader =
        "attribute vec2 a_Position;\n"  //屏幕坐标
                "attribute vec2 a_TextureCoordinates;\n"//纹理坐标
                "varying vec2 v_TextureCoordinates;\n"
                "void main() {\n"
                "   v_TextureCoordinates = a_TextureCoordinates;\n"
                "   gl_Position = vec4(a_Position,0,1.0);\n"
                "}\n";
auto tFragmentShader =
        "precision mediump float;\n"
                "uniform sampler2D u_TextureUnit;\n"    //S扫纹理采样器
                "varying vec2 v_TextureCoordinates;\n"  //采样坐标
                "vec4 color_temp;\n"
                "void main() {\n"
                "   color_temp = texture2D(u_TextureUnit, v_TextureCoordinates);\n" //使用v_TextureCoordinates采样u_TextureUnit中的颜色
                "   if(color_temp.r<0.3125){\n"        //使用伪彩色映射
                "       color_temp=vec4(1.0-2.9875*color_temp.r,1.0-2.4*color_temp.r,1.0-0.1875*color_temp.r,1.0);\n"
                "   }\n"
                "   else if(color_temp.r<0.391){\n"
                "       color_temp=vec4(0.0627+0.8*(color_temp.r-0.3125),0.25+6.4*(color_temp.r-0.3125),0.9375-11.2*(color_temp.r-0.3125),1.0);\n"
                "   }\n"
                "   else if(color_temp.r<0.46875){\n"
                "       color_temp=vec4(0.125+8.0*(color_temp.r-0.391),0.75-1.6*(color_temp.r-0.391),0.0625-0.4*(color_temp.r-0.391),1.0);\n"
                "   }\n"
                "   else if(color_temp.r<0.625){\n"
                "       color_temp=vec4(0.75+1.2*(color_temp.r-0.46875),0.625-1.6*(color_temp.r-0.46875),0.03127-0.2*(color_temp.r-0.46875),1.0);\n"
                "   }\n"
                "   else{\n"
                "       color_temp=vec4(0.9375-1.2632*(color_temp.r-0.625),0.375-1.0*(color_temp.r-0.625),0.0,1.0);\n"
                "   }\n"
                "   gl_FragColor=color_temp;\n"
                "}\n";


GLrenderS::GLrenderS(){

}
GLrenderS::~GLrenderS(){

}

//读取texture
void GLrenderS::loadTexture(){
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(uTextureUnitLocation, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 1024, 64, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, sb_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //放大缩小使用的函数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GLrenderS::cal_pixel() {
    int i;
    float theta;
    for(i=0;i<POINTS;i++){
        gTextureVertices[4*i]=0;    //计算texture分块区域
        gTextureVertices[4*i+1]=texture_step*i*49.0/64.0;
        gTextureVertices[4*i+2]=1.0*1000.0/1024.0;
        gTextureVertices[4*i+3]=texture_step*i*49.0/64.0;

        theta=-2.094+i*angle_step;
        gScreenVertices[4*i]=0.0;//计算屏幕上对应分块区域（扇形）
        gScreenVertices[4*i+1]=oy;
        gScreenVertices[4*i+2]=0.0+R*cos(theta);
        gScreenVertices[4*i+3]=oy+R*sin(theta);
    }
    sb_data=new unsigned char[1024*64];
    memset(sb_data,100,1024*64);

}

void GLrenderS::SurfaceCreate() {
    tProgram = createProgram(tVertexShader, tFragmentShader);

    uTextureUnitLocation = glGetUniformLocation(tProgram, "u_TextureUnit");
    aPositionLocation  = glGetAttribLocation(tProgram, "a_Position");
    aTextureCoordinatesLocation = glGetAttribLocation(tProgram, "a_TextureCoordinates");

    loadTexture();  //读取texture
    glClearColor(0.0,0.0,0.0,1.0f);

    glGenBuffers(2, texture_buffer_id);     //生成存储texture坐标的buffer
    glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_id[0]);
    glBufferData(GL_ARRAY_BUFFER, 4*POINTS * sizeof(GLfloat), gScreenVertices, GL_STATIC_DRAW); //扇形坐标
    glEnableVertexAttribArray(aPositionLocation);
    glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_id[1]);
    glBufferData(GL_ARRAY_BUFFER, 4*POINTS * sizeof(GLfloat), gTextureVertices, GL_STATIC_DRAW);//矩形坐标
    glEnableVertexAttribArray(aTextureCoordinatesLocation);
}

void GLrenderS::SurfaceChange(int width, int height) {
    glViewport(0, 0, width, height);
}


void GLrenderS::DrawFrame(){
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //画Texture
    glUseProgram(tProgram); //使用texture的program
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,1000,49,GL_LUMINANCE, GL_UNSIGNED_BYTE,sb_data);  //更新显示图像
    glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_id[0]);    //使用VBO中存储的点
    glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);    //读取纹理坐标
    glEnableVertexAttribArray(aPositionLocation);
    glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_id[1]);
    glVertexAttribPointer(aTextureCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(aTextureCoordinatesLocation);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2*POINTS);    //显示纹理
    glBindBuffer(GL_ARRAY_BUFFER, 0);   //用完buffer以后解绑定

}

