// cgitiles.cpp : �������̨Ӧ�ó������ڵ㡣
// ͨ��cpp��CGI���������Ƭ����

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "cgi-lib.h"

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif // WIN32


using namespace std;

//0,1...,9,-   3x7x11
int DigitTemplate[] = { 1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,1,  //48
						0,0,1,0,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1, //49
						1,1,1,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,1,1,1,
						1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,
						1,0,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,1,0,0,1,
						1,1,1,1,0,0,1,0,0,1,1,1,0,0,1,0,0,1,1,1,1,
						1,1,1,1,0,0,1,0,0,1,1,1,1,0,1,1,0,1,1,1,1,
						1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,
						1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,
						1,1,1,1,0,1,1,0,1,1,1,1,0,0,1,0,0,1,1,1,1,
						0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0} ;

///Burn digit in image data.
/// x0 - upperleft corner in image
/// y0 - upperleft corner in image
/// weight - line weight in pixel at least 1
/// imgWid - image width
/// imgHei - image height
/// imgData - data array
/// digiStr - digit char array
/// backVal - background value
/// frontVal - foreground value
void drawdigits(int x0,int y0,int weight,int imgWid,int imgHei,short* imgData,const char* digiStr ,int backVal, int frontVal)
{
	weight = max(1,weight) ;
	//every digit is 3x7
	int num = strlen(digiStr) ;
	for(int ic = 0 ; ic < num ; ++ ic )
	{
		int xstart = x0 + ic * 5 * weight ;
		int ystart = y0 ;
		int temIndex = 10 ;
		int digiValue = digiStr[ic] ;
		if( digiValue >= 48 && digiValue <=57 ){
			temIndex = digiValue - 48 ;
		}

		//background 5x9
		for(int iy = ystart ; iy < min(imgHei,ystart+weight*9) ; ++ iy )
		{
			for(int ix = xstart ; ix < min(imgWid,xstart+weight*5); ++ ix )
			{
				int temix = (ix - xstart)/weight -1;
				int temiy = (iy - ystart)/weight -1;
				if( temix == -1 || temiy==-1 || temix==3 || temiy==7 )
				{//background
					imgData[ix + iy * imgWid] = (short)backVal ;
				}else if( DigitTemplate[temIndex*21+temix+temiy*3] == 1 )
				{//foreground
					imgData[ix+iy*imgWid] = (short)frontVal ;
				}else
				{//back
					imgData[ix+iy*imgWid] = (short)backVal ;
				}
			}
		}
	}
}

void test() ;

int main(int argc, char* argv[])
{
	//init digit template 3*7*11 = 231
	//test() ;

	LIST *head = cgi_input_parse();
	if( head ){
		char* xval =  find_val(head,"x") ;
		char* yval =  find_val(head,"y") ;
		char* zval =  find_val(head,"z") ;
		char* v0val = find_val(head,"v0") ;
		char* v1val = find_val(head,"v1") ;

		int v0i = 0 ;
		int v1i = 255 ;
		if( v0val && v1val ){
			//�����Сֵ
			v0i = atof(v0val) ;
			v1i = atof(v1val) ;
		}

		if( xval && yval && zval )
		{
			cout << "Content-type:application/octet-stream; charset=x-user-defined\nContent-Length:131072\nAccept-Ranges: bytes\r\n\r\n";
			_setmode( _fileno( stdout ), _O_BINARY );
			float dpx = (v1i - v0i ) / 255.f  ;
			
			int itilex = atof(xval) ;
			int itiley = atof(yval) ;
			int itilez = atof(zval) ;
			const int imgSize = 256*256 ;
			short* tdata = new short[imgSize] ;
			for(int iy = 0 ; iy < 256 ; ++ iy ){
				int pxval = v0i + iy * dpx ;
				for(int ix = 0 ; ix < 256 ; ++ ix )
				{
					tdata[ix+iy*256]= (short) pxval ; // 
					//�ҵ����������ˣ�MSVC2010��fwrite���������ݵ�stdout�У�stdout��Ѷ��������ݵ����ı������
					//��0A 00 Ϊ������ֵ10����0A��ΪASCII��������LF���з�����˼��winϵͳ��stdout����������з���
					//�Զ�����0D Ҳ����CR�س�������ֵ���Ϊ������win������ô���ģ��������������������ƾ�������������ֽڣ�
					//��Ϊ2�ֽڽ����ʱ��Ҳ�������󡣽������������stdout�����ʱ��Ҫ���ı�����ʽ������Զ�������ʽ�����
				}
			}
			
			drawdigits(0,0    ,3,256,256,tdata,xval,v0i,v1i) ;
			drawdigits(0,27   ,3,256,256,tdata,yval,v0i,v1i) ;
			drawdigits(0,27+27,3,256,256,tdata,zval,v0i,v1i) ;
			
			fwrite( tdata , 2, imgSize, stdout);
			
 
			delete[] tdata ;
		}
	}
	return 0;
}

//test function
void test(){
	int v0i = 0 ;
	int v1i = 2000 ;

	short* tdata = new short[256*256] ;
	memset(tdata,0,256*256*2) ;
	drawdigits(0,0,3,256,256,tdata,"123999",v0i,v1i) ;
	drawdigits(0,27,3,256,256,tdata,"a56",v0i,v1i) ;
	drawdigits(0,27+27,3,256,256,tdata,"7",v0i,v1i) ;
	FILE* fp = fopen("testfunction","wb") ;
	fwrite(tdata,2,256*256,fp) ;
	fclose(fp) ;
	delete[] tdata ;
}