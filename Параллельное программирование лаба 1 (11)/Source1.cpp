#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>

#define MPI_TAG 0


using namespace std;
void make_matrix(int *mat, int hei, int wid)
{
	cout << "generated matrix is" << endl << endl;

	for (int i = 0;i<hei*wid;i++)
	{
		mat[i] = rand() % 10;
		cout << mat[i] << "  ";
		if (i%wid == wid - 1)
			cout << endl;
	}
}
void sumsumsum(int *mat, int *resmat, int hei, int wid)  //
{
	
	for (int i = 0; i < hei; i++) //���� �� ����������� �������� �������
	{
		int sum = 0;
		for (int j = i*wid; j < (i + 1)*wid; j++)
		{
			sum += mat[j]; //��������� �������� ������				
		}
		
		resmat[i] = sum;
	}
}
void let_me_see(int *resmat, int height) 
{   
	int r = 1;
	for (int i = 0; i < height; i++)
	{
		cout << r << "str: " << resmat[i] << endl; //������� �����
		r++;
	}
	cout << endl;
}
bool compare(int *posl, int *parallel,int height)
{
	bool res = true;
	//cout << "height:" << height << endl;
	for (int i = 0;i < height;i++)
	{
		//cout << posl[i] << "?" << parallel[i] << endl;
		if (posl[i] != parallel[i])
		{
			res = false;
			break; 
		}
	}

	return res;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);            //�������������
	int mpi_rank, mpi_size;            //���������� ���-�� ��������� � ����� ��������
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size); 
	int *matrix;                //������� �����
	int *resultPOSL;            //�������������� ������ � ����������������
	int *tmpres;                //���� ����� ��� �������� ���� ����� ��� ���������� (��� �����) �������
	int *result;                //�������������� ������ � ������������
	int count = 0;                     //�� ���� �� ���� ��� ��������,� ���� ������ ����������
	int coeff = 0;                     //���������� �����,���������� ������ ��������
	double times;                      //���������� ��� ����������� ������� ������
	MPI_Status Status;                 //���������� ������� ���������� �������� ������ ������
	int height = atoi(argv[1]);
	int width = height;


	coeff = height / mpi_size;		
	tmpres = new int[coeff];           //���� ����� ��� �������� ���� ����� ��� ���������� (��� �����) �������
	matrix = new int[height*width];    //�������� ����������� ������ ��� �������
	result = new int[height];          //�������� ������ ��� ��������������� �������
	resultPOSL = new int[height];      //�������� ������ ��� ��������������� ������� ���������������� ��������
	if (mpi_rank == 0)                 //���� � ������� ��������
	{
		cout << "mpi_size=" << mpi_size << endl; //����� ���-�� ����������� ��������� � ���������
		                                         //��������� ���������� �����,������������ ������ ��������
		srand(time(0));
		cout << endl;
		
		make_matrix(matrix, width, height);  //��������� � ������� �������

		times = MPI_Wtime();                                 //�������� ���������������
		sumsumsum(matrix, resultPOSL, width, height);
		fprintf(stdout, "End(%d): %2.10f\n", mpi_rank, MPI_Wtime() - times);
		let_me_see(resultPOSL, height);


// [[ MPI ]]  
		times = MPI_Wtime();	  			  
		for (int i = 1; i < mpi_size; i++) //�� ���� �� ���� ��������� ���������
		{
			MPI_Send(&matrix[i*(width*coeff)], width*coeff, MPI_INT, i, 0, MPI_COMM_WORLD);

		}
		sumsumsum(matrix, tmpres,coeff,width);  //������� �����(�)		
	}
	else            //���� �� � ������� ��������
	{
	    int *tmp2 = new int[width*coeff];
		MPI_Recv(tmp2, width*coeff, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //��������� � ����� tum ������ ������ �� �������� ��������
		sumsumsum(tmp2, tmpres,coeff,width);
		delete tmp2;
		
	}
	MPI_Gather(tmpres, coeff, MPI_INT, result, coeff, MPI_INT, 0, MPI_COMM_WORLD);	
	
	if (mpi_rank == 0) //���� � ������� ��������
	{
		fprintf(stdout, "End(%d): %2.10f\n", mpi_rank, MPI_Wtime() - times); //�������, ������� ������� ������������� ��� ������������
		let_me_see(result, height);
		if (compare(resultPOSL, result, height) == true)
		{
			cout << "equal";
		}
		else cout << "not equal";
	}

	  delete matrix, result, tmpres; //������� ������
	  MPI_Finalize(); //��������� ������ � MPI
	  return 0;
}
