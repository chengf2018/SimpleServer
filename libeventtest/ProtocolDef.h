#pragma once

//ϵͳЭ��
enum enumProtocol
{
	cmd_Invalid = 0,//��Ч
	cmd_LoginSystem = 1,//��¼��ϵͳ
	cmd_UserInfoSystem = 2,//�û���Ϣϵͳ
};

//��¼��ϵͳ
enum enumLoginSystem
{
	LoginSystem_cRegister = 1, //ע��
	LoginSystem_cLogin = 2,//��¼
	LoginSystem_cChangePassWord = 3,//�޸�����

	LoginSystem_sRegister = 1, //ע�����·�
	LoginSystem_sLogin = 2,//��¼����·�
	LoginSystem_sChangePassWord = 3,//�޸��������·�
};