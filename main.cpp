#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
using std::string;
using std::fstream;
using std::ios;

//清除字符串中所有换行符以及多余空格 
void ClearFormat(string& f)
{
	bool neword = false;
	bool quote = false;				//引用模式 
	unsigned char commentMode = 0;	//位模式
	//位开关标识：
	/*  0x80 代表进入注释模式,单行注释 
		0x40 代表出现/符号
		0x20 出现*符号 
		0x81 代表多行注释
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
			//注释模式下不清除格式 
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
		{	//若是引用或者注释则不格式化 
			newf.append(1,c);	
		}
		iter++;
	}
	f = newf;
}
//换行 
void NewLine(string &f)
{
	f.append(1,'\r');
	f.append(1,'\n');
}
//缩进 
void Tab(string& f,int tabs)
{
	const int space = 2;
	if(tabs>0) f.append(tabs*space,' ');
}

void Reformat(string& f)
{
	string newf;
	int tabs=0;
	
	bool afterNL = false;		//标示是否已经换行 
	bool paramMode = false;		//判断是否为括号模式，即语句参数状态下 
	bool preProcMode = false;  //判断是否为预处理模式
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
		std::cout<<"参数有误！\n";
	}
	else
	{
		if(Format(argv[1])==false)
		{
			std::cout<<"无法打开排版文件！\n"<<"File::\\\\ "<<argv[1]; 
		}
		else
		{
			std::cout<<"排版完成，请在相关目录下查看!\n";
		}
	}
	getchar();
	return 0;	
	
}
