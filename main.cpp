#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
using std::string;
using std::fstream;
using std::ios;
using std::cout;
using std::endl;

//���� 
void NewLine(string &f)
{
	f.append(1,'\n');
}
//���� 
void Tab(string& f,int tabs)
{
	const int space = 2;
	if(tabs>0) f.append(tabs*space,' ');
}

//����ַ��������л��з��Լ�����ո� 
void ClearFormat(string& f)
{
	bool neword = false;			//�µ�����ʼ��־
	bool quote = false;				//����ģʽ 
	bool precode = false;			//Ԥ����ģʽ
	unsigned char commentMode = 0;	//λģʽ

	//λ���ر�ʶ��
	/*  0x80 ������ע�� 
		0x40 �ճ��ֹ�/����
		0x20 �ճ��ֹ�*����
		0x10 �������ע�͸ս���(��ζ������'*''/'���)
		0x81 �������ע��
		0x02 �������':'���������з���0
	*/
	
	string newf="";
	string::iterator iter = f.begin();
	char c = '\0';
	while(iter!=f.end())
	{
		c = *iter;

		if(c==':') commentMode |=0x02;

		if(c=='\r' || c=='\t')
		{
			iter++;
			continue;
		}
		//��ʽ�����໻�з�
		if(c=='\n')
		{
			//Ԥ����ע�͡������еĻ��з�����
			if((commentMode&0x80)!=0)
			{
				NewLine(newf);
				//��Ϊ����ע��ģʽ�����������з���Ϊע�ͽ���
				if((commentMode&0x01)==0) commentMode=0;
			}
			else if(quote)
			{
				NewLine(newf);
			}
			else if(precode)
			{
				NewLine(newf);
				precode = false;
			}
			else if(commentMode&0x10)
			{
				//����ע�͸ս����������з�
				commentMode =0;
				NewLine(newf);
			}
			else if(commentMode&0x02)
			{
				commentMode&=(~0x02);
				if( !quote && !(commentMode&0x80) ) NewLine(newf);
			}

			iter++;
			continue;
		}
		
		if((c=='\"' || c=='\'')&& (!(commentMode&0x80)) &&(!precode))
		{
			quote = !quote;
		}
		//��ע�ͻ�����ģʽ�³���'#'����ζ��Ԥ������䣬������'##'������
		if(c=='#' &&(!quote) && (!(commentMode&0x80)) ) precode =true;

		if((!quote)&&(!(commentMode&0x80)))
		{
			//����������ģʽ��ע��ģʽʱ��ʽ��
			//��ʽ������ո�
			if(c==' ')
			{
				if(neword)
				{
					newf.append(1,c);
					neword = false;
				}
			}
			else if(isalnum(c))
			{
				neword = true;
				newf.append(1,c);
			}
			else
			{
				//ָ����ַ���÷��ź�ո���
				switch (c)
				{
					case '*':
					case '&':
					case '_':
						neword = true;
						break;
					default:
						neword = false;
				}
				newf.append(1,c);
			}
			//ʶ��ע�ͱ�־
			if(c=='/')
			{
				if((commentMode&0x40)!=0) commentMode |=0x80;
				commentMode &=(~0x20);
				commentMode |= 0x40;
			}
			else if(c=='*')
			{
				if((commentMode&0x40)!=0) commentMode |=0x81;
				commentMode &=(~0x40);
				commentMode |= 0x20;
			}
			else
			{
				commentMode &= (~(0x40|0x20));
			}
		}
		else
		{	//�������û���ע���򲻸�ʽ��
			if(commentMode&0x80)
			{
				if(c=='*') commentMode |=0x20;
				else if(c=='/' &&(commentMode&0x20)) commentMode = 0x10;
				else commentMode &= (~(0x40|0x20|0x10));
			}
			newf.append(1,c);	
		}
		iter++;
	}
	f = newf;
}

void Reformat(string& f)
{
	string newf;
	int tabs=0;
	
	bool afterNL = false;		//��ʾ�Ƿ��Ѿ�����
	bool backslash = false;		//��б���������

	unsigned char mode = 0;
	unsigned char ct = 0;		//Բ���ŵ��Ӵ����������ж��Ƿ�������״̬
	/*
	λģʽ��
	0x80 ����ע��
	0x90 ����ע��
	0x40 ��һѭ������'/'
	0x20 ��һѭ������'*'
	0x08 ����״̬
	0x04 ����״̬��Ĭ��˫���ţ�
	0x02 Ԥ����״̬
	0x05 ������ģʽ
	����ע��״̬0x80������״̬0x08������״̬0x04��Ԥ����0x02 ��Чʱλ��Ϊ1
	����״̬������״̬������Ч����֮������
	�����״̬����
	*/
	 
	string::iterator iter=f.begin();
	char c='\0';
	while(iter!=f.end())
	{
		c= *iter;

		if(c=='(' && !(mode&(0x80|0x04|0x02)))
		{
			mode |=0x08;
			ct++;
		}
		else if (c==')' && !(mode&(0x80|0x04|0x02)))
		{
			if(--ct==0)mode &=(~0x08);
		}
		else if(c=='#' && !(mode&(0x80|0x08|0x04)))
		{
			mode |= 0x02;
		}
		else if((c=='\"') &&!backslash &&!(mode&(0x80|0x02)) )
		{
			if(!(mode&0x04))mode|=0x04;
			else if((mode&0x05)==0x04) mode &=(~0x04);
		}
		else if(c=='\'' &&!backslash &&!(mode&(0x80|0x02)) )
		{
			if(!(mode&0x04))mode|=0x05;
			else if((mode&0x05)==0x05) mode &=(~0x05);
		}
		else if(c=='/' && !(mode&(0x08|0x04|0x02)))
		{
			if(mode&0x40) mode|=0x80;else if(mode&0x20) mode &=(~0x90);
			mode &=(~0x20);
			mode |= 0x40;
		}
		else if(c=='*' && !(mode&(0x08|0x04|0x02)))
		{
			if(mode&0x40) mode|=0x90;
			mode &=(~0x40);
			mode |= 0x20;
		}
		else if(c=='\n')
		{
			if(!(mode&(0x08|0x04|0x02)))
			{
				//����ǰΪ����ע��ģʽ�������з�����Ϊע�ͽ���
				if((mode&(0x80|0x90))==0x80) mode&=(~0x80);
			}
			else if(!(mode&(0x80|0x08|0x04)))
			{
				mode &=(~0x02);		//Ԥ����ģʽ��������������ΪԤ�������
			}
		}

		backslash =((c=='\\')?true:false);

		if(c!='*' && c!='/')
		{
			mode &=(~(0x40|0x20));
		}
		//������ע�͡����š����š�Ԥ������һģʽʱ�򲻽��и�ʽ��
		if(mode&(0x80|0x08|0x04|0x02))
		{
			newf.append(1,c);
			afterNL=( (c=='\n')?true:false );
			iter++;
			continue;
		} 
		//��ʽ��ʶ��
		switch(c)
		{
			case '{': 
				if(!afterNL)
				{
					NewLine(newf);
					afterNL=true;
				}
				Tab(newf,tabs);
				newf.append(1,'{');
				tabs+=1;
				NewLine(newf);
				afterNL=true;
				break;
			case '}':
				if(!afterNL)
				{
					NewLine(newf);
					afterNL=true;
				}
				tabs-=1;
				Tab(newf,tabs);
				newf.append(1,'}');
				NewLine(newf);
				afterNL=true;
				break;
			case ';':
				newf.append(1,';');
				NewLine(newf);
				afterNL=true;
				break;
			case '#':
				if(!afterNL)
				{
					NewLine(newf);
					afterNL=true;
				}
				Tab(newf,0);
				newf.append(1,'#');
				break;
			default:
				if(!(c=='\n'&&afterNL))
				{
					//���Ѿ��������ٴγ��ֻ��з���ֱ�Ӻ��Ըò���
					//���к�����
					if(afterNL) Tab(newf,tabs);
					newf.append(1,c);
				}
				afterNL=( (c=='\n')?true:false );
				break;					
		}
		iter++;
	}
	
	f=newf;
}

bool Format(char *fname)
{
	//�򿪴��Ű��ļ�
	fstream codefile;
	codefile.open(fname,ios::in|ios::out);
	if(!codefile.is_open())
	{
		codefile.close();
		return false;
	}
	//�õ��ļ���С
	codefile.seekg(0,ios::end);
	int len = (int)codefile.tellg();
	//��ȡ�ļ���c_string
	codefile.seekg(0);
	char* mydoc = new char[len];
	codefile.get(mydoc,len,EOF);
	codefile.close();
	//�Ű�
	string document = mydoc;
	delete[] mydoc;
	ClearFormat(document);
	Reformat(document);
	//�Ű�����������ļ���
	fstream nfile("new.cpp",ios::out);
	nfile<<document;
	nfile.close();

	return nfile.good();
}

int main(int argc, char** argv) 
{
	//string mycpp1=
	//"int _tmain(int argc, _TCHAR* argv[])\n{\nstd::string f=\"hello:world\";\ncase:\nreturn 0;\n}";
	//string mycpp="int main()\n{\tstd::string f;\n\tcase:\n\treturn 0;\n}";
	//std::cout<<"=====Origin Code:====="<<endl;
	//cout<<mycpp;
	//cout<<endl<<"====Clear Format:===="<<endl;
	//ClearFormat(mycpp);
	//std::cout<<mycpp;

	//cout<<endl<<"====After Format===="<<endl;
	//Reformat(mycpp);
	//std::cout<<mycpp;

	//system("pause");
	//return 0;
	if (argc!=2)
	{
		std::cout<<"��������\n";
	}
	else
	{
		if(Format(argv[1])==false)
		{
			std::cout<<"�޷����Ű��ļ���\n"<<"File::\\\\ "<<argv[1]; 
		}
		else
		{
			std::cout<<"�Ű���ɣ��������Ŀ¼�²鿴!\n";
		}
	}
	system("pause");
	return 0;	
	
}
