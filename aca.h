#pragma once
#include<iostream>
#include<math.h>
#include<time.h>
//���нڵ���Ŀ
#define N 18
double temp_double_array[N][2] = {
};

//��������
#define M 75
//���ѭ������NcMax
int NcMax = 1000;
//��Ϣ�������ӣ���������ʽ���ӣ�ȫ����Ϣ�ػӷ��������ֲ���Ϣ�ػӷ�����, ״̬ת�ƹ�ʽ�е�q0
double alpha = 2, beta = 5, rou = 0.1, alpha1 = 0.1, qzero = 0.1;
//-----------����������------------------------------------------------------------------------


//===========================================================================================================
//�ֲ�����ʱ��ʹ�õĵĳ���������������ڷ����õ���һ������
//ʲô������ڷ���?:)���Ǵ�Դ�ڵ������ÿ��ѡ��һ��������̵ĵ����������еĽڵ�õ���·��
//ÿ���ڵ㶼������ΪԴ�ڵ�������
double Lnn;
//�����ʾ��������֮��ľ���
double allDistance[N][N];

//������������֮��ľ���
double calculateDistance(int i, int j)
{
	return sqrt(pow((temp_double_array[i][0] - temp_double_array[j][0]), 2.0) + pow((temp_double_array[i][1] - temp_double_array[j][1]), 2.0));
}

//�ɾ����ʾ��������֮��ľ���
void calculateAllDistance()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (i != j)
			{
				allDistance[i][j] = calculateDistance(i, j);
				allDistance[j][i] = allDistance[i][j];
			}
		}
	}
}

//��þ���n�����е�·������
double calculateSumOfDistance(int* tour)
{
	double sum = 0;
	for (int i = 0; i < N; i++)
	{
		int row = *(tour + 2 * i);
		int col = *(tour + 2 * i + 1);
		sum += allDistance[row][col];
	}
	return sum;
}

class ACSAnt;

class AntColonySystem
{
private:
	double info[N][N], visible[N][N];//�ڵ�֮�����Ϣ��ǿ��,�ڵ�֮����ܼ���
public:
	AntColonySystem()
	{
	}
	//���㵱ǰ�ڵ㵽��һ�ڵ�ת�Ƶĸ���
	double Transition(int i, int j);
	//�ֲ����¹���
	void UpdateLocalPathRule(int i, int j);
	//��ʼ��
	void InitParameter(double value);
	//ȫ����Ϣ�ظ���
	void UpdateGlobalPathRule(int* bestTour, int globalBestLength);
};

//���㵱ǰ�ڵ㵽��һ�ڵ�ת�Ƶĸ���
double AntColonySystem::Transition(int i, int j)
{
	if (i != j)
	{
		return (pow(info[i][j], alpha) * pow(visible[i][j], beta));
	}
	else
	{
		return 0.0;
	}
}
//�ֲ����¹���
void AntColonySystem::UpdateLocalPathRule(int i, int j)
{
	info[i][j] = (1.0 - alpha1) * info[i][j] + alpha1 * (1.0 / (N * Lnn));
	info[j][i] = info[i][j];
}
//��ʼ��
void AntColonySystem::InitParameter(double value)
{
	//��ʼ��·���ϵ���Ϣ��ǿ��tao0
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			info[i][j] = value;
			info[j][i] = value;
			if (i != j)
			{
				visible[i][j] = 1.0 / allDistance[i][j];
				visible[j][i] = visible[i][j];
			}
		}
	}
}

//ȫ����Ϣ�ظ���
void AntColonySystem::UpdateGlobalPathRule(int* bestTour, int globalBestLength)
{
	for (int i = 0; i < N; i++)
	{
		int row = *(bestTour + 2 * i);
		int col = *(bestTour + 2 * i + 1);
		info[row][col] = (1.0 - rou) * info[row][col] + rou * (1.0 / globalBestLength);
		info[col][row] = info[row][col];
	}
}

class ACSAnt
{
private:
	AntColonySystem* antColony;
protected:
	int startCity, cururentCity;//��ʼ���б�ţ���ǰ���б��
	int allowed[N];//���ɱ�	
	int Tour[N][2];//��ǰ·��
	int currentTourIndex;//��ǰ·����������0��ʼ���洢���Ͼ������еı��
public:
	ACSAnt(AntColonySystem* acs, int start)
	{
		antColony = acs;
		startCity = start;
	}
	//��ʼ����
	int* Search();
	//ѡ����һ�ڵ�
	int Choose();
	//�ƶ�����һ�ڵ�
	void MoveToNextCity(int nextCity);

};

//��ʼ����
int* ACSAnt::Search()
{
	cururentCity = startCity;
	int toCity;
	currentTourIndex = 0;
	for (int i = 0; i < N; i++)
	{
		allowed[i] = 1;
	}
	allowed[cururentCity] = 0;
	int endCity;
	int count = 0;
	do
	{
		count++;
		endCity = cururentCity;
		toCity = Choose();
		if (toCity >= 0)
		{
			MoveToNextCity(toCity);
			antColony->UpdateLocalPathRule(endCity, toCity);
			cururentCity = toCity;
		}
	} while (toCity >= 0);
	MoveToNextCity(startCity);
	antColony->UpdateLocalPathRule(endCity, startCity);

	return *Tour;
}

//ѡ����һ�ڵ�
int ACSAnt::Choose()
{
	int nextCity = -1;
	double q = rand() / (double)RAND_MAX;
	//��� q <= q0,������֪ʶ�������򰴸���ת�ƣ�
	if (q <= qzero)
	{
		double probability = -1.0;//ת�Ƶ���һ�ڵ�ĸ���
		for (int i = 0; i < N; i++)
		{
			//ȥ�����ɱ������߹��Ľڵ�,��ʣ�½ڵ���ѡ�������ʵĿ��нڵ�
			if (1 == allowed[i])
			{
				double prob = antColony->Transition(cururentCity, i);
				if (prob > probability)
				{
					nextCity = i;
					probability = prob;
				}
			}
		}
	}
	else
	{
		//������ת��			
		double p = rand() / (double)RAND_MAX;//����һ�������,�����ж������ĸ������
		double sum = 0.0;
		double probability = 0.0;//���ʵ�����㣬p �����ĸ�����Σ���õ���ת�Ƶķ���
		//������ʹ�ʽ�ķ�ĸ��ֵ
		for (int i = 0; i < N; i++)
		{
			if (1 == allowed[i])
			{
				sum += antColony->Transition(cururentCity, i);
			}
		}
		for (int j = 0; j < N; j++)
		{
			if (1 == allowed[j] && sum > 0)
			{
				probability += antColony->Transition(cururentCity, j) / sum;
				if (probability >= p || (p > 0.9999 && probability > 0.9999))
				{
					nextCity = j;
					break;
				}
			}
		}
	}
	return nextCity;
}

//�ƶ�����һ�ڵ�
void ACSAnt::MoveToNextCity(int nextCity)
{
	allowed[nextCity] = 0;
	Tour[currentTourIndex][0] = cururentCity;
	Tour[currentTourIndex][1] = nextCity;
	currentTourIndex++;
	cururentCity = nextCity;
}

//------------------------------------------
//ѡ����һ���ڵ㣬�������ĺ���������ĳ���
int ChooseNextNode(int currentNode, int visitedNode[])
{
	int nextNode = -1;
	double shortDistance = 0.0;
	for (int i = 0; i < N; i++)
	{
		//ȥ�����߹��Ľڵ�,��ʣ�½ڵ���ѡ���������Ľڵ�
		if (1 == visitedNode[i])
		{
			if (shortDistance == 0.0)
			{
				shortDistance = allDistance[currentNode][i];
				nextNode = i;
			}
			if (shortDistance < allDistance[currentNode][i])
			{
				nextNode = i;
			}
		}
	}
	return nextNode;
}

//��һ���ڵ�������ھ��뷽�����㳤��
double CalAdjacentDistance(int node)
{
	double sum = 0.0;
	int visitedNode[N];
	for (int j = 0; j < N; j++)
	{
		visitedNode[j] = 1;
	}
	visitedNode[node] = 0;
	int currentNode = node;
	int nextNode;
	do
	{
		nextNode = ChooseNextNode(currentNode, visitedNode);
		if (nextNode >= 0)
		{
			sum += allDistance[currentNode][nextNode];
			currentNode = nextNode;
			visitedNode[currentNode] = 0;
		}
	} while (nextNode >= 0);
	sum += allDistance[currentNode][node];
	return sum;
}
