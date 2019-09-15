// pplTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <ppl.h>
#include <ppltasks.h>
#include <array>
#include <thread>
#include <functional>
#include <numeric>
#include <amp.h>
#include <amp_math.h>
using namespace std;
using namespace Concurrency;
using namespace Concurrency::fast_math;

bool is_prime(int n) {
	if (n == 1)
		return false;
	for (int i = 2; i*i <= n; i++) {//检验到根号n就可以了
		if (n%i == 0)
			return false;
	}
	return true;
}

void TestPpl()
{
	std::array<double, 10> seq;
	generate(begin(seq), end(seq), [] {
		return (double)rand() / double(RAND_MAX);
		});
	for_each(begin(seq), end(seq), [](double& item) {
		item = sin(item);
		});



	std::cout << "Hello World!\n";
	std::array<double, 10> sins;
	cout << "main thread id=" << this_thread::get_id() << endl;
	parallel_for(0, 10, [&](int idx) {
		//cout << "thread id=" << this_thread::get_id() << endl;
		sins[idx] = sin(seq[idx]);
		});

	std::array<int, 10> nums = { 1, 2 ,3, 4, 5, 6, 7, 8, 9, 10 };
	//volatile long odd_count = 0;
	combinable<int> odd_count([] { return 0; });
	parallel_for_each(begin(nums), end(nums), [&odd_count](int value) {
		if (value % 2 == 1) odd_count.local()++;
		});
	cout << odd_count.combine(plus<int>()) << endl;
	//cout << odd_count.combine(plus<int>()) << endl;

	typedef std::array<int, 10>::iterator arr_iter;
	auto total_odd_count = parallel_reduce(begin(nums), end(nums), 0,
		[](arr_iter block_begin, arr_iter block_end, int init)-> int {
			int sub_odd_count = init;
			for_each(block_begin, block_end, [&sub_odd_count](int value) {
				if (value % 2 == 1) sub_odd_count++;
				});
			return sub_odd_count;
		}, plus<int>());

	int sum_of_odds = 0;
	int first_prime = 0;


	create_task([&] {
		generate(begin(nums), end(nums), [] {return rand() % 1000; });
		}).then([&] {
			std::array<function<void()>, 2> actions = {
				[nums, &sum_of_odds]
				{
					sum_of_odds = accumulate(begin(nums), end(nums), 0, [](int acc, int item) {
						return acc + (item % 2 == 1 ? item : 0);
					});
				},
				[nums, &first_prime]
				{
					auto found = find_if(begin(nums), end(nums), is_prime);
					if (found != end(nums))
					{
						first_prime = *found;
					}
				}
			};

			parallel_for_each(begin(actions), end(actions), [](function<void()> action) {
				action(); });
			}).then([&]() {
				cout << "sum of odds=" << sum_of_odds << ", first_prime=" << first_prime << endl;
				});

}

void TestAmp()
{
	const int size = 10;
	std::array<float, size> seq;
	generate(begin(seq), end(seq), [] {
		return (float)rand() / (float)RAND_MAX;
		});
	array_view<float, 1> seq_view(size, seq);
	parallel_for_each(seq_view.extent, [=](index<1> idx) restrict(amp) {
		seq_view[idx] = sin(seq_view[idx]);
		
		});
	
	 std::cout << "hello" << std::endl;
}

vector<int> create_matrix(int size)
{
	vector<int> matrix(size);
	generate(begin(matrix), end(matrix), [] {
		return rand() % 100;
		});
	return matrix;
}

void TestAddMatrix()
{
	const int rows = 100;
	const int columns = 100;
	vector<int> a = create_matrix(rows * columns);
	vector<int> b = create_matrix(rows * columns);
	vector<int> c(rows * columns);
	array_view<const int, 2> av(rows, columns, a);
	array_view<const int, 2> bv(rows, columns, b);
	array_view<int, 2> cv(rows, columns, c);
	cv.discard_data();
	parallel_for_each(cv.extent, [=](index<2> idx) restrict(amp) {
		cv[idx] = av[idx] + bv[idx];
		});
	index<2> idx(14, 12);
	std::cout << cv[idx] << std::endl;
}

int main()
{
	TestAddMatrix();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
