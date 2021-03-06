// deadmult.cpp : Defines the entry point for the console application.
//

#include <thread>
#include <list>
#include <iostream>
#include <mutex>

using namespace std;


class MyResource
{

	condition_variable c1;
	mutex m1;
	bool done;
	int in = 0;
	int jn = 0;

public:
	MyResource(int i, int j) :in(i), jn(j)
	{
		done = false;
	}

	void writer()
	{
		thread::id tid = this_thread::get_id();
		cout << "\n Writer thread id is " << tid << " \n";		

		unique_lock<mutex> wrlock(m1);
		in += 3;		
		jn *= 2;

		c1.notify_all();//since you have 2 readers so notify_one is insufficient
		done = true;
	}

	void reader()
	{

		thread::id tid = this_thread::get_id();
					
		unique_lock<mutex> rdlock(m1);
		while(!done)
			c1.wait(rdlock);
		
		cout << "\n Reader thread id is " << tid << " \n";
		cout << "\n in is " << in << " \n";		
		cout << "\n jn is " << jn << " \n";		
		
	}
};

void writerfun(MyResource& mr)
{
	mr.writer();	
}

void readerfun(MyResource& mr)
{
	mr.reader();
}

int main()
{
	MyResource mrs(1, 2);

	thread t3(readerfun, ref(mrs));
	cout << "\n In Main t3 id is " << t3.get_id();
	thread t1(writerfun, ref(mrs));
	cout << "\n In Main t1 id is " << t1.get_id();
	thread t2(readerfun, ref(mrs));
	cout << "\n In Main t2 id is " << t2.get_id();
	

	t1.join();
	t2.join();
	t3.join();

	return 0;
}

