#pragma once
#include<iostream>
#include<math.h>
#include<time.h>
//城市节点数目
#define N 18
double temp_double_array[N][2] = {
};

//蚂蚁数量
#define M 75
//最大循环次数NcMax
int NcMax = 1000;
//信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
double alpha = 2, beta = 5, rou = 0.1, alpha1 = 0.1, qzero = 0.1;
//-----------问题三结束------------------------------------------------------------------------


//===========================================================================================================
//局部更新时候使用的的常量，它是由最近邻方法得到的一个长度
//什么是最近邻方法?:)就是从源节点出发，每次选择一个距离最短的点来遍历所有的节点得到的路径
//每个节点都可能作为源节点来遍历
double Lnn;
//矩阵表示两两城市之间的距离
double allDistance[N][N];

//计算两个城市之间的距离
double calculateDistance(int i, int j)
{
	return sqrt(pow((temp_double_array[i][0] - temp_double_array[j][0]), 2.0) + pow((temp_double_array[i][1] - temp_double_array[j][1]), 2.0));
}

//由矩阵表示两两城市之间的距离
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

//获得经过n个城市的路径长度
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
	double info[N][N], visible[N][N];//节点之间的信息素强度,节点之间的能见度
public:
	AntColonySystem()
	{
	}
	//计算当前节点到下一节点转移的概率
	double Transition(int i, int j);
	//局部更新规则
	void UpdateLocalPathRule(int i, int j);
	//初始化
	void InitParameter(double value);
	//全局信息素更新
	void UpdateGlobalPathRule(int* bestTour, int globalBestLength);
};

//计算当前节点到下一节点转移的概率
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
//局部更新规则
void AntColonySystem::UpdateLocalPathRule(int i, int j)
{
	info[i][j] = (1.0 - alpha1) * info[i][j] + alpha1 * (1.0 / (N * Lnn));
	info[j][i] = info[i][j];
}
//初始化
void AntColonySystem::InitParameter(double value)
{
	//初始化路径上的信息素强度tao0
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

//全局信息素更新
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
	int startCity, cururentCity;//初始城市编号，当前城市编号
	int allowed[N];//禁忌表	
	int Tour[N][2];//当前路径
	int currentTourIndex;//当前路径索引，从0开始，存储蚂蚁经过城市的编号
public:
	ACSAnt(AntColonySystem* acs, int start)
	{
		antColony = acs;
		startCity = start;
	}
	//开始搜索
	int* Search();
	//选择下一节点
	int Choose();
	//移动到下一节点
	void MoveToNextCity(int nextCity);

};

//开始搜索
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

//选择下一节点
int ACSAnt::Choose()
{
	int nextCity = -1;
	double q = rand() / (double)RAND_MAX;
	//如果 q <= q0,按先验知识，否则则按概率转移，
	if (q <= qzero)
	{
		double probability = -1.0;//转移到下一节点的概率
		for (int i = 0; i < N; i++)
		{
			//去掉禁忌表中已走过的节点,从剩下节点中选择最大概率的可行节点
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
		//按概率转移			
		double p = rand() / (double)RAND_MAX;//生成一个随机数,用来判断落在哪个区间段
		double sum = 0.0;
		double probability = 0.0;//概率的区间点，p 落在哪个区间段，则该点是转移的方向
		//计算概率公式的分母的值
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

//移动到下一节点
void ACSAnt::MoveToNextCity(int nextCity)
{
	allowed[nextCity] = 0;
	Tour[currentTourIndex][0] = cururentCity;
	Tour[currentTourIndex][1] = nextCity;
	currentTourIndex++;
	cururentCity = nextCity;
}

//------------------------------------------
//选择下一个节点，配合下面的函数来计算的长度
int ChooseNextNode(int currentNode, int visitedNode[])
{
	int nextNode = -1;
	double shortDistance = 0.0;
	for (int i = 0; i < N; i++)
	{
		//去掉已走过的节点,从剩下节点中选择距离最近的节点
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

//给一个节点由最近邻距离方法计算长度
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
