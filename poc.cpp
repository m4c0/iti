#pragma leco app

import casein;
import vee;
import voo;

class thread : public voo::casein_thread {
public:
  void run() override;
};

void thread::run() {
  voo::device_and_queue dq{"iti", native_ptr()};
  vee::command_buffer cb =
      vee::allocate_primary_command_buffer(dq.command_pool());

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};

    resized() = false;
    while (!interrupted() && !resized()) {
      sw.acquire_next_image();

      { voo::cmd_buf_one_time_submit pcb{cb}; }

      sw.queue_submit(dq.queue(), cb);
      sw.queue_present(dq.queue());
    }
  }
}

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  t.handle(e);
}
