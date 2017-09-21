//
// Created by xingkong on 2017/8/10.
//

#ifndef NATIVEGL_DEMO2_GL_PARAMS_H
#define NATIVEGL_DEMO2_GL_PARAMS_H

#pragma once


//图形参数
const int POINTS=200;  //扇形弧面采点数
const float Sang=1.047; //扇扫扇形的角度
const float ang_channel=Sang/49.0;  //每个通道的角度
const float texture_step=1.0/(POINTS-1.0);  //texture采点步距
const float angle_step=Sang/(POINTS-1.0);  //扇形采点角度步距
//const float width_height=800.0/480; //根据屏幕宽高比拉伸图像
const float R=2.0; //扇形半径
const float oy=R/2;    //圆心纵坐标


//S扫texture坐标
extern unsigned char *sb_data;  //S扫数据指针



#endif //PHASEDARRAY2_0_GL_PARAMS_H
