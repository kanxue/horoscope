#include "common/system/concurrency//thread_pool.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

void Entery(int index)
{
    LOG(INFO) << "enter index " << index;
    LOG(INFO) << "doing index " << index;
    ThisThread::Sleep(10);
    LOG(INFO) << "leave index " << index;
}

int main(int argc, char** argv)
{
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);


    LOG(INFO) << "begin main function.";
    ThreadPool th(4, 4);
    for (int idx = 0; idx < 5; ++idx) {
        th.AddTask(NewClosure(&Entery, idx));
    }

    th.Terminate();

    LOG(INFO) << "All is ok.";

    return 0;
}

