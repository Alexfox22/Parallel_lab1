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
	
	for (int i = 0; i < hei; i++) //идем по необходимым процессу строкам
	{
		int sum = 0;
		for (int j = i*wid; j < (i + 1)*wid; j++)
		{
			sum += mat[j]; //суммируем элементы строки				
		}
		
		resmat[i] = sum;
	}
}
void let_me_see(int *resmat, int height) 
{   
	int r = 1;
	for (int i = 0; i < height; i++)
	{
		cout << r << "str: " << resmat[i] << endl; //выводим суммы
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
	MPI_Init(&argc, &argv);            //инициализаци€
	int mpi_rank, mpi_size;            //переменные кол-ва процессов и ранга процесса
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size); 
	int *matrix;                //матрица Ќ”∆Ќј
	int *resultPOSL;            //результирующий вектор в ѕќ—Ћ≈ƒќ¬ј“≈Ћ№Ќќ…
	int *tmpres;                //сюда пишут все процессы свои суммы дл€ нескольких (или одной) строчек
	int *result;                //результирующий вектор в ѕј–јЋЋ≈Ћ№Ќќ…
	int count = 0;                     //не надо на мен€ так смотреть,€ ведь просто переменна€
	int coeff = 0;                     //количество строк,выдел€емых одному процессу
	double times;                      //переменные дл€ отображени€ времени работы
	MPI_Status Status;                 //переменна€ статуса выполнени€ операции приема данных
	int height = atoi(argv[1]);
	int width = height;


	coeff = height / mpi_size;		
	tmpres = new int[coeff];           //сюда пишут все процессы свои суммы дн€ нескольких (или одной) строчек
	matrix = new int[height*width];    //выдел€ем необходимую пам€ть дл€ матрицы
	result = new int[height];          //выдел€ем пам€ть дл€ результирующего вектора
	resultPOSL = new int[height];      //выдел€ем пам€ть дл€ результирующего вектора ѕќ—Ћ≈ƒќ¬ј“≈Ћ№Ќџ… јЋ√ќ–»“ћ
	if (mpi_rank == 0)                 //если в нулевом процессе
	{
		cout << "mpi_size=" << mpi_size << endl; //вывод кол-ва действующих процессов в программе
		                                         //вычисл€ем количество строк,передаваемых одному процессу
		srand(time(0));
		cout << endl;
		
		make_matrix(matrix, width, height);  //заполн€ем и выводим матрицу

		times = MPI_Wtime();                                 //Ќј„»Ќј≈ћ ѕќ—Ћ≈ƒќ¬ј“≈Ћ№Ќќ
		sumsumsum(matrix, resultPOSL, width, height);
		fprintf(stdout, "End(%d): %2.10f\n", mpi_rank, MPI_Wtime() - times);
		let_me_see(resultPOSL, height);


// [[ MPI ]]  
		times = MPI_Wtime();	  			  
		for (int i = 1; i < mpi_size; i++) //мы идем по всем ненулевым процессам
		{
			MPI_Send(&matrix[i*(width*coeff)], width*coeff, MPI_INT, i, 0, MPI_COMM_WORLD);

		}
		sumsumsum(matrix, tmpres,coeff,width);  //считаем сумму(ы)		
	}
	else            //если не в нулевом процессе
	{
	    int *tmp2 = new int[width*coeff];
		MPI_Recv(tmp2, width*coeff, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //принимаем в буфер tum нужную строку из нулевого процесса
		sumsumsum(tmp2, tmpres,coeff,width);
		delete tmp2;
		
	}
	MPI_Gather(tmpres, coeff, MPI_INT, result, coeff, MPI_INT, 0, MPI_COMM_WORLD);	
	
	if (mpi_rank == 0) //если в нулевом процессе
	{
		fprintf(stdout, "End(%d): %2.10f\n", mpi_rank, MPI_Wtime() - times); //выводим, сколько времени потребовалось дл€ суммировани€
		let_me_see(result, height);
		if (compare(resultPOSL, result, height) == true)
		{
			cout << "equal";
		}
		else cout << "not equal";
	}

	  delete matrix, result, tmpres; //очищаем пам€ть
	  MPI_Finalize(); //завершаем работу с MPI
	  return 0;
}
