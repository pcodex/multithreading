// deadmult.cpp : Defines the entry point for the console application.
//


#include <thread>
#include <list>
#include <iostream>
#include <mutex>

using namespace std;

 
class MyResource
{

	mutex m1;
	mutex m2;
	int in = 0;
	int jn = 0;

public:
	MyResource(int i, int j):in(i),jn(j)
	{}

	void writer()
	{
		thread::id tid = this_thread::get_id();
		cout << "\n Writer thread id is " << tid << " \n";

		m1.lock();
		in += 3;
		m2.lock();
		jn *= 2;

		m1.unlock();
		m2.unlock();		
		
	}

	void reader()
	{
		
		thread::id tid = this_thread::get_id();
		cout << "\n Reader thread id is " << tid << " \n";

		/* //no deadlocking
		m1.lock();
		cout << "\n in is " << in << " \n";
		m2.lock();		
		cout << "\n jn is " << jn << " \n";		
		m2.unlock();
		m1.unlock();
		*/

		/* to deadlock*/
		m2.lock();
		cout << "\n in is " << in << " \n";
		m1.lock();		
		cout << "\n jn is " << jn << " \n";		
		m2.unlock();
		m1.unlock();
		
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
	
	thread t1(writerfun,ref(mrs));
	thread t2(readerfun, ref(mrs));	

	t1.join();
	t2.join();	

    return 0;
}

