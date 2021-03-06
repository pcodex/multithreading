// deadmult.cpp : Defines the entry point for the console application.
//

#include <thread>
#include <list>
#include <iostream>
#include <mutex>

using namespace std;


class MyResource
{

	condition_variable cadd,caddread,cmult;
	mutex m1;
	bool doneadd,doneaddread, donemult;
	int in = 0;
	int jn = 0;

public:
	MyResource(int i, int j) :in(i), jn(j)
	{
		doneadd = false;
		doneaddread = false;
		donemult = false;
	}

	void addwriter()
	{
		thread::id tid = this_thread::get_id();
		cout << "\n Add Writer tid is " << tid << " \n";

		unique_lock<mutex> wraddlock(m1);
		in += 3;
		jn += 3;

		doneadd = true;
		cadd.notify_one();//since you have 1 add reader
	}

	void addreader()
	{

		thread::id tid = this_thread::get_id();

		unique_lock<mutex> rdaddlock(m1);
		while (!doneadd)
			cadd.wait(rdaddlock);

		cout << " \n Add Reader tid is " << tid << " \n ";
		cout << " \n in is " << in << " \n ";
		cout << " \n jn is " << jn << " \n ";

		doneaddread = true;
		caddread.notify_one();

	}

	void multwriter()
	{
		thread::id tid = this_thread::get_id();
		cout << "\n Mult Writer tid is " << tid << " \n";

		unique_lock<mutex> wrmultlock(m1);
		while (!doneaddread)
			caddread.wait(wrmultlock);

		in *= 3;
		jn *= 3;

		donemult = true;
		//cmult.notify_one();//since you have 1 mult reader
		cmult.notify_all();//since you have many mult reader
		
	}
	

	void multreader()
	{
		thread::id tid = this_thread::get_id();

		unique_lock<mutex> rdmultlock(m1);
		cmult.wait(rdmultlock, [&] {return donemult; });

		cout << " \n MultReader tid is " << tid << " \n ";
		cout << " \n in is " << in << " \n ";
		cout << " \n jn is " << jn << " \n ";
	}
};

void addwriterfun(MyResource& mr)
{
	mr.addwriter();
}

void addreaderfun(MyResource& mr)
{
	mr.addreader();
}

void multwriterfun(MyResource& mr)
{
	mr.multwriter();
}

void multreaderfun(MyResource& mr)
{
	mr.multreader();
}

int main()
{
	MyResource mrs(1, 2);

	thread radd(addreaderfun, ref(mrs));
	thread wadd([&] {mrs.addwriter(); });
	thread wmult(multwriterfun, ref(mrs));	
	thread rmult1(multreaderfun, ref(mrs));
	thread rmult2(multreaderfun, ref(mrs));
	thread rmult3(multreaderfun, ref(mrs));

	wadd.join();
	wmult.join();
	radd.join();
	rmult1.join();
	rmult2.join();
	rmult3.join();

	return 0;
}

