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

//换行 
void NewLine(string &f)
{
	f.append(1,'\n');
}
//缩进 
void Tab(string& f,int tabs)
{
	const int space = 2;
	if(tabs>0) f.append(tabs*space,' ');
}

//清除字符串中所有换行符以及多余空格 
void ClearFormat(string& f)
{
	bool neword = false;			//新单词起始标志
	bool quote = false;				//引用模式 
	bool precode = false;			//预处理模式
	unsigned char commentMode = 0;	//位模式

	//位开关标识：
	/*  0x80 代表单行注释 
		0x40 刚出现过/符号
		0x20 刚出现过*符号
		0x10 代表多行注释刚结束(意味着遇到'*''/'组合)
		0x81 代表多行注释
		0x02 代表出现':'，遇到换行符置0
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
		//格式化多余换行符
		if(c=='\n')
		{
			//预处理、注释、引用中的换行符保留
			if((commentMode&0x80)!=0)
			{
				NewLine(newf);
				//若为单行注释模式，则遇到换行符认为注释结束
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
				//多行注释刚结束则保留换行符
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
		//非注释或引用模式下出现'#'则意味着预处理语句，不考虑'##'的情形
		if(c=='#' &&(!quote) && (!(commentMode&0x80)) ) precode =true;

		if((!quote)&&(!(commentMode&0x80)))
		{
			//当不是引用模式或注释模式时格式化
			//格式化多余空格
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
				//指针或地址引用符号后空格保留
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
			//识别注释标志
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
		{	//若是引用或者注释则不格式化
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
	
	bool afterNL = false;		//标示是否已经换行
	bool backslash = false;		//反斜杠最近出现

	unsigned char mode = 0;
	unsigned char ct = 0;		//圆括号叠加次数，用于判断是否处于括号状态
	/*
	位模式：
	0x80 单行注释
	0x90 多行注释
	0x40 上一循环出现'/'
	0x20 上一循环出现'*'
	0x08 括号状态
	0x04 引号状态（默认双引号）
	0x02 预处理状态
	0x05 单引号模式
	其中注释状态0x80，括号状态0x08，引号状态0x04，预处理0x02 生效时位设为1
	括号状态下引号状态可以生效，反之不成立
	其余各状态互斥
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
				//若当前为单行注释模式，遇换行符则认为注释结束
				if((mode&(0x80|0x90))==0x80) mode&=(~0x80);
			}
			else if(!(mode&(0x80|0x08|0x04)))
			{
				mode &=(~0x02);		//预处理模式下遇到换行则认为预处理结束
			}
		}

		backslash =((c=='\\')?true:false);

		if(c!='*' && c!='/')
		{
			mode &=(~(0x40|0x20));
		}
		//当处于注释、括号、引号、预处理任一模式时则不进行格式化
		if(mode&(0x80|0x08|0x04|0x02))
		{
			newf.append(1,c);
			afterNL=( (c=='\n')?true:false );
			iter++;
			continue;
		} 
		//格式化识别
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
					//若已经换过行再次出现换行符则直接忽略该操作
					//换行后缩进
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
	//打开待排版文件
	fstream codefile;
	codefile.open(fname,ios::in|ios::out);
	if(!codefile.is_open())
	{
		codefile.close();
		return false;
	}
	//得到文件大小
	codefile.seekg(0,ios::end);
	int len = (int)codefile.tellg();
	//读取文件到c_string
	codefile.seekg(0);
	char* mydoc = new char[len];
	codefile.get(mydoc,len,EOF);
	codefile.close();
	//排版
	string document = mydoc;
	delete[] mydoc;
	ClearFormat(document);
	Reformat(document);
	//排版后代码存入新文件中
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
	system("pause");
	return 0;	
	
}
