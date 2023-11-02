#pragma leco app
#pragma leco add_shader "iti.vert"
#pragma leco add_shader "iti.frag"

import casein;
import missingno;
import sitime;
import vee;
import voo;

namespace {
struct xz {
  float x;
  float z;
};
struct ccff {
  float c;
  float wc;
  float wf;
  float f;
};
struct segment {
  xz xz0;
  xz xz1;
  ccff cf;
};
class segbuf {
  vee::buffer m_buf;
  vee::device_memory m_mem;
  unsigned m_size;

public:
  static constexpr const auto max_segs = 1024;

  explicit segbuf(vee::physical_device pd) {
    m_buf = vee::create_vertex_buffer(sizeof(segment) * max_segs);
    m_mem = vee::create_host_buffer_memory(pd, *m_buf);
    vee::bind_buffer_memory(*m_buf, *m_mem, 0);

    vee::mapmem mem{*m_mem};
    auto *m = static_cast<segment *>(*mem);
    auto *sm = m;
    *m++ = {
        .xz0 = {0, 0},
        .xz1 = {5, 0},
        .cf = {2.5, 0.0, 0.0, -2.5},
    };
    *m++ = {
        .xz0 = {5, 0},
        .xz1 = {10, 0},
        .cf = {1.5, 0.0, 0.0, -1.5},
    };
    *m++ = {
        .xz0 = {0, 0},
        .xz1 = {0, 10},
        .cf = {2.5, 0.0, 0.0, -2.5},
    };
    *m++ = {
        .xz0 = {10, 0},
        .xz1 = {10, 10},
        .cf = {1.5, 0.0, 0.0, -1.5},
    };
    *m++ = {
        .xz0 = {10, 10},
        .xz1 = {5, 10},
        .cf = {1.5, 0.0, 0.0, -1.5},
    };
    *m++ = {
        .xz0 = {0, 10},
        .xz1 = {5, 10},
        .cf = {2.5, 0.0, 0.0, -2.5},
    };
    *m++ = {
        .xz0 = {5, 0},
        .xz1 = {5, 3.5},
        .cf = {2.5, 1.5, -1.5, -2.5},
    };
    *m++ = {
        .xz0 = {5, 4},
        .xz1 = {5, 10},
        .cf = {2.5, 1.5, -1.5, -2.5},
    };
    *m++ = {
        .xz0 = {5, 3.5},
        .xz1 = {5.5, 3.5},
        .cf = {1.5, 0.0, 0.0, -1.5},
    };
    *m++ = {
        .xz0 = {5.5, 3.5},
        .xz1 = {5.5, 4},
        .cf = {1.5, 0.0, 0.0, -1.5},
    };
    *m++ = {
        .xz0 = {5.5, 4},
        .xz1 = {5, 4},
        .cf = {1.5, 0.0, 0.0, -1.5},
    };
    *m++ = {
        .xz0 = {5, 4},
        .xz1 = {4.5, 4},
        .cf = {2.5, 0.0, 0.0, -2.5},
    };
    *m++ = {
        .xz0 = {4.5, 4},
        .xz1 = {4.5, 3.5},
        .cf = {2.5, 0.0, 0.0, -2.5},
    };
    *m++ = {
        .xz0 = {4.5, 3.5},
        .xz1 = {5, 3.5},
        .cf = {2.5, 0.0, 0.0, -2.5},
    };
    m_size = m - sm;
  }

  void run(const voo::cmd_render_pass &scb, unsigned idx) const noexcept {
    vee::cmd_bind_vertex_buffers(*scb, idx, *m_buf);
  }

  [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
};

struct cam {
  float x;
  float y;
  float z;
  float angle;
};
struct upc {
  cam camera{};
  float window_w;
  float window_h;
  float aspect;
  float fov;
  float time;
};
} // namespace

class thread : public voo::casein_thread {
  cam m_camera{2.0, 0.0, 6.0, -0.6};
  mno::opt<float> m_prev_x{};

public:
  void mouse_move(const casein::events::mouse_move &e) override {
    constexpr const auto mouse_speed = 1000.0f;

    float x = (*e).x;
    float px = m_prev_x.unwrap(x);
    m_camera.angle += (px - x) / mouse_speed;
    m_prev_x = x;
  }
  void run() override;
};

void thread::run() {
  sitime::stopwatch w{};

  voo::device_and_queue dq{"iti", native_ptr()};
  auto cb = vee::allocate_primary_command_buffer(dq.command_pool());

  voo::one_quad quad{dq.physical_device()};
  segbuf segs{dq.physical_device()};

  while (!interrupted()) {
    voo::swapchain_and_stuff sw{dq};

    auto pl = vee::create_pipeline_layout({
        vee::vert_frag_push_constant_range<upc>(),
    });
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
            vee::vertex_attribute_vec4(1, sizeof(xz) * 2),
        });

    resized() = false;
    while (!interrupted() && !resized()) {
      sw.acquire_next_image();

      upc pc = {
          .camera = m_camera,
          .window_w = static_cast<float>(sw.extent().width),
          .window_h = static_cast<float>(sw.extent().height),
          .aspect = sw.aspect(),
          .fov = 40.0f * 3.141592f / 180.0f,
          .time = w.millis() / 1000.0f,
      };

      {
        voo::cmd_buf_one_time_submit pcb{cb};

        auto scb = sw.cmd_render_pass(cb);
        vee::cmd_bind_gr_pipeline(cb, *gp);
        vee::cmd_push_vert_frag_constants(cb, *pl, &pc);
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
