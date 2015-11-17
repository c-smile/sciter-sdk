// worker thread sample

#include "stdafx.h"
#include "ui-framework.h"

struct thread_params {
  sciter::value taskId;
  sciter::value progressCb; // "progress" callback, function passed from script
  sciter::value doneCb;     // "done" callback, function passed from script
};

void thread_body(thread_params params);

sciter::value window::exec_task(sciter::value taskId, sciter::value progressCb, sciter::value doneCb)
{
  thread_params params;
  params.taskId = taskId;
  params.progressCb = progressCb;
  params.doneCb = doneCb;

  sciter::thread(thread_body,params);
  return sciter::value(); // void method
}

// worker thread body, simulate time consuming task
void thread_body(thread_params params)
{
  for(int i = 1; i <= 100; ++i) {
    ::Sleep(100);
GUI_CODE_START
     params.progressCb.call(i); // report task progress
GUI_CODE_END
  }
  // report task completion,
  // we can pass some result data here, for now just taskId
GUI_CODE_START
    params.doneCb.call(params.taskId);  
GUI_CODE_END
}

#if 0 // equivalent of the above but wihtout macro
  void thread_body(thread_params params)
  {
    for(int i = 1; i <= 10; ++i) {
      ::Sleep(1000);
      gui_exec([&]()
      {
          params.progressCb.call(i*10); // report task progress
      });
    }
    // report task completion,
    // we can pass some result data here, for now just taskId
      gui_exec([&]()
      {
          params.doneCb.call(params.taskId);  
      });
  }
#endif




