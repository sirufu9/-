#include<stdio.h>  
#include<graphics.h>  

void drawShape(){  
	//设置颜色,这是一个函数，要在图形窗口创建后调用  
	setcolor(BLUE);  
	//画一个矩形  
	rectangle(100, 100, 200, 200);  
	//填充颜色  
	floodfill(150, 150, BLUE);  
}  
void drawText() {  
	//设置字体  
	settextstyle(24, 0, _T("黑体"));
	//显示文本  
	outtextxy(100, 50, _T("Hello, Graphics!"));  

}  

int main() {  
	//创建一个图形窗口，宽*高  
	initgraph(648, 648,EX_SHOWCONSOLE);  
	//设置颜色  
	setbkcolor(WHITE);  
	//用该颜色填充  
	cleardevice();  

	//调用绘图函数  
	drawShape();  
	drawText();  

	//防止程序退出  
	getchar();  
	closegraph();  
	return 0;  
}