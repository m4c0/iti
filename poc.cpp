#pragma leco app
#pragma leco add_shader "iti.vert"
#pragma leco add_shader "iti.frag"

import casein;
import vee;
import voo;

namespace {
struct xz {
  float x;
  float z;
};
struct segment {
  xz xz0;
  xz xz1;
  float ceil_h;
  float floor_h;
};
class segbuf {
  vee::buffer m_buf;
  vee::device_memory m_mem;

public:
  static constexpr const auto max_segs = 1024;

  explicit segbuf(vee::physical_device pd) {
    m_buf = vee::create_vertex_buffer(sizeof(segment) * max_segs);
    m_mem = vee::create_host_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem, 0);

    vee::mapmem mem{*m_mem};
    auto *m = static_cast<segment *>(*mem);
    m[0] = {};
    m[0].xz0.x = -0.4;
    m[0].xz1.x = -0.2;

    m[1] = {};
    m[1].xz0.x = 0.6;
    m[1].xz1.x = 0.4;
  }

  void run(const voo::cmd_render_pass &scb, unsigned idx) const noexcept {
    vee::cmd_bind_vertex_buffers(*scb, idx, *m_buf);
  }

  [[nodiscard]] constexpr auto size() const noexcept { return 2; }
};
} // namespace

class thread : public voo::casein_thread {
public:
  void run() override;
};

void thread::run() {
  voo::device_and_queue dq{"iti", native_ptr()};
  auto cb = vee::allocate_primary_command_buffer(dq.command_pool());

  voo::one_quad quad{dq.physical_device()};
  segbuf segs{dq.physical_device()};

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};

    auto pl = vee::create_pipeline_layout();
    auto gp = vee::create_graphics_pipeline(
        *pl, sw.render_pass(),
        {
            voo::shader("iti.vert.spv").pipeline_vert_stage(),
            voo::shader("iti.frag.spv").pipeline_frag_stage(),
        },
        {
            quad.vertex_input_bind(),
            vee::vertex_input_bind_per_instance(sizeof(segment)),
        },
        {
            quad.vertex_attribute(0),
            vee::vertex_attribute_vec2(1, 0),
            vee::vertex_attribute_vec2(1, sizeof(xz)),
            vee::vertex_attribute_vec2(1, sizeof(xz) * 2),
            vee::vertex_attribute_vec2(1, sizeof(xz) * 2 + sizeof(float)),
        });

    resized() = false;
    while (!interrupted() && !resized()) {
      sw.acquire_next_image();

      {
        voo::cmd_buf_one_time_submit pcb{cb};

        auto scb = sw.cmd_render_pass(cb);
        vee::cmd_bind_gr_pipeline(cb, *gp);
        segs.run(scb, 1);
        quad.run(scb, 0, segs.size());
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
