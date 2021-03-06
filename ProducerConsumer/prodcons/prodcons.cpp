// prodcons.cpp : Defines the entry point for the console application.
//
//Note : Comment out the start of the shutter thread to see normal Producer-Consumer operations

#include <vector>
#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>
using namespace std;

const int capacity = 5;
mutex mutx;
condition_variable produce_done;
condition_variable consume_done;
bool shutdown = false;

void producer(queue<int>& q, int avalue)
{
	unique_lock<mutex> prodlck(mutx);

	if (q.size() < capacity && !shutdown)
	{
		//cout << "\n Queue has space. Producer pushing " << avalue << " into queue \n";
		q.push(avalue);
	}
	else
	{
		cout << "\n Producer "<<avalue<<" waiting since queue is full. Value to push is "<<avalue<<" \n";
		consume_done.wait(prodlck, [&]() {return (q.size() < capacity)||(shutdown==true); });
		if (!shutdown) {

			cout << "\n Producer "<<avalue<<" pushing " << avalue << " into free queue \n";
			q.push(avalue);
		}
		else
		{
			cout << "\nShutting down Producer thread "<<avalue<<" \n";
		}
	}

	produce_done.notify_all();
}


void consumer(queue<int>& q, int consid)
{
	unique_lock<mutex> conslck(mutx);
	cout << "\n Consumer " <<consid<<" waiting since queue is empty \n";
	produce_done.wait(conslck, [&]() {return (q.size()>0)||(shutdown==true);});
	if (!shutdown)
	{
		cout << "\n Consumer " << consid << " popping out value " << q.front() << " \n";
		q.pop();
	}
	else
	{
		cout << "\nShutting down Consumer thread "<<consid<<" \n";
	}

	consume_done.notify_all();	
}

void shutdownthreads()
{
	unique_lock<mutex> shtdwnlck(mutx);
	shutdown = true;
	consume_done.notify_all();
	produce_done.notify_all();
}



int main()
{
	queue<int> shqu;
	vector<thread> prods;
	vector<thread> cons;

	int prodcnt = 10;
	int conscnt = 4;//4 slots will be consumed and 4 new producers will populate the queue. The 10th producer will wait for a free slot
	
	for (int pi = 0; pi < prodcnt; pi++)
	{
		//cout << "\n Calling producer with value " << pi+1 << " \n";
		prods.push_back(thread(producer,ref(shqu),pi+1));
		this_thread::sleep_for(chrono::seconds(1));
		//cout << "\n Q front is : " << shqu.front() << " \n";
	}

	//Shutdown Scenario 1
	//start a shutdown thread. This should shutdown the blocking Producer No:10
	thread shutter = thread(shutdownthreads);
		

	for (int ci = 0; ci < conscnt; ci++)
	{		
		cons.push_back(thread(consumer,ref(shqu),ci+1));
		this_thread::sleep_for(chrono::seconds(1));	
	}
		
	//10th producer will wait for space to be freed up so start consumer no:5 here after a wait of 15 secs
	cout << "\n Producer 10 is waiting for a free slot \n";
	this_thread::sleep_for(chrono::seconds(15));
	
	//Shutdown Scenario 2
	//start a shutdown thread. This should shutdown the blocking Producer No:10
	//thread shutter = thread(shutdownthreads);

	thread tc5 = thread(consumer, ref(shqu),5);

	//now 5 slots are full. so start 6 new consumers so that the last consumer will wait
	thread tc6 = thread(consumer, ref(shqu),6);
	thread tc7 = thread(consumer, ref(shqu),7);
	thread tc8 = thread(consumer, ref(shqu),8);
	thread tc9 = thread(consumer, ref(shqu),9);
	thread tc10 = thread(consumer, ref(shqu),10);
	thread tc11 = thread(consumer, ref(shqu),11);

	for (int ai = 0; ai < prodcnt; ++ai)
	{
		prods[ai].join();		
	}

	for (int bi = 0; bi < conscnt; ++bi)
	{		
		cons[bi].join();
	}

	//consumer 11 will be waiting. so start a producer which can then be consumed and lead
	//to a graceful shutdown of the main thread
	//this_thread::sleep_for(chrono::seconds(15));
	//thread tp11 = thread(producer, ref(shqu), 11);

	//Shutdown Scenario 3
	//start a shutdown thread. This should shutdown the waiting Consumer No:11
	//thread shutter = thread(shutdownthreads);

	tc5.join();
	tc6.join();
	tc7.join();
	tc8.join();
	tc9.join();
	tc10.join();
	tc11.join();

	//tp11.join();
	shutter.join();

	cout << "\n Exiting main in 8 seconds \n";
	for (int sec = 8; sec >= 1; --sec)
	{
		cout << sec << "...";
		this_thread::sleep_for(chrono::seconds(1));
	}
    return 0;
}

