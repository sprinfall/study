// 问题：将总数为a的现金换成n种硬币的不同方式的数目；或者描述成，用n种硬币组成总现金为a的不同方式的数目。（可选：打印所有不同组合）
// 曾遇过的一道面试题，当时不知道怎么弄，最近在 *SICP*（计算机程序的构造和解释）里看到了。按照 SICP 的说法，应该可以用 *动态规划* 对这种树形的递归做时间上的优化。

#include <iostream>
//#include <list>
using namespace std;

// 原理：（照搬 SICP）
// 将总数为a的现金换成n种硬币的不同方式的数目等于
// - 将现金a换成除第一种硬币外的所有其他硬币的不同方式数目，加上 （都没有使用第一种硬币）
// - 将现金a - d换成所有种类硬币的不同方式的数目，其中d是第一种硬币的币值。（都使用了第一种硬币）

// Uncomment to print the result.
int coin_change(int amount, int coins[], int index)
{
	//static list<int> coin_stack;

	if (amount == 0) {
		//copy(coin_stack.begin(), coin_stack.end(), ostream_iterator<int>(cout, " "));
		//cout << endl;
		return 1;
	}
	else if (amount < 0 || index == 0) { return 0; }
	else {
		int count = coin_change(amount, coins, index - 1);
		//coin_stack.push_back(coins[index - 1]);
		count += coin_change(amount - coins[index - 1], coins, index);
		//coin_stack.pop_back();
		return count;
	}
}

int main()
{
	int coins[] = {1, 2, 5, 10}; // 顺序无关
	cout << coin_change(15, coins, sizeof(coins)/sizeof(int)) << endl;
}

