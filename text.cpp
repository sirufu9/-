#include<stdio.h>  
#include<graphics.h>  

void drawShape(){  
	//������ɫ,����һ��������Ҫ��ͼ�δ��ڴ��������  
	setcolor(BLUE);  
	//��һ������  
	rectangle(100, 100, 200, 200);  
	//�����ɫ  
	floodfill(150, 150, BLUE);  
}  
void drawText() {  
	//��������  
	settextstyle(24, 0, _T("����"));
	//��ʾ�ı�  
	outtextxy(100, 50, _T("Hello, Graphics!"));  

}  

int main() {  
	//����һ��ͼ�δ��ڣ���*��  
	initgraph(648, 648,EX_SHOWCONSOLE);  
	//������ɫ  
	setbkcolor(WHITE);  
	//�ø���ɫ���  
	cleardevice();  

	//���û�ͼ����  
	drawShape();  
	drawText();  

	//��ֹ�����˳�  
	getchar();  
	closegraph();  
	return 0;  
}