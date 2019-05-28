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
	//f.append(1,'\r');
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
	bool neword = false;
	bool quote = false;				//����ģʽ 
	bool precode = false;			//Ԥ����ģʽ
	unsigned char commentMode = 0;	//λģʽ

	//λ���ر�ʶ��
	/*  0x80 ������ע�� 
		0x40 �ճ��ֹ�/����
		0x20 �ճ��ֹ�*���� 
		0x81 �������ע��
	*/ 
	
	string newf="";
	string::iterator iter = f.begin();
	char c = '\0';
	while(iter!=f.end())
	{
		c = *iter;
		
		if(c=='\r' || c=='\t')
		{
			iter++;
			continue;
		}
		
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
			iter++;
			continue;
		}
		
		if(c=='\"')
		{
			quote = !quote;
		}
		//��ע�ͻ�����ģʽ�³���'#'����ζ��Ԥ������䣬������'##'������
		if(c=='#' &&(!quote) && (!(commentMode&0x80)) ) precode =true;

		if((!quote)&&((commentMode&0x80)==0))
		{

			if(c==' ' && !neword)
			{
			}
			else if(c==' ' && neword)
			{
				neword = false;
				newf.append(1,c);
			}
			else if(isalnum(c))
			{
				neword = true;
				newf.append(1,c);
			}
			else
			{
				if(c=='/')
				{
					if((commentMode&0x40)!=0) commentMode |=0x80;
					commentMode |= 0x40;
				}
				else if(c=='*')
				{
					if((commentMode&0x40)!=0) commentMode |=0x81;
					commentMode |= 0x20;
				}
				else
				{
					commentMode &= (~0x60);
				}
				//ָ����ַ���÷��ź�ո���
				if(!(c=='*' || c=='&')) neword=false;
				newf.append(1,c);
			}	
		}
		else
		{	//�������û���ע���򲻸�ʽ��
			if(commentMode&80)
			{
				if(c=='*') commentMode &=0x20;
				else if(c=='/' &&(commentMode&0x20)) commentMode = 0;
				else commentMode &= (~0x60);
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
	bool paramMode = false;		//�ж��Ƿ�Ϊ����ģʽ����������״̬�� 
	bool quoteMode = false;
	 
	string::iterator iter=f.begin();
	char c='\0';
	while(iter!=f.end())
	{
		c= *iter;
		if(c=='(' && !paramMode)
		{
			paramMode = true;
		}
		else if (c==')' && paramMode)
		{
			paramMode = false;
		}
		else if(c=='"')
		{
			quoteMode =!quoteMode;
		}
		
		if(quoteMode||paramMode)
		{
			//���û����ģʽ�򲻸�ʽ��
			newf.append(1,c);
			iter++;
			continue;
		} 
		
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
				if(afterNL) Tab(newf,tabs);
				newf.append(1,c);
				if(c!='\n')afterNL=false; else afterNL = true;
				break;					
		}
		iter++;
	}
	
	f=newf;
}

bool Format(char *fname)
{
	fstream codefile;
	codefile.open(fname,ios::in|ios::out);
	if(!codefile.is_open())
	{
		codefile.close();
		return false;
	}
	//
	//codefile.seekg(0,ios::end);
	//int len = (int)codefile.tellg();
	//std::cout<<"File Length= " <<len<<std::endl;
	//codefile.seekg(0,ios::beg);
	//char* document = new char[len];
	//
	//codefile.read(document,len);
	//
	//string newdoc = document;
	//std::cout<<"====OriginCode===\n"<<newdoc<<std::endl;
	//delete[] document;
	//ClearFormat(newdoc);
	//std::cout<<"====ClearFormatCode===\n"<<newdoc<<std::endl;
	//Reformat(newdoc);
	//std::cout<<"====ReFormatCode===\n"<<newdoc<<std::endl;
	////std::cout<<newdoc.length()<<std::endl;
	//
	//fstream nfile("nfile.cpp",ios::binary|ios::out);
	//document = const_cast<char*>(newdoc.c_str());
	//nfile.write(document,newdoc.length());
	//codefile.close();
	//nfile.close();

	return true;
}

int main(int argc, char** argv) 
{
	//string mycpp=
	//"// Format.cpp\n//\n#include \"stdafx.h\"\n\nint _tmain(int argc, _TCHAR* argv[])\n{\n	return 0;\n}";
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
	getchar();
	return 0;	
	
}
