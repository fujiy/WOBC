#pragma once

#include "process.h"

namespace process {

class Timer : public Process {
public:
  Timer(const char* name, unsigned period_ms, bool auto_reload = true);
  ~Timer();

  void stop();
  void start();
  void reset();

  void setReload(bool auto_reload);
  void changePeriod(unsigned ms);


protected:
  virtual void callback() {} // Overwrite this

private:
  xTimerHandle timer_handle_;

  void onStart() override;

  static void entryPoint(xTimerHandle timer_handle);
};

}
