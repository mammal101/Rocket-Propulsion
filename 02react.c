#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <math.h>


//=���������� � ������� ���� � ��������========================
LRESULT CALLBACK WindowFunc(HWND,UINT,WPARAM,LPARAM);
DWORD WINAPI ThreadProc (LPVOID lpParameter);
// ������ ��������� ����� ������
BOOL CALLBACK DialogPlanet(HWND hwnd,UINT message, WPARAM wParam,LPARAM lParam);
// ������ ��������� ������
BOOL CALLBACK DialogRocket(HWND hwnd,UINT message, WPARAM wParam,LPARAM lParam);
char szWinName[] = "���������� ��������";
HWND hwnd; // ���������� ����
HDC memdc;
PAINTSTRUCT paintstruct;
HINSTANCE ThisInst;	
//=��������� �������============================================  
double	S_planet_mass, // ����� �������
		S_planet_radius, // ������ �������
		Spmm, // ����� ������� ��� ���������� �� 10 � ������� Spmp
		Spmp, // ������� 10 � ����� �������
		S_rocket_um, // ����� �������� �������� ������
		S_rocket_fm, // ����� �������
		S_rocket_fv, // �������� ������� �����
		S_rocket_fu, // ������ ������� �� ������
		S_rocket_alim; // ����������� ���������
//=���������� � ������ ������������� ===========================
double h,
	   R,
	   v,
	   c,
	   v2k,	
	   a,
	   g,
	   M,
	   m,
	   dmpodt,
	   maxt,
	   t,
	   dt,
	   G,
	   res;
void set_dmpodt();
double dV(double argt, double argx);
//=�������====================================================== 
int x_to_Gx(double arg);
int y_to_Gy(double arg);
double	max_a_x,
		max_a_y,
		min_a_x,
		min_a_y,
		max_g_x,
		max_g_y,
		min_g_x,
		min_g_y;
void Set_CS_A();
void Set_CS_V();
void Set_CS_H();
void Set_CS_DMPODT();
void Set_A();
double	limA,
		limV,
		limH,
		limT,
		limDMPODT;
HDC hdc;
HBITMAP hbit;
DWORD Tid;
HANDLE hct;
int maxX, maxY;  	
int G_dmpodt_x0;
int	G_dmpodt_y0;
int G_H_x0;
int	G_H_y0;
int G_V_x0;
int	G_V_y0;
int G_A_x0;
int	G_A_y0;
int param_x0;
int param_y0;
int res_x0;
int res_y0;
int v_str_sp;
int tab_sp;
int axes_size;
void draw_line(int x0, int y0, int x1, int y1);
void draw_axes(int x0, int y0);
void init_graph(); // ���������� ��������� �������
void refresh_window(); // ���������� ����
//=��������� ������=============================================  
double	m_radius[10]; // ������
double	m_mass[10][2]; // �����
char*	m_name[11]; // �������� �������
int		m_name_l[11]; // ������ �������� �������
int		m_name_c; // �������
int		m_name_id[11]; // ID ������ � ����
void	InitPlanets(); // ���������� �������� ���������� ������
void Set_Planet(int c);
//==============================================================
int Set_A_coords_plus_first_count();
int fc;
void Model();
//==============================================================
maxX=820; // ������ ����
maxY=600; // ������ ����

// ���������� ��������
G_dmpodt_x0=50; // ������ �������
G_dmpodt_y0=250;

G_H_x0=50; // ������
G_H_y0=500;

G_V_x0=310; // ��������
G_V_y0=500;

G_A_x0=570; // ���������
G_A_y0=500;

param_x0=300; // ���������� ����������
param_y0=30;

res_x0=580; // ���������� �����������
res_y0=30;

v_str_sp=20; // ������������ ���������� ����� ��������
tab_sp=5; // ���������
axes_size=210; // ������ ����

//=��������� �����===================================================
// ����� �� ����� (x0,y0) � ����� (x1,y1)
void draw_line(int x0, int y0, int x1, int y1)
{
	SelectObject(memdc, CreatePen(PS_SOLID,1,RGB(0,0,0)));
	MoveToEx(memdc,x0,y0,0);
	LineTo(memdc,x1,y1);
}
//=��� ��������======================================================
// x0, y0 - ���������� ����� 0
// axes_size - ������ ����

void draw_axes(int x0, int y0)
{
	int i;
	draw_line(x0,y0,x0+axes_size,y0);
	draw_line(x0,y0,x0,y0-axes_size);

	// ������� 0x
	draw_line(x0+axes_size,y0,x0+axes_size-5,y0-3);
	draw_line(x0+axes_size,y0,x0+axes_size-5,y0+3);
	
	// ������� 0y
	draw_line(x0,y0-axes_size,x0-3,y0-axes_size+5);
	draw_line(x0,y0-axes_size,x0+3,y0-axes_size+5);

	for (i=1; i<=10; i++) // �����
	{
		draw_line(x0+i*20,y0-3,x0+i*20,y0+3);
		draw_line(x0-3,y0-i*20,x0+3,y0-i*20);
	}
}
void reset_rocket_parameters() {
    S_rocket_um = 5;      // �������� ��������
    S_rocket_fm = 100;    // ����� �������
    S_rocket_fv = 4;      // �������� ������� �����
    S_rocket_fu = 1.2;    // ������ ������� �� ������
    S_rocket_alim = 50;   // ����������� ���������
}
//=���������� ��������� �������======================================
void init_graph()
{
	char str[80]; // ��������������� ����������
	
	// ������� ������
	SelectObject (memdc, WHITE_BRUSH);	
	Rectangle(memdc, 0,0,maxX,maxY);
	
	// ��� ��������
	draw_axes(G_dmpodt_x0, G_dmpodt_y0); // ������ �������
	draw_axes(G_H_x0, G_H_y0); // ������
	draw_axes(G_V_x0, G_V_y0); // ��������
	draw_axes(G_A_x0, G_A_y0); // ���������

	// ������� � ��������
	SetTextColor(memdc, RGB(0, 0, 0)); // ���� ������ - ������
	TextOut(memdc,G_H_x0, G_H_y0-axes_size-20,"������(��)",10);
	TextOut(memdc,G_V_x0, G_V_y0-axes_size-20,"��������(��/�)",14);
	TextOut(memdc,G_A_x0, G_A_y0-axes_size-20,"���������(�/�^2)",17);
	TextOut(memdc,G_dmpodt_x0, G_dmpodt_y0-axes_size-20,"������ �������(�/�)",19);
		
	
	// ����� ���������
	draw_line(res_x0-10,res_y0-10,res_x0-10,res_y0+220);
	draw_line(res_x0-10,res_y0-10,res_x0+220,res_y0-10);
	draw_line(res_x0+220,res_y0-10,res_x0+220,res_y0+220);
	draw_line(res_x0-10,res_y0+220,res_x0+220,res_y0+220);
	TextOut(memdc,res_x0+75,res_y0-18," ������� ",9);

// ���������
// �����
draw_line(param_x0-10,param_y0-10,param_x0-10,param_y0+220);
draw_line(param_x0-10,param_y0-10,param_x0+260,param_y0-10);
draw_line(param_x0+260,param_y0-10,param_x0+260,param_y0+220);
draw_line(param_x0-10,param_y0+220,param_x0+260,param_y0+220);
TextOut(memdc,param_x0+80,param_y0-18," ��������� ",11);

TextOut(memdc,param_x0,param_y0,"������:",7);



if (S_rocket_um < 0) {
    MessageBox(NULL, "������������ ��������: ����� �������� �������� �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp,"����� �������� ��������(�):",27);
    sprintf(str,"%1.4lf",S_rocket_um);
    TextOut(memdc,param_x0+215,param_y0+v_str_sp,str,5);
}

if (S_rocket_fm < 0) {
    MessageBox(NULL, "������������ ��������: ����� ������� �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*2,"����� �������(�):",17);
    sprintf(str,"%1.4lf",S_rocket_fm);
    TextOut(memdc,param_x0+215,param_y0+v_str_sp*2,str,5);
}

if (S_rocket_fv < 0) {
    MessageBox(NULL, "������������ ��������: �������� ������� ����� �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*3,"�������� ������� �����(��/�):",29);
    sprintf(str,"%1.4lf",S_rocket_fv);
    TextOut(memdc,param_x0+215,param_y0+v_str_sp*3,str,5);
}

if (S_rocket_fu < 0) {
    MessageBox(NULL, "������������ ��������: ������ ������� �� ������ �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*4,"������ ������� �� ������(�/�):",30);
    sprintf(str,"%1.4lf",S_rocket_fu);
    TextOut(memdc,param_x0+215,param_y0+v_str_sp*4,str,5);
}

if (S_rocket_alim < 0) {
    MessageBox(NULL, "������������ ��������: ����������� ��������� �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*5,"����������� ���������(�/�^2):",29);
    sprintf(str,"%1.4lf",S_rocket_alim);
    TextOut(memdc,param_x0+215,param_y0+v_str_sp*5,str,5);
}

TextOut(memdc,param_x0,param_y0+v_str_sp*7,"����� ������:",13);

TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*8,"�������:",8);
TextOut(memdc,param_x0+175,param_y0+v_str_sp*8,m_name[m_name_c],m_name_l[m_name_c]);


if (Spmm < 0) {
    MessageBox(NULL, "������������ ��������: ����� ������� �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*9,"����� �������(��):",18);
    sprintf(str,"%1.4lf",Spmm);
    TextOut(memdc,param_x0+165,param_y0+v_str_sp*9,str,5);
    TextOut(memdc,param_x0+205,param_y0+v_str_sp*9,"*10^",4);
    sprintf(str,"%2.0lf",Spmp);
    TextOut(memdc,param_x0+235,param_y0+v_str_sp*9,str,2);
}

if (S_planet_radius < 0) {
    MessageBox(NULL, "������������ ��������: ������ ������� �� ����� ���� �������������.", "������", MB_OK | MB_ICONERROR);
	reset_rocket_parameters();
} else {
    TextOut(memdc,param_x0+tab_sp,param_y0+v_str_sp*10,"������ �������(��):",19);
    sprintf(str,"%1.4lf",S_planet_radius);
    TextOut(memdc,param_x0+220,param_y0+v_str_sp*10,str,4);
}

refresh_window();

}


//=���������� ����===================================================
void refresh_window()
{
			InvalidateRect(hwnd,NULL,TRUE);
			UpdateWindow(hwnd);
}
//===================================================================

int WINAPI WinMain (HINSTANCE hThisInst, 
  					HINSTANCE hPrevInst,
  					LPSTR lpszArgs,
  					int nWinMode)
  {
  	
  	MSG msg;
  	WNDCLASS wcl;
  	wcl.hInstance=hThisInst;
  	wcl.lpszClassName=szWinName; 
  	wcl.lpfnWndProc=WindowFunc; 
  	wcl.style=0;                
  	wcl.hIcon=LoadIcon(hThisInst ,MAKEINTRESOURCE(ID_KSICON3)); 
    wcl.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcl.lpszMenuName= (LPSTR)ID_MENU_1; 
    wcl.cbClsExtra=0;
    wcl.cbWndExtra=0;
    wcl.hbrBackground=(HBRUSH) GetStockObject(WHITE_BRUSH);
    
	if (!RegisterClass (&wcl)) return 0;
    
    hwnd = CreateWindow(szWinName,"���������� ��������",
    					WS_OVERLAPPEDWINDOW,
	  					50, // X ���������� ����
	  					50, // Y ���������� ����
	  					maxX,// ������ ����
	  					maxY,// ������ ����
	  					HWND_DESKTOP,
	  					NULL,
	  					hThisInst,
	  					NULL);

	// �������� ���� � ���������� ����������
	ShowWindow(hwnd,nWinMode);
	UpdateWindow (hwnd);
	
	// ���� ��������� ���������
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	DeleteDC (memdc);
	return msg.wParam;
  }	                  
//=������� ����� �������=============================================
void Set_Planet(int c)
{
	Spmm = m_mass[c][0];
	Spmp = m_mass[c][1];
	S_planet_mass=Spmm*pow(10,Spmp);			
	S_planet_radius=m_radius[c];
	CheckMenuItem(GetMenu(hwnd),m_name_id[m_name_c],MF_UNCHECKED);
	m_name_c=c;
	CheckMenuItem(GetMenu(hwnd),m_name_id[m_name_c],MF_CHECKED);
	
	init_graph(memdc);
}
//===================================================================
 LRESULT CALLBACK WindowFunc(HWND hwnd,
							 UINT message,
  							 WPARAM wParam,
							 LPARAM lParam)
  {
   	switch (message)
  	{
  		  					     
  		case WM_CREATE:
			
			hdc = GetDC(hwnd);
  			memdc = CreateCompatibleDC(hdc);
			hbit = CreateCompatibleBitmap (hdc,maxX,maxY);
  			SelectObject (memdc, hbit);
			
			PatBlt (memdc, 0,0, maxX,maxY,PATCOPY);
  			ReleaseDC (hwnd, hdc);       
			
			InitPlanets(); // �������� ����������� ���������� ������
			
			// ������� �� ��������� - �����
			m_name_c=2;
			S_planet_radius=m_radius[m_name_c];
			Spmm=m_mass[m_name_c][0]; 
			Spmp=m_mass[m_name_c][1];
			
			S_planet_mass=Spmm*pow(10,Spmp); // ����� �������
			
			// ��������� ������ �� ���������
			S_rocket_um=5; // �������� ��������
			S_rocket_fm=100; // ����� �������
			S_rocket_fv=4; // �������� ������� �����
			S_rocket_fu=1.2; // ������ ������� �� ������
			S_rocket_alim=50; // ����������� ���������

			init_graph(memdc);
			break;
  		
	  	case WM_PAINT:
  			hdc = BeginPaint(hwnd,&paintstruct);
			BitBlt (hdc, 0, 0, maxX,maxY, memdc,0,0,SRCCOPY);	
			EndPaint(hwnd,&paintstruct);
			break;
  		
		case WM_COMMAND:
  			switch (LOWORD(wParam))
  			{
  				
				case ID_ROCKET: // ������� ���� ���������->������
					DialogBox(ThisInst, MAKEINTRESOURCE(ID_D_ROCKET),hwnd, DialogRocket); 
					break;
								
				case ID_PLANET: // ������� ���� ���������->����� ������->������
					DialogBox(ThisInst, MAKEINTRESOURCE(ID_D_PLANET),hwnd, DialogPlanet);
					break;
				
				// ����� ������� � ���������->����� ������
				case ID_MERC: // ��������
					Set_Planet(0);
					break;
				
				case ID_VEN: // ������
					Set_Planet(1);
					break;
				
				case ID_EARTH: // �����
					Set_Planet(2);
					break;
				
				case ID_LUNA: // ����
					Set_Planet(3);
					break;
				
				case ID_MARS: // ����
					Set_Planet(4);
					break;
				
				case ID_JUP: // ������
					Set_Planet(5);
					break;
				
				case ID_SAT: // ������
					Set_Planet(6);
					break;
				
				case ID_URAN: // ����
					Set_Planet(7);
					break;
				
				case ID_NEPT: // ������
					Set_Planet(8);
					break;

				case ID_PLUT: // ������
					Set_Planet(9);
					break;

				case ID_PAUSE: // ������� ���� �����
					SuspendThread(hct); // ��������� ����������
					
					// ��������� ������ �����������
					EnableMenuItem(GetMenu(hwnd),ID_GOON,MF_ENABLED);
					
					// ��������� ������ �����
					EnableMenuItem(GetMenu(hwnd),ID_PAUSE,MF_GRAYED);
					DrawMenuBar(hwnd); // ���������� ����
					break;
				
				case ID_GOON: // ������� ���� ����������
					ResumeThread(hct); // ����������� ����������
					
					// ���������� ������ �����������
					EnableMenuItem(GetMenu(hwnd),ID_GOON,MF_GRAYED);
					
					// ��������� ������ �����
					EnableMenuItem(GetMenu(hwnd),ID_PAUSE,MF_ENABLED);
					DrawMenuBar(hwnd); // ���������� ����
					break;

				case ID_GO: // ������ ����
					init_graph(memdc);
					TextOut(memdc,res_x0,res_y0,"���� ��������������� �������",28);
					
					InvalidateRect(hwnd,NULL,TRUE);
					UpdateWindow(hwnd);
					
					EnableMenuItem(GetMenu(hwnd),ID_PAUSE,MF_ENABLED);
					EnableMenuItem(GetMenu(hwnd),ID_RELOAD,MF_ENABLED);
					EnableMenuItem(GetMenu(hwnd),ID_PLANET,MF_GRAYED);
					EnableMenuItem(GetMenu(hwnd),ID_ROCKET,MF_GRAYED);
					EnableMenuItem(GetMenu(hwnd),ID_GO,MF_GRAYED); // ���������� ������ "����"
					DrawMenuBar(hwnd);

					fc=Set_A_coords_plus_first_count();
					TextOut(memdc,res_x0,res_y0+v_str_sp,"���������",9);
					
					if (fc==2) 
					{
						TextOut(memdc,res_x0,res_y0,"������ �� ����� ����������",26);
						TextOut(memdc,res_x0,res_y0+v_str_sp,"�� ����������� �������!",23);
					}
					refresh_window(hwnd);
					
					if (Set_A_coords_plus_first_count()<2)
					{
							hct = CreateThread(NULL,0,ThreadProc,NULL,0,&Tid);
					}
					break;

				case ID_RELOAD: // ������� ���� �����
					
					EnableMenuItem(GetMenu(hwnd),ID_GO,MF_ENABLED);
					EnableMenuItem(GetMenu(hwnd),ID_PLANET,MF_ENABLED);
					EnableMenuItem(GetMenu(hwnd),ID_ROCKET,MF_ENABLED);
					EnableMenuItem(GetMenu(hwnd),ID_GOON,MF_GRAYED);
					EnableMenuItem(GetMenu(hwnd),ID_PAUSE,MF_GRAYED);
					EnableMenuItem(GetMenu(hwnd),ID_RELOAD,MF_GRAYED);
					DrawMenuBar(hwnd);

					TerminateThread(hct,0);
					
					init_graph(memdc);

					break;

				case ID_EX:
  				    if (MessageBox(hwnd, "�� ������������� ������ �����?",
  					"�����",MB_YESNO | MB_ICONQUESTION) == IDYES)
  				    	DestroyWindow(hwnd);
 					break;
  			}  
  			break;
  		            		
		case WM_DESTROY: // ���������� ���������
			PostQuitMessage(0);
  			break;
  		
  		default:
  			return DefWindowProc (hwnd, message, wParam, lParam);
  	}
  	return 0;
  }							
//=������ ��������� ������==================================================  	
BOOL CALLBACK DialogRocket(HWND hDlg,UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	char str[80];
	switch (uMsg)
		{
		case (WM_INITDIALOG): // ���������� ����� �����
			sprintf (str,"%1.4lf",S_rocket_um);
			SetDlgItemText(hDlg,IDC_EDIT_UM,str);
			sprintf (str,"%1.4lf",S_rocket_fm);
			SetDlgItemText(hDlg,IDC_EDIT_FM,str);
			sprintf (str,"%1.4lf",S_rocket_fv);
			SetDlgItemText(hDlg,IDC_EDIT_FV,str);
			sprintf (str,"%1.4lf",S_rocket_fu);
			SetDlgItemText(hDlg,IDC_EDIT_FU,str);
			sprintf (str,"%1.4lf",S_rocket_alim);
			SetDlgItemText(hDlg,IDC_EDIT_ALIM,str);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD (wParam)) 
		{
		case ID2_OK: // ���� �� ��������� ����������
			GetDlgItemText(hDlg,IDC_EDIT_UM,str,80);
			S_rocket_um = atof(str);
			GetDlgItemText(hDlg,IDC_EDIT_FM,str,80);
			S_rocket_fm = atof(str);			
			GetDlgItemText(hDlg,IDC_EDIT_FV,str,80);
			S_rocket_fv = atof(str);
			GetDlgItemText(hDlg,IDC_EDIT_FU,str,80);
			S_rocket_fu = atof(str);
			GetDlgItemText(hDlg,IDC_EDIT_ALIM,str,80);
			S_rocket_alim = atof(str);
			EndDialog(hDlg, TRUE);
			init_graph(memdc);
		return TRUE;

		case ID2_CANCEL: // ������ - ������ �� ������
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
	}
	return FALSE;
}
//=������ ��������� ����� ������============================================
BOOL CALLBACK DialogPlanet(HWND hDlg,UINT uMsg, WPARAM wParam,LPARAM lParam)
{
	char str[80];
	switch (uMsg)
	{
		case (WM_INITDIALOG): // ���������� ����� �����
		sprintf (str,"%1.3lf",Spmm);
		SetDlgItemText(hDlg,IDC_EDIT_MP_M,str);
		sprintf (str,"%2.0lf",Spmp);
		SetDlgItemText(hDlg,IDC_EDIT_MP_P,str);
		sprintf (str,"%5.0lf",S_planet_radius);
		SetDlgItemText(hDlg,IDC_EDIT_RP,str);
		
		return TRUE;

		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
			case ID1_OK: // ���� �� ��������� ����������		  		  
				GetDlgItemText(hDlg,IDC_EDIT_MP_M,str,80);
				Spmm = atof(str);
				GetDlgItemText(hDlg,IDC_EDIT_MP_P,str,80);
				Spmp = atof(str);
				S_planet_mass=Spmm*pow(10,Spmp);			
				GetDlgItemText(hDlg,IDC_EDIT_RP,str,80);
				S_planet_radius=atof(str);			
				EndDialog(hDlg, TRUE);
				CheckMenuItem(GetMenu(hwnd),m_name_id[m_name_c],MF_UNCHECKED);
				m_name_c=10;
				CheckMenuItem(GetMenu(hwnd),m_name_id[m_name_c],MF_CHECKED);
				init_graph(memdc);
			return TRUE;

			case ID1_CANCEL: // ������ - ������ �� ������
				EndDialog(hDlg, TRUE);
				return TRUE;
		}
	}
	return FALSE;
}
//==============================================================

//=��������� ������=============================================
void InitPlanets()
{
	m_name[0]="��������";
	m_name_l[0]=8;
	m_name_id[0]=ID_MERC;
	m_radius[0]=2440;
	m_mass[0][0]=3.3;
	m_mass[0][1]= 23;

	m_name[1]="������";
	m_name_l[1]=6;
	m_name_id[1]=ID_VEN;
	m_radius[1]=6050;
	m_mass[1][0]=4.9;
	m_mass[1][1]= 24;
	
	m_name[2]="�����";
	m_name_l[2]=5;
	m_name_id[2]=ID_EARTH;
	m_radius[2]=6370;
	m_mass[2][0]=6.0;
	m_mass[2][1]=24;//�����

	m_name[3]="����";
	m_name_l[3]=4;
	m_name_id[3]=ID_LUNA;
	m_radius[3]=1740;
	m_mass[3][0]=7.4;
	m_mass[3][1]=22;//����

	m_name[4]="����";
	m_name_l[4]=4;
	m_name_id[4]=ID_MARS;
	m_radius[4]=3390;
	m_mass[4][0]=6.4;
	m_mass[4][1]=23;//����
	
	m_name[5]="������";
	m_name_l[5]=6;
	m_name_id[5]=ID_JUP;
	m_radius[5]=71400;
	m_mass[5][0]=1.9;
	m_mass[5][1]=27;//������
	
	m_name[6]="������";
	m_name_l[6]=6;
	m_name_id[6]=ID_SAT;
	m_radius[6]=60330;
	m_mass[6][0]=3.7;
	m_mass[6][1]=26;//������
	
	m_name[7]="����";
	m_name_l[7]=4;
	m_name_id[7]=ID_URAN;
	m_radius[7]=26700;
	m_mass[7][0]=8.1;
	m_mass[7][1]=25;//����
	
	m_name[8]="������";
	m_name_l[8]=6;
	m_name_id[8]=ID_NEPT;
	m_radius[8]=25000;
	m_mass[8][0]=1.0;
	m_mass[8][1]=26;//������
	
	m_name[9]="������";
	m_name_l[9]=6;
	m_name_id[9]=ID_PLUT;
	m_radius[9]=1270;
	m_mass[9][0]=1.4;
	m_mass[9][1]=22;//������
	
	m_name[10]="����������";
	m_name_l[10]=10;
	m_name_id[10]=ID_PLANET;
}
//==============================================================  	
int x_to_Gx(double arg)
{
	return min_g_x+((max_g_x-min_g_x)/(max_a_x-min_a_x))*(arg-min_a_x);
}

int y_to_Gy(double arg)
{

	return max_g_y-((max_g_y-min_g_y)/(max_a_y-min_a_y))*(arg-min_a_y);
}
//==============================================================  	
void Set_A()
{
	min_a_x=0;
	min_a_y=0;
	max_a_x=limT;
}

void Set_CS_V() // ��������
{
	min_g_x=G_V_x0;
	min_g_y=G_V_y0-axes_size+10;
	max_g_x=G_V_x0+axes_size-10;
	max_g_y=G_V_y0;
	Set_A();
	max_a_y=limV;
}
void Set_CS_H() // ������
{
	min_g_x=G_H_x0;
	min_g_y=G_H_y0-axes_size+10;
	max_g_x=G_H_x0+axes_size-10;
	max_g_y=G_H_y0;
	Set_A();
	max_a_y=limH;
}

void Set_CS_A() // ���������
{
	min_g_x=G_A_x0;
	min_g_y=G_A_y0-axes_size+10;
	max_g_x=G_A_x0+axes_size-10;
	max_g_y=G_A_y0;
	Set_A();
	max_a_y=limA;
}

void Set_CS_DMPODT() // ������ �������
{
	min_g_x=G_dmpodt_x0;
	min_g_y=G_dmpodt_y0-axes_size+10;
	max_g_x=G_dmpodt_x0+axes_size-10;
	max_g_y=G_dmpodt_y0;
	Set_A();
	max_a_y=limDMPODT;
}
//==============================================================  	
void set_dmpodt()
{
int t=1;
	while (t==1)
	{
		dmpodt=dmpodt-0.01;
		if (dmpodt*c/m-g <= S_rocket_alim) t=2;
	}
}
//=����������� ����������=======================================
void standart_calc() 
{
	m=(S_rocket_um + S_rocket_fm)*1000.0; // ����� ����� ������
	dmpodt=S_rocket_fu*1000.0; // ������ ������� �� ������
	c=S_rocket_fv*1000.0; // �������� ������� �����
	t=0; // �����
	v=0; // ��������
	h=0; // ������
	M=S_planet_mass; // ����� �������
	R=S_planet_radius*1000.0; // ������ �������
	G=6.672*pow(10.0,-11); // �������������� ����������
	g=G*M/R/R; // ��������� ���������� �������
}
//=��������������� �������======================================
int Set_A_coords_plus_first_count()
{
	standart_calc();
	dt=0.001;
	limDMPODT=dmpodt;
	if (dmpodt*c/m < g) return 2; // ������ �� �������� �� ����� (a<g)
	else
		{
		while (m>S_rocket_um*1000) // ���� ����� ������ ������ �������� �����
			{	
			a =	dmpodt*c/m-g; // ���������
			if (a > S_rocket_alim) 
			{
				set_dmpodt();
				a =	dmpodt*c/m-g;
			}
			h+=dt*v; // ������
			v+=dt*a; // ��������
			g =G*M/(R+h)/(R+h); // ��������� ���������� �������
			m-=dmpodt*dt; // ����� ������
			v2k=sqrt(2*M*G/(R+h)); // ������ ����������� ��������
			t+=dt; // �����
		}
		
		// ����� ������� �� �������� �� ������� ����
		limT=t; 
		limH=h;
		limA=a;
		if (v>=sqrt(2*M*G/R)) limV=v;
		else limV=sqrt(2*M*G/R);
		
		return 1;
		}
}
//==============================================================
void Model()
{
	double kx1,kx2,kx3,kx4,
		   kv1,kv2,kv3,kv4;
	int i;
	char temp[80];
	RECT rec1;
	standart_calc();
	dt=limT*0.00005;
	
	i=0;

	TextOut(memdc,res_x0,res_y0+v_str_sp*3,"�����(�):",9);
	TextOut(memdc,res_x0,res_y0+v_str_sp*4,"������(��):",11);
	TextOut(memdc,res_x0,res_y0+v_str_sp*5,"��������(��/c):",15);
	TextOut(memdc,res_x0,res_y0+v_str_sp*6,"���������(��/c*c):",17);
	TextOut(memdc,res_x0,res_y0+v_str_sp*7,"�������(�):",11);
	TextOut(memdc,res_x0,res_y0+v_str_sp*8,"g(�/c*c):",9);
	TextOut(memdc,res_x0,res_y0+v_str_sp*9,"������ �����������",18);
	TextOut(memdc,res_x0,res_y0+v_str_sp*10,"��������(��/�):",15);


	for (i=1;i<=10;i++)
	{
	
		sprintf (temp,"%lf",limDMPODT*0.001/10*(11-i));
		TextOut(memdc,G_dmpodt_x0-45,G_dmpodt_y0-axes_size+2+(i-1)*20,temp,5);
		
		sprintf (temp,"%lf",limH*0.001/10*(11-i));
		TextOut(memdc,G_H_x0-45,G_H_y0-axes_size+2+(i-1)*20,temp,5);
		
		sprintf (temp,"%lf",limV*0.001/10*(11-i));
		TextOut(memdc,G_V_x0-45,G_V_y0-axes_size+2+(i-1)*20,temp,5);

		sprintf (temp,"%lf",limA/10*(11-i));
		TextOut(memdc,G_A_x0-45,G_A_y0-axes_size+2+(i-1)*20,temp,5);
	}
	
	sprintf (temp,"%lf",limT);
	TextOut(memdc,G_A_x0+axes_size-30,G_A_y0+5,temp,5);
	TextOut(memdc,G_A_x0+axes_size+10,G_A_y0+5,"�",1);
	
	TextOut(memdc,G_H_x0+axes_size-30,G_H_y0+5,temp,5);
	TextOut(memdc,G_H_x0+axes_size+10,G_H_y0+5,"�",1);
	
	TextOut(memdc,G_V_x0+axes_size-30,G_V_y0+5,temp,5);
	TextOut(memdc,G_V_x0+axes_size+10,G_V_y0+5,"�",1);
	
	TextOut(memdc,G_dmpodt_x0+axes_size-30,G_dmpodt_y0+5,temp,5);
	TextOut(memdc,G_dmpodt_x0+axes_size+10,G_dmpodt_y0+5,"�",1);
	
	
	refresh_window(hwnd);
	
	
	for (t;m>S_rocket_um*1000;t+=dt)
		{	
		a =	dmpodt*c/m-g;
		if (a > S_rocket_alim) 
			{
			set_dmpodt();
			a =	dmpodt*c/m-g;
			}
		kx1=(v)			   *dt;
		kv1=dV(0,h)        *dt;
		kx2=(v+kv1/2)	   *dt;
		kv2=dV(0.5,h+kx1/2)*dt;
		kx3=(v+kv2/2)	   *dt;
		kv3=dV(0.5,h+kx2/2)*dt;
		kx4=(v+kv3)		   *dt;
		kv4=dV(1,h+kx3)	   *dt;
		
		h+=(kx1+2*kx2+2*kx3+kx4)/6;
		v+=(kv1+2*kv2+2*kv3+kv4)/6;
		g =G*M/(R+h)/(R+h);
		m-=dmpodt*dt;
		v2k=sqrt(2*M*G/(R+h));
		
		Set_CS_H();
		SetPixel(memdc,x_to_Gx(t),y_to_Gy(h),RGB(0,0,255));
		rec1.top=y_to_Gy(h)-1;
		rec1.left=x_to_Gx(t)-1;
		rec1.right=x_to_Gx(t)+1;
		rec1.bottom=y_to_Gy(h)+1;
		InvalidateRect(hwnd,&rec1,TRUE);
		UpdateWindow(hwnd);
		
		Set_CS_V();
		SetPixel(memdc,x_to_Gx(t),y_to_Gy(v),RGB(0,0,255));
		rec1.top=y_to_Gy(v)-1;
		rec1.left=x_to_Gx(t)-1;
		rec1.right=x_to_Gx(t)+1;
		rec1.bottom=y_to_Gy(v)+1;
		InvalidateRect(hwnd,&rec1,TRUE);
		UpdateWindow(hwnd);
		
		SetPixel(memdc,x_to_Gx(t),y_to_Gy(v2k),RGB(255,0,0));
		rec1.top=y_to_Gy(v2k)-1;
		rec1.left=x_to_Gx(t)-1;
		rec1.right=x_to_Gx(t)+1;
		rec1.bottom=y_to_Gy(v2k)+1;
		InvalidateRect(hwnd,&rec1,TRUE);
		UpdateWindow(hwnd);
		
		Set_CS_A();
		SetPixel(memdc,x_to_Gx(t),y_to_Gy(a),RGB(0,0,255));
		rec1.top=y_to_Gy(a)-1;
		rec1.left=x_to_Gx(t)-1;
		rec1.right=x_to_Gx(t)+1;
		rec1.bottom=y_to_Gy(a)+1;
		InvalidateRect(hwnd,&rec1,TRUE);
		UpdateWindow(hwnd);
		
		Set_CS_DMPODT();
		SetPixel(memdc,x_to_Gx(t),y_to_Gy(dmpodt),RGB(0,0,255));
		rec1.top=y_to_Gy(dmpodt)-1;
		rec1.left=x_to_Gx(t)-1;
		rec1.right=x_to_Gx(t)+1;
		rec1.bottom=y_to_Gy(dmpodt)+1;
		InvalidateRect(hwnd,&rec1,TRUE);
		UpdateWindow(hwnd);
		
		if (t>i*dt)
			{
			i+=20;
			sprintf (temp,"%lf",t);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*3,temp,6);
			sprintf (temp,"%lf",h*0.001);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*4,temp,6);
			sprintf (temp,"%lf",v*0.001);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*5,temp,6);
			sprintf (temp,"%lf",a*0.001);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*6,temp,6);
			sprintf (temp,"%lf",m*0.001-S_rocket_um);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*7,temp,6);
			sprintf (temp,"%lf",g);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*8,temp,6);
			sprintf (temp,"%lf",sqrt(2*M*G/(R+h))*0.001);
			TextOut(memdc,res_x0+170,res_y0+v_str_sp*10,temp,6);
			
			rec1.top=res_y0+v_str_sp*3;
			rec1.left=res_x0+170;
			rec1.right=res_x0+215;
			rec1.bottom=res_y0+v_str_sp*11;
			InvalidateRect(hwnd,&rec1,TRUE);
			UpdateWindow(hwnd);
			}
		}
	if (v>=v2k) res=1; // ������ ������� ������ ����������� ��������
	else		res=0; // ������ �� ������� ������ ����������� ��������
}
//==============================================================
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{

Model();

if (res==1) MessageBox(hwnd, "������ ������� ������ ����������� ��������","���������",MB_OK | MB_ICONINFORMATION);
else MessageBox(hwnd, "������ �� ������� ������ ����������� ��������","���������",MB_OK | MB_ICONINFORMATION);

EnableMenuItem(GetMenu(hwnd),ID_ROCKET,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_PLANET,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_GO,MF_ENABLED);
EnableMenuItem(GetMenu(hwnd),ID_PAUSE,MF_GRAYED);
DrawMenuBar(hwnd);

return 0;
}
//==============================================================
double dV(double argt, double argx)
{
return (dmpodt*c/(m-dmpodt*argt)-G*M/(R+argx)/(R+argx));
}
//==============================================================