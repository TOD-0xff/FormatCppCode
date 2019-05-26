#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
using std::string;
using std::fstream;
using std::ios;

//����ַ��������л��з��Լ�����ո� 
void ClearFormat(string& f)
{
	bool neword = false;
	bool quote = false;				//����ģʽ 
	unsigned char commentMode = 0;	//λģʽ
	//λ���ر�ʶ��
	/*  0x80 �������ע��ģʽ,����ע�� 
		0x40 �������/����
		0x20 ����*���� 
		0x81 �������ע��
	*/ 
	
	string newf="";
	string::iterator iter = f.begin();
	char c = '\0';
	while(iter!=f.end())
	{
		c = *iter;
		
		if(c=='/')
		{
			if(commentMode&0x40!=0) commentMode |=0x80; else commentMode |=0x40;
			if(commentMode&0x20!=0) commentMode = 0;
		}
		else if(c=='*')
		{
			if(commentMode&0x81==0x81) commentMode |=0x20;
			if(commentMode&0x40!=0) commentMode |=0x81;		
		}
		
		if(c=='\r')
		{
			iter++;
			continue;
		}
		
		if((c=='\n')&&(commentMode&0x80==0))
		{
			iter++;
			continue;
		}
		else
		{
			//ע��ģʽ�²������ʽ 
			if(commentMode&0x01==0) commentMode = 0;
			newf.append(1,'\n');
			iter++;
			continue;	
		}
		
		if(c=='\"')
		{
			quote = !quote;
		}
		
		if((!quote)&&(commentMode&0x80==0))
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
				neword=false;
				newf.append(1,c);
			}	
		}
		else
		{	//�������û���ע���򲻸�ʽ�� 
			newf.append(1,c);	
		}
		iter++;
	}
	f = newf;
}
//���� 
void NewLine(string &f)
{
	f.append(1,'\r');
	f.append(1,'\n');
}
//���� 
void Tab(string& f,int tabs)
{
	const int space = 2;
	if(tabs>0) f.append(tabs*space,' ');
}

void Reformat(string& f)
{
	string newf;
	int tabs=0;
	
	bool afterNL = false;		//��ʾ�Ƿ��Ѿ����� 
	bool paramMode = false;		//�ж��Ƿ�Ϊ����ģʽ����������״̬�� 
	bool preProcMode = false;  //�ж��Ƿ�ΪԤ����ģʽ
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
		else if(c=='#' && !preProcMode)
		{
			preProcMode = true; 
		}
		else if(c=='"')
		{
			quoteMode =!quoteMode;
		}
		
		if(quoteMode||paramMode)
		{
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
				NewLine(newf);
				Tab(newf,0);
				newf.append(1,'#');
				break;
			case '>':
				if(preProcMode)
				{
					NewLine(newf);
					preProcMode = false;
				} 
				newf.append(1,'>');
				break;
			default:
				if(afterNL) Tab(newf,tabs);
				newf.append(1,c);
				afterNL=false;
				break;					
		}
		iter++;
	}
	
	f=newf;
}

bool Format(char *fname)
{
	fstream codefile;
	codefile.open(fname,ios::binary|ios::in|ios::out);
	if(!codefile.is_open())
	{
		codefile.close();
		return false;
	}
	
	codefile.seekg(0,ios::end);
	int len = (int)codefile.tellg();
	std::cout<<"File Length= " <<len<<std::endl;
	codefile.seekg(0,ios::beg);
	char* document = new char[len];
	
	codefile.read(document,len);
	
	string newdoc = document;
	delete[] document;
	ClearFormat(newdoc);
		std::cout<<newdoc<<std::endl;
	Reformat(newdoc);
	std::cout<<newdoc.length()<<std::endl;
	
	fstream nfile("e:\\nfile.cpp",ios::binary|ios::in);
	document = const_cast<char*>(newdoc.c_str());
	nfile.write(document,newdoc.length());
	codefile.close();
	nfile.close();
	return true;
}

int main(int argc, char** argv) 
{
	string mycpp="int main(){helloworld;}      void newline(){coutMM;}";
	ClearFormat(mycpp);
	std::cout<<mycpp;
	
	return 0;
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
