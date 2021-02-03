#pragma once
#ifndef __MathTest_H__
#define __MathTest_H__

#include "lMathf.h"
#include <iostream>
#include<iomanip>

using namespace std;

namespace Lonely
{
	static void DebugVector2(string Name, Vector2 temp)
	{
		cout << "[" << Name.c_str() << "]" << "测试" << endl;
		cout << "[X=" << temp.x << ",Y=" << temp.y << "]" << endl;
	}

	static void DebugFloat32(string Name,float32 value)
	{
		cout << Name.c_str() << "=" << value << endl;
	}

	static void Vector2Test()
	{
		Vector2 temp1(2.5F, 4.8F);
		Vector2 temp2(12.5F, 14.8F);
		float32 s = 5;

		cout << "X=" << temp1[0] << endl;
		cout << "Y=" << temp1[1] << endl;

		temp1 += temp2;
		DebugVector2("+=", temp1);

		temp1 -= temp2;
		DebugVector2("-=", temp1);

		temp1 *= s;
		DebugVector2("*=", temp1);

		temp1 /= s;
		DebugVector2("/=", temp1);

		if (temp1 == temp2)
		{
			cout << "==结果" << endl;
		}
		if (temp1 != temp2)
		{
			cout << "!=结果" << endl;
		}

		float32 angle = Angle(temp1, temp2);
		DebugFloat32("Angle", angle);

		float32 distance = Distance(temp1, temp2);
		DebugFloat32("Distance", distance);

		float32 dot = Dot(temp1, temp2);
		DebugFloat32("Dot", dot);

		Vector2 lerp = Lerp(temp1, temp2, 0.5F);
		DebugVector2("Lerp", lerp);

		Vector2 max = Max(temp1, temp2);
		DebugVector2("Max", max);

		Vector2 min = Min(temp1, temp2);
		DebugVector2("Min", min);

		Vector2 reflect = Reflect(temp1, temp2);
		DebugVector2("Reflect", reflect);

		Vector2 project = Project(temp1, temp2);
		DebugVector2("Project", project);

		float32 dot2 = temp1.Dot(temp2);
		DebugFloat32("Dot2", dot2);

		float32 magn = temp1.Magnitude();
		DebugFloat32("Magnitude", magn);

		float32 sqrtMagn = temp1.SqrMagnitude();
		DebugFloat32("SqrMagnitude", sqrtMagn);

		float32 angle2 = temp1.Angle(temp2);
		DebugFloat32("Angle2", angle2);

		float32 distance2 = temp1.Distance(temp2);
		DebugFloat32("Distance2", distance2);
	}

	static void DebugVector3Test(string Name, Vector3 temp)
	{
		cout << "[" << Name.c_str() << "]" << "测试" << endl;
		cout << "[X=" << temp.x << ",Y=" << temp.y << ",Z=" << temp.z << "]" << endl;
	}

	static void Vector3Test()
	{
		Vector3 temp1(2.5F, 4.8F,1.5F);
		Vector3 temp2(12.5F, 14.8F, 10.0F);
		float32 s = 5;

		cout << "X=" << temp1[0] << endl;
		cout << "Y=" << temp1[1] << endl;
		cout << "Z=" << temp1[2] << endl;

		temp1 += temp2;
		DebugVector3Test("+=", temp1);

		temp1 -= temp2;
		DebugVector3Test("-=", temp1);

		temp1 *= s;
		DebugVector3Test("*=", temp1);

		temp1 /= s;
		DebugVector3Test("/=", temp1);

		if (temp1 == temp2)
		{
			cout << "==结果" << endl;
		}
		if (temp1 != temp2)
		{
			cout << "!=结果" << endl;
		}

		float32 dot = Dot(temp1, temp2);
		DebugFloat32("Dot", dot);

		float32 angle = Angle(temp1, temp2);
		DebugFloat32("Angle", angle);

		float32 distance = Distance(temp1, temp2);
		DebugFloat32("Distance", distance);

		Vector3 cross = Cross(temp1, temp2);
		DebugVector3Test("Cross", cross);

		Vector3 lerp = Lerp(temp1, temp2, 0.5F);
		DebugVector3Test("Lerp", lerp);

		Vector3 max = Max(temp1, temp2);
		DebugVector3Test("Max", max);

		Vector3 min = Min(temp1, temp2);
		DebugVector3Test("Min", min);

		Vector3 reflect = Reflect(temp1, temp2);
		DebugVector3Test("Reflect", reflect);

		Vector3 project = Project(temp1, temp2);
		DebugVector3Test("Project", project);

		float32 dot2 = temp1.Dot(temp2);
		DebugFloat32("Dot2", dot2);

		float32 magn = temp1.Magnitude();
		DebugFloat32("Magnitude", magn);

		float32 sqrtMagn = temp1.SqrMagnitude();
		DebugFloat32("SqrMagnitude", sqrtMagn);
	}

	static void DebugVector4Test(string Name, Vector4 temp)
	{
		cout << "[" << Name.c_str() << "]" << "测试" << endl;
		cout << "[X=" << temp.x << ",Y=" << temp.y << ",Z=" << temp.z << ",W=" << temp.w << "]" << endl;
	}


	static void Vector4Test()
	{
		Vector4 temp1(2.5F, 4.8F, 1.5F, 1);
		Vector4 temp2(12.5F, 14.8F, 10.0F, 1);
		float32 s = 5;

		cout << "X=" << temp1[0] << endl;
		cout << "Y=" << temp1[1] << endl;
		cout << "Z=" << temp1[2] << endl;
		cout << "W=" << temp1[3] << endl;

		temp1 += temp2;
		DebugVector4Test("+=", temp1);

		temp1 -= temp2;
		DebugVector4Test("-=", temp1);

		temp1 *= s;
		DebugVector4Test("*=", temp1);

		temp1 /= s;
		DebugVector4Test("/=", temp1);

		if (temp1 == temp2)
		{
			cout << "==结果" << endl;
		}
		if (temp1 != temp2)
		{
			cout << "!=结果" << endl;
		}

		float32 dot = Dot(temp1, temp2);
		DebugFloat32("Dot", dot);

		float32 angle = Angle(temp1, temp2);
		DebugFloat32("Angle", angle);

		float32 distance = Distance(temp1, temp2);
		DebugFloat32("Distance", distance);

		Vector4 lerp = Lerp(temp1, temp2, 0.5F);
		DebugVector4Test("Lerp", lerp);

		Vector4 max = Max(temp1, temp2);
		DebugVector4Test("Max", max);

		Vector4 min = Min(temp1, temp2);
		DebugVector4Test("Min", min);

		Vector4 reflect = Reflect(temp1, temp2);
		DebugVector4Test("Reflect", reflect);

		Vector4 project = Project(temp1, temp2);
		DebugVector4Test("Project", project);
	}

	static void DebuglQuaternionTest(string Name, Quaternion temp)
	{
		cout << "[" << Name.c_str() << "]" << "测试" << endl;
		cout << "[X=" << temp.x << ",Y=" << temp.y << ",Z=" << temp.z << ",W=" << temp.w << "]" << endl;
	}

	static void QuaternionTest()
	{
		Quaternion temp1(2.5F, 4.8F, 1.5F, 1);
		Quaternion temp2(12.5F, 14.8F, 10.0F, 1);
		float32 s = 5;

		temp1 += temp2;
		DebuglQuaternionTest("+=", temp1);

		temp1 -= temp2;
		DebuglQuaternionTest("-=", temp1);

		temp1 *= s;
		DebuglQuaternionTest("*=", temp1);

		temp1 /= s;
		DebuglQuaternionTest("/=", temp1);

		temp1 *= temp2;
		DebuglQuaternionTest("*=", temp1);

		float32 dot = Dot(temp1, temp2);
		DebugFloat32("Dot", dot);

		Quaternion lerp = Lerp(temp1, temp2, 0.5F);
		DebuglQuaternionTest("Lerp", lerp);
		DebuglQuaternionTest("Conjugation", lerp.Conjugation());
		
		Quaternion slerp = Slerp(temp1, temp2, 0.5F);
		DebuglQuaternionTest("Slerp", slerp);
		DebuglQuaternionTest("Inverse", slerp.Inverse());
	}

#pragma region 矩阵测试

	static void DebugMatrix3x3Test(string Name, Matrix3x3 temp)
	{
		setprecision(6);
		cout << "[" << Name.c_str() << "]" << "测试" << endl;
		cout << temp[0] << " " << temp[1] << " " << temp[2] << endl;
		cout << temp[3] << " " << temp[4] << " " << temp[5] << endl;
		cout << temp[6] << " " << temp[7] << " " << temp[8] << endl;
	}

	static void Matrix3x3Test()
	{
		Matrix4x4 mat4;
		mat4[0] = 1.1F;
		mat4[1] = 2.2F;
		mat4[2] = 3.3F;
		mat4[3] = 4.4F;
		mat4[4] = 5.5F;
		mat4[5] = 6.6F;
		mat4[6] = 7.7F;
		mat4[7] = 8.8F;
		mat4[8] = 9.9F;
		mat4[9] = 10.1F;
		mat4[10] = 2.5F;
		mat4[11] = 3.5F;
		mat4[12] = 1.125F;
		mat4[13] = 2.33F;
		mat4[14] = 4.55F;
		mat4[15] = 10.2F;

		Matrix3x3 temp1;
		Matrix3x3 temp2;
		temp2 = mat4;

		Vector3 v3;
		float32 s = 5;

		temp1[0] = 15.5F;
		temp1[1] = 16.7F;
		temp1[2] = 18.2F;
		temp1[3] = 1.3F;
		temp1[4] = 2.1F;
		temp1[5] = 4.2F;
		temp1[6] = 1.5F;
		temp1[7] = 0.5F;
		temp1[8] = 0.23F;

		temp1 += temp2;
		temp1 -= temp2;
		temp1 *= s;
		temp1 *= v3;
		temp1 *= temp2;
		temp1 *= mat4;
		temp1 /= s;

		if (temp1 == temp2)
		{
			cout << "==结果" << endl;
		}
		if (temp1 != temp2)
		{
			cout << "!=结果" << endl;
		}

		Matrix3x3 temp3;
		Vector3 v32;
		temp3 = temp1 + temp2;
		temp3 = temp1 - temp2;
		temp3 = temp1 * s;
		temp3 = temp1 * temp2;
		v32 = temp1.MultiplyVector3(v3);
		temp3 = -temp1;

		Matrix3x3 temp4;
		temp4.SetIdentity();
		temp4.Scale(Vector3(1, 2, 3));

		temp1.Determinant();
		temp1.Transpose();
		temp1.Inverse();

		DebugVector3Test("GetColumn", temp1.GetColumn(0));
		DebugVector3Test("GetRow", temp1.GetRow(0));

		Matrix3x3 axisAngle = BuildAxisAngle(Vector3::up(), 0.2F);
		Matrix3x3 xAngle = BuildXAngle(0.2F);
		Matrix3x3 yAngle = BuildYAngle(0.2F);
		Matrix3x3 zAngle = BuildZAngle(0.2F);
	}

	static void DebugMatrix4x4Test(string Name, Matrix4x4 temp)
	{
		setprecision(6);
		cout << "\n[" << Name.c_str() << "]" << "测试\b" << endl;
		cout << temp[0] << " " << temp[1] << " " << temp[2] << " " << temp[3] << endl;
		cout << temp[4] << " " << temp[5] << " " << temp[6] << " " << temp[7] << endl;
		cout << temp[8] << " " << temp[9] << " " << temp[10] << " " << temp[11] << endl;
		cout << temp[12] << " " << temp[13] << " " << temp[14] << " " << temp[15] << endl;
	}

	static void Matrix4x4Test()
	{
		Matrix4x4 temp1;
		Matrix4x4 temp2;
		Vector4 v4(0.25F, 1.254F, 3.168F, 1);
		float32 s = 5;

		temp1[0] = 1.1F;
		temp1[1] = 2.2F;
		temp1[2] = 3.3F;
		temp1[3] = 4.4F;
		temp1[4] = 5.5F;
		temp1[5] = 6.6F;
		temp1[6] = 7.7F;
		temp1[7] = 8.8F;
		temp1[8] = 9.9F;
		temp1[9] = 10.1F;
		temp1[10] = 2.5F;
		temp1[11] = 3.5F;
		temp1[12] = 1.125F;
		temp1[13] = 2.33F;
		temp1[14] = 4.55F;
		temp1[15] = 10.2F;

		temp2[0] = 10.25F;
		temp2[1] = 1.2222F;
		temp2[2] = 3.1F;
		temp2[3] = 3.1415F;
		temp2[4] = 0.2689F;
		temp2[5] = 1.2544F;
		temp2[6] = 0.2551F;
		temp2[7] = 10.545F;
		temp2[8] = 11.245F;
		temp2[9] = 1.2585F;
		temp2[10] = 1.3549F;
		temp2[11] = 10.125F;
		temp2[12] = 10.2548F;
		temp2[13] = 0.25451;
		temp2[14] = 12.35F;
		temp2[15] = 10.1555;

		temp1 += temp2;
		DebugMatrix4x4Test("+=", temp1);

		temp1 -= temp2;
		DebugMatrix4x4Test("-=", temp1);

		temp1 *= s;
		DebugMatrix4x4Test("*=", temp1);

		temp1 /= s;
		DebugMatrix4x4Test("/=", temp1);

		//temp1 *= temp2;
		//DebugMatrix4x4Test("*=", temp1);

		//temp1 *= v4;
		//DebugMatrix4x4Test("*=", temp1);

		if (temp1 == temp2)
		{
			cout << "==结果" << endl;
		}
		if (temp1 != temp2)
		{
			cout << "!=结果" << endl;
		}

		Matrix4x4 temp3;
		Vector4 v32;
		temp3 = temp1 + temp2;
		DebugMatrix4x4Test("+", temp3);

		temp3 = temp1 - temp2;
		DebugMatrix4x4Test("-", temp3);

		temp3 = temp1 * s;
		DebugMatrix4x4Test("*S", temp3);

		temp3 = temp1 * temp2;
		DebugMatrix4x4Test("矩阵与矩阵乘法", temp3);

		temp3 = temp2 * temp1;
		DebugMatrix4x4Test("矩阵与矩阵乘法", temp3);

		v32 = temp1.MultiplyVector4(v4);
		DebugVector4Test("*v4", v32);

		//temp3 = -temp1;
		//DebugMatrix4x4Test("-", temp3);

		float32 determinant = temp1.Determinant();
		DebugFloat32("Determinant", determinant);
		 
		DebugMatrix4x4Test("temp1", temp1);

		Matrix4x4  inverse = temp1.Inverse();
		DebugMatrix4x4Test("Inverse", inverse);

		DebugVector4Test("GetColumn", temp1.GetColumn(0));
		DebugVector4Test("GetRow", temp1.GetRow(0));

		Vector4 pos(10, 20, 30);
		float32 angle = Sin(20 * DEG_TO_RAD);
		float32 cA = Cos(20 * DEG_TO_RAD);
		Quaternion q(angle, cA, cA, cA);
		Vector3 scale = Vector3::zero() * 3;

		Matrix4x4 temp4;
		temp4.SetTRS(pos, q, scale);

		DebugMatrix4x4Test("SetTRS", temp4);
	}

	static void ProjectionMatrixTest()
	{
		float32 oleft = 2;
		float32 oright = 3.5F;
		float32 t = 2.5F;
		float32 b = 4.5F;
		float32 of = 10.5F;
		float32 on = 2.3F;

		Matrix4x4 oMat;
		oMat = MatrixOrthoLH(oleft, oright, t, b, on, of);
		DebugMatrix4x4Test("正交投影矩阵", oMat);

		float32 fov = 30;
		float32 aspect = 1.2F;
		float32 zn = 10;
		float32 zf = 100;

		Matrix4x4 pMat;
		pMat = MatrixPerspectiveFovLH(fov, aspect, zn, zf);
		DebugMatrix4x4Test("透视投影矩阵", pMat);
	}

	static void TransformTest()
	{
		Vector3 euler(20, 30, 10);

		Matrix3x3 temp1;
		EulerToMatrix3x3(euler, temp1);
		DebugMatrix3x3Test("EulerToMatrix", temp1);

		//cout << "-------------QuaternionTest_START-----------------" << endl;
		//Quaternion q1;
		//EulerToQuaternion(euler, q1);
		//DebuglQuaternionTest("EulerToQuaternion=", q1);

		//Quaternion qtemp(Cos(20), Sin(10), Sin(10), Sin(10));
		//Vector3 euler2;
		//Matrix3x3 qMat;
		//QuaternionToEuler(qtemp, euler2);
		//cout << "-------------四元数to欧拉角_START-----------------" << endl;
		//DebugVector3Test("欧拉角=", euler2);
		//QuaternionToMatrix(qtemp, qMat);
		//cout << "-------------四元数to矩阵_START-----------------" << endl;
		//DebugMatrix3x3Test("矩阵=", qMat);

		//Matrix4x4 matrix;
		//matrix[0] = 1; matrix[1] = 12; matrix[2] = 2.3555F; matrix[3] = 0;
		//matrix[4] = 2.5F; matrix[5] = 3.2F; matrix[6] = 0.12548F; matrix[7] = 0;
		//matrix[8] = 1.25F; matrix[9] = 0.1F; matrix[10] = 1.2564F; matrix[11] = 0;
		//matrix[12] = 0; matrix[13] = 0; matrix[14] = 0; matrix[15] = 1;
		//Vector4 euler3;
		//Quaternion q3;
		//MatrixToEuler(matrix, euler3);
		//DebugVector4Test("矩阵转换成欧拉角=", euler3);

		//MatrixToQuaternion(matrix, q3);
		//DebuglQuaternionTest("矩阵转换成四元数=", q3);
	}

#pragma endregion


	static void MathTest()
	{
		//cout << "-------------Vector2Test_START-----------------" << endl;
		//Vector2Test();
		//cout << "-------------Vector2Test_END-----------------" << endl;

		//cout << "-------------Vector3Test_START-----------------" << endl;
		//Vector3Test();
		//cout << "-------------Vector3Test_END-----------------" << endl;
	
		//cout << "-------------Vector4Test_START-----------------" << endl;
		//Vector4Test();
		//cout << "-------------Vector4Test_END-----------------" << endl;

		//cout << "-------------QuaternionTest_START-----------------" << endl;
		//QuaternionTest();
		//cout << "-------------QuaternionTest_END-----------------" << endl;
	
		//cout << "-------------Matrix3x3Test_START-----------------" << endl;
		//Matrix3x3Test();
		//cout << "-------------Matrix3x3Test_END-----------------" << endl;

		//cout << "-------------Matrix4x4Test_START-----------------" << endl;
		//Matrix4x4Test();
		//cout << "-------------Matrix4x4Test_END-----------------" << endl;

		/*cout << "-------------ProjectionMatrixTest_START-----------------" << endl;
		ProjectionMatrixTest();
		cout << "-------------ProjectionMatrixTest_END-----------------" << endl;*/

		cout << "-------------TransformTest_START-----------------" << endl;
		TransformTest();
		cout << "-------------TransformTest_END-----------------" << endl;

	}


}


#endif

