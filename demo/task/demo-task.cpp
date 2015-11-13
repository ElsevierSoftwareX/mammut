/*
 * demo-process.cpp
 * This is a minimal demo on processes and threads management.
 *
 * Created on: 10/01/2015
 *
 * =========================================================================
 *  Copyright (C) 2014-, Daniele De Sensi (d.desensi.software@gmail.com)
 *
 *  This file is part of mammut.
 *
 *  mammut is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public
 *  License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.

 *  mammut is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with mammut.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 * =========================================================================
 */

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <mammut/mammut.hpp>

using namespace mammut;
using namespace mammut::task;
using namespace mammut::topology;
using namespace std;

static pid_t gettid(){
#ifdef SYS_gettid
    return syscall(SYS_gettid);
#else
    throw runtime_error("gettid() not available.");
#endif
}

void* idleThread(void* arg){
    pid_t tid = gettid();
    ThreadHandler* thisThread = ((ProcessHandler*) arg)->getThreadHandler(tid);
    double coreUsage = 0;
    uint sleepingSecs = 10;
    cout << "[Idle Thread] Created [Tid: " << tid << "]. Sleeping " << sleepingSecs << " seconds." << endl;
    thisThread->resetCoreUsage();
    sleep(sleepingSecs);
    thisThread->getCoreUsage(coreUsage);
    cout << "[Idle Thread] Core usage over the last " << sleepingSecs << " seconds: " << coreUsage << "%" << endl;
    ((ProcessHandler*) arg)->releaseThreadHandler(thisThread);
    return NULL;
}

void* sinThread(void* arg){
    pid_t tid = gettid();
    ThreadHandler* thisThread = ((ProcessHandler*) arg)->getThreadHandler(tid);
    double coreUsage = 0;
    uint sinIterations = 100000000;
    double sinRes = rand();
    cout << "[Sin Thread] Created [Tid: " << tid << "]. Computing " << sinIterations << " sin() iterations." << endl;
    thisThread->resetCoreUsage();
    for(uint i = 0; i < sinIterations; i++){
        sinRes = sin(sinRes);
    }
    thisThread->getCoreUsage(coreUsage);
    cout << "[Sin Thread] SinResult: " << sinRes << endl;
    cout << "[Sin Thread] Core usage during " << sinIterations << " sin iterations: " << coreUsage << "%" << endl;
    ((ProcessHandler*) arg)->releaseThreadHandler(thisThread);
    return NULL;
}

int main(int argc, char** argv){
    Mammut m;
    Topology* topology = m.getInstanceTopology();
    TasksManager* pm = m.getInstanceTask();

    vector<TaskId> processes = pm->getActiveProcessesIdentifiers();
    cout << "There are " << processes.size() << " active processes: ";
    for(size_t i = 0; i < processes.size(); i++){
        cout << "[" << processes.at(i) << "]";
    }
    cout << endl;

    cout << "[Process] Getting information (Pid: " << getpid() << ")." << endl;
    ProcessHandler* thisProcess = pm->getProcessHandler(getpid());

    VirtualCoreId vid;
    assert(thisProcess->getVirtualCoreId(vid));
    vector<PhysicalCore*> physicalCores = topology->getPhysicalCores();
    cout << "[Process] Currently running on virtual core: " << vid << endl;
    cout << "[Process] Moving to physical core " << physicalCores.back()->getPhysicalCoreId() << endl;
    //    assert(thisProcess->move(physicalCores.back()));

    cout << "[Process] Creating some threads..." << endl;
    pthread_t tid_1, tid_2;
    pthread_create(&tid_1, NULL, idleThread, thisProcess);
    pthread_create(&tid_2, NULL, sinThread, thisProcess);

    vector<TaskId> threads = thisProcess->getActiveThreadsIdentifiers();
    cout << "[Process] There are " << threads.size() << " active threads: ";
    for(size_t i = 0; i < threads.size(); i++){
        cout << "[" << threads.at(i) << "]";
    }
    cout << endl;

    uint priority;
    assert(thisProcess->getPriority(priority));
    cout << "[Process] Current priority: " << priority << endl;
    cout << "[Process] Try to change priority to max (" << MAMMUT_PROCESS_PRIORITY_MAX << ")" << endl;
    assert(thisProcess->setPriority(MAMMUT_PROCESS_PRIORITY_MAX));
    sleep(1);
    assert(thisProcess->getPriority(priority));
    cout << "[Process] New priority: " << priority << endl;
    assert(priority == MAMMUT_PROCESS_PRIORITY_MAX);

    double coreUsage = 0;
    thisProcess->resetCoreUsage();
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);
    thisProcess->getCoreUsage(coreUsage);
    cout << "[Process] Core usage " << coreUsage << "%" << endl;

    pm->releaseProcessHandler(thisProcess);
    return 1;
}
