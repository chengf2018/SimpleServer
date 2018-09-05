#pragma once

//系统协议
enum enumProtocol
{
	cmd_Invalid = 0,//无效
	cmd_LoginSystem = 1,//登录子系统
	cmd_UserInfoSystem = 2,//用户信息系统
};

//登录子系统
enum enumLoginSystem
{
	LoginSystem_cRegister = 1, //注册
	LoginSystem_cLogin = 2,//登录
	LoginSystem_cChangePassWord = 3,//修改密码

	LoginSystem_sRegister = 1, //注册结果下发
	LoginSystem_sLogin = 2,//登录结果下发
	LoginSystem_sChangePassWord = 3,//修改密码结果下发
};