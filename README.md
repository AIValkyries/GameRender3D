# GameRender3D
# GameRender3D

#主要约定
   左手坐标系
   列向量，列矩阵
       /* 坐标系 
	       +y
	       ^   +z
	       |  /
         | /
		     *---->+x
	   */
#数学库功能实现
   *1.基本函数库
   *2.cos和sin优化
   *3.向量
   *4.矩阵
   *5.四元数
   *6.旋转表达式实现
＃软光栅功能实现
   *1.线框渲染
   *2.三角形填充着色（Gouraud）
   *3.渲染管线
   *4.视锥体裁剪（主要裁剪近裁剪平面）
   *5.纹理采样
   *6.1/z缓存
   *7.背面剔除
   *8.透明
#基本光照功能实现
   *1.平行光、点光灯、聚光灯
   *2.漫反射、镜面反射
#纹理功能实现
    *1.纹理映射UV
    *2.纹理线性和双线性过滤模式
    *3.寻址模式
    *4.透明度混合
    *5.透视矫正
    *6.Mipmap
    *7.法线纹理生成
    *8.png纹理读取，jpeg纹理读取，bmp纹理读取
#蒙皮骨骼
    *1.OGRE网格读取
    *2.主要动画，蒙皮过程实现
#地形功能实现
    *1.高度图生成（柏林噪声）
    *2.地形顶点和索引生成
    *3.ROAM LOD实现
#场景功能实现
    *1.AABB包围盒
    *2.AABB碰撞检测
    *3.四叉树场景管理
    *4.天空盒
    *5.摄像机
   

