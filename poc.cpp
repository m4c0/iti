#pragma leco app
#pragma leco add_shader "iti.vert"
#pragma leco add_shader "iti.frag"

import casein;
import vee;
import voo;

class thread : public voo::casein_thread {
public:
  void run() override;
};

void thread::run() {
  voo::device_and_queue dq{"iti", native_ptr()};
  auto cb = vee::allocate_primary_command_buffer(dq.command_pool());

  voo::one_quad quad{dq.physical_device()};

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};

    auto pl = vee::create_pipeline_layout();
    auto gp = vee::create_graphics_pipeline(
        *pl, sw.render_pass(),
        {
            voo::shader("iti.vert.spv").pipeline_vert_stage(),
            voo::shader("iti.frag.spv").pipeline_frag_stage(),
        },
        {quad.vertex_input_bind()}, {quad.vertex_attribute(0)});

    resized() = false;
    while (!interrupted() && !resized()) {
      sw.acquire_next_image();

      {
        voo::cmd_buf_one_time_submit pcb{cb};

        auto scb = sw.cmd_render_pass(cb);
        vee::cmd_bind_gr_pipeline(cb, *gp);
        quad.run(scb, 0);
      }

      sw.queue_submit(dq.queue(), cb);
      sw.queue_present(dq.queue());
    }

    vee::device_wait_idle();
  }
}

extern "C" void casein_handle(const casein::event &e) {
  static thread t{};
  t.handle(e);
}
