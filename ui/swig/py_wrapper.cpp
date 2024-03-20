// FUZX is a Python extension for Free Unix Spectrum Emulator (Fuse)

// Fuse emulator is C library with a lot of global state, we can't have more than one instance of it
// so we do not have to use a class to wrap it, we can just use free functions

// API we want to expose:
// * Select machine type
// * Select peripherals
// * Load snapshot, tape, disc, etc
// * Load code and BASIC program via API
// * Toggle NMI, IRQ, RESET
// * Keyboard input
// * Joystick input
// * Mouse input
// * Run BASIC program or code from arbitrary memory location
// * Advance running machine frame by frame,
//   * Getting back video raster image, paletted, not RGB
//   * Getting back audio samples for the frame
// * Get/set machine state
//   * Program counter, registers, flags, memory, etc
// * Save snapshot, tape, disc, etc
// * Realtime running with streaming video and audio with different speeds

// Enums and constants we want to expose:
// * Machine type
// * Peripherals
// * Video mode (Timex)
// * Keyboard key codes

// Error handling:
// * Exceptions for errors

// Buliding:
// ./autogen.sh
// ./configure --with-swig --with-pyext --without-zlib --without-png --with-pic --with-roms-dir=roms
// make

#include <ios>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Do not include FUSE headers, they can conflict with pybind11 dependencies headers
// Just declare the functions we want to expose and let the linker find them
extern "C" {

#include "libspectrum.h"
#include "../../machine.h"
#include "../../loader.h"
#include "../../z80/z80.h"
#include "../../memory_pages.h"
#include "../../tape.h"
#include "../../settings.h"
#include "../../timer/timer.h"

extern int fuse_exiting;		/* Shall we exit now? */

int fuse_init(int argc, char **argv);
int fuse_end(void);
int unittests_run(void);
int event_do_events(void);

}

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

void check_status(int status) {
    if (status != 0) {
        // TODO get error message from ui_error intercepted calls
        throw std::runtime_error("FUSE error");
    }
}

std::string safe_to_string(const char *cstr) {
    return cstr ? std::string(cstr) : "NULL";
}

using ram_page_t = std::array<libspectrum_byte, 0x4000>;
// typedef libspectrum_byte ram_pages_t[ SPECTRUM_RAM_PAGES ][0x4000];


class Fuzx {
public:
    static Fuzx& Instance() {
        std::cerr << "Fuzx instance" << std::endl;
        if (Instance_ == nullptr) {
            Instance_ = std::unique_ptr<Fuzx>(new Fuzx());
        }
        return *Instance_;
    }

    static void Destroy() {
        std::cerr << "Fuzx destroy" << std::endl;
        Instance_.reset();
    }

    void SelectMachine(libspectrum_machine machine) const {
        check_status(machine_select(machine));
    }

    void RunUnittests() const {
        check_status(unittests_run());
    }

    void Reset(int hard_reset = 1) const {
        machine_reset(hard_reset);
    }

    void DoOpcodes() const {
        z80_do_opcodes();
    }

    void DoEvents() const {
        event_do_events();
    }

    libspectrum_word GetProcessorRegPC() const {
        return z80.pc.w;
    }

    fuse_machine_info& GetMachineInfo() const {
        if (machine_current == nullptr) {
            throw std::runtime_error("No machine selected");
        }
        return *machine_current;
    }

    processor& GetProcessor() const {
        return z80;
    }

    ram_page_t& GetRAMPage(size_t page) const {
        return reinterpret_cast<ram_page_t&>(RAM[page]);
    }

    libspectrum_byte GetScreenPageNum() const {
        return memory_current_screen;
    }

    std::array<libspectrum_byte, 6912>& GetScreenData() const {
        ram_page_t& page = GetRAMPage(GetScreenPageNum());
        return reinterpret_cast<std::array<libspectrum_byte, 6912>&>(page);
    }

    void LoadTape(const std::string &filename, bool autoload) const {
        std::cerr << "Fuzx load tape " << filename << std::endl;
        check_status(tape_open(filename.c_str(), autoload));
    }

    void LoadTapeWait(const std::string &filename) const {
        LoadTape(filename, true);
        WaitFastLoading();
    }

    settings_info& GetSettings() const {
        return settings_current;
    }

    bool IsExiting() const {
        return fuse_exiting;
    }

    bool IsFastLoadingActive() const {
        return timer_fastloading_active();
    }

    void WaitFastLoading(int max_frames = 100 * 60 * 50) const {
        std::cerr << "Fuzx wait fast loading...";
        std::cerr.flush();
        int frames = 0;
        while (IsFastLoadingActive() && frames++ < max_frames) {
            DoOpcodes();
            DoEvents();
            if (frames % 50 == 0) {
                std::cerr << ".";
                std::cerr.flush();
            }
        }
        std::cerr << " done" << std::endl;
    }

    ~Fuzx() noexcept(false) {
        std::cerr << "Fuzx desctructor" << std::endl;
        fuse_end();
        if (Instance_ != nullptr) {
            throw std::runtime_error("Fuzx instance destroyed twice");
        }
    }

private:
    Fuzx() {
        std::cerr << "Fuzx constructor" << std::endl;
        char prog[] = "fuse\0";
        char *argv[] = { prog };
        fuse_init(1, argv);
    }

    static std::unique_ptr<Fuzx> Instance_;
};


std::unique_ptr<Fuzx> Fuzx::Instance_ {};


std::string to_string(const regpair &rp) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << int(rp.w) << std::dec
        << " (" << int(rp.w)
        << ", h=" << int(rp.b.h)
        << " l=" << int(rp.b.l)
        << ")" << std::nouppercase;
    return ss.str();
}

std::string to_hex(const libspectrum_dword &value) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << value << std::dec << std::nouppercase;
    return ss.str();
}

std::string to_hex(const regpair &rp) {
    return to_hex(rp.w);
}


// Binding

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(fuzx, m) {
    m.doc() = "FUZX is a Python extension for Free Unix Spectrum Emulator (FUSE)";

    py::enum_<libspectrum_machine>(m, "Machine")
        .value("ZXSpectrum48",      LIBSPECTRUM_MACHINE_48)
        .value("TimexTC2048",       LIBSPECTRUM_MACHINE_TC2048)
        .value("ZXSpectrum128",     LIBSPECTRUM_MACHINE_128)
        .value("ZXSpectrumPlus2",   LIBSPECTRUM_MACHINE_PLUS2)
        .value("Pentagon128",       LIBSPECTRUM_MACHINE_PENT)
        .value("ZXSpectrumPlus2A",  LIBSPECTRUM_MACHINE_PLUS2A)
        .value("ZXSpectrumPlus3",   LIBSPECTRUM_MACHINE_PLUS3)
        .value("Unknown",           LIBSPECTRUM_MACHINE_UNKNOWN)
        .value("ZXSpectrum16",      LIBSPECTRUM_MACHINE_16)
        .value("TimexTC2068",       LIBSPECTRUM_MACHINE_TC2068)
        .value("ScorpionZS256",     LIBSPECTRUM_MACHINE_SCORP)
        .value("ZXSpectrumPlus3E",  LIBSPECTRUM_MACHINE_PLUS3E)
        .value("SpectrumSE",        LIBSPECTRUM_MACHINE_SE)
        .value("TimexTS2068",       LIBSPECTRUM_MACHINE_TS2068)
        .value("Pentagon512",       LIBSPECTRUM_MACHINE_PENT512)
        .value("Pentagon1024",      LIBSPECTRUM_MACHINE_PENT1024)
        .value("ZXSpectrum48NTSC",  LIBSPECTRUM_MACHINE_48_NTSC)
        .value("ZXSpectrum128E",    LIBSPECTRUM_MACHINE_128E)
        .export_values();

    py::class_<machine_timings>(m, "MachineTimings")
        .def(py::init<>())
        .def("__repr__", [](const machine_timings &a) {
            return "<MachineTimings processor_speed=" + std::to_string(a.processor_speed)
                    + " tstates_per_frame=" + std::to_string(a.tstates_per_frame)
                    + ">";
        })
        .def_readonly("processor_speed", &machine_timings::processor_speed)
        .def_readonly("left_border", &machine_timings::left_border)
        .def_readonly("horizontal_screen", &machine_timings::horizontal_screen)
        .def_readonly("right_border", &machine_timings::right_border)
        .def_readonly("tstates_per_line", &machine_timings::tstates_per_line)
        .def_readonly("interrupt_length", &machine_timings::interrupt_length)
        .def_readonly("tstates_per_frame", &machine_timings::tstates_per_frame)
        ;

    py::class_<spectrum_raminfo>(m, "SpectrumRamInfo")
        .def(py::init<>())
        .def("__repr__", [](const spectrum_raminfo &a) {
            return "<SpectrumRamInfo>";
        })
        ;

    py::class_<ayinfo>(m, "AyInfo")
        .def(py::init<>())
        .def("__repr__", [](const ayinfo &a) {
            return "<AyInfo>";
        })
        ;

    py::class_<fuse_machine_info>(m, "MachineInfo")
        .def(py::init<>())
        .def("__repr__", [](const fuse_machine_info &a) {
            return "<MachineInfo machine = " + std::to_string(a.machine)
                    + " id = \"" + safe_to_string(a.id) + "\""
                    + ">";
        })
        .def_readonly("machine", &fuse_machine_info::machine)
        .def_readonly("id", &fuse_machine_info::id)
        .def_readonly("capabilities", &fuse_machine_info::capabilities)
        .def_readonly("timex", &fuse_machine_info::timex)
        .def_readonly("timings", &fuse_machine_info::timings)
        ;

    py::class_<regpair>(m, "RegPair")
        .def(py::init<>())
        .def("__repr__", [](const regpair &a) {
            return to_string(a);
        })
        .def_property("w", [](const regpair &rp) {
            return rp.w;
        }, [](regpair &rp, libspectrum_word value) {
            rp.w = value;
        })
        .def_property("h", [](const regpair &rp) {
            return rp.b.h;
        }, [](regpair &rp, libspectrum_byte value) {
            rp.b.h = value;
        })
        .def_property("l", [](const regpair &rp) {
            return rp.b.l;
        }, [](regpair &rp, libspectrum_byte value) {
            rp.b.l = value;
        })
        ;

    py::class_<processor>(m, "Processor")
        .def(py::init<>())
        .def("__repr__", [](const processor &a) {
            return "<Processor af=" + to_hex(a.af)
                    + " bc=" + to_hex(a.bc)
                    + " de=" + to_hex(a.de)
                    + " hl=" + to_hex(a.hl)
                    + " af_=" + to_hex(a.af_)
                    + " bc_=" + to_hex(a.bc_)
                    + " de_=" + to_hex(a.de_)
                    + " hl_=" + to_hex(a.hl_)
                    + " ix=" + to_hex(a.ix)
                    + " iy=" + to_string(a.iy)
                    + " i=" + std::to_string(a.i)
                    + " r=" + std::to_string(a.r)
                    + " r7=" + std::to_string(a.r7)
                    + " sp=" + to_hex(a.sp)
                    + " pc=" + to_hex(a.pc)
                    + " im=" + std::to_string(a.im)
                    + " halted=" + std::to_string(a.halted);
        })
        .def_readwrite("af", &processor::af)
        .def_readwrite("bc", &processor::bc)
        .def_readwrite("de", &processor::de)
        .def_readwrite("hl", &processor::hl)
        .def_readwrite("af_", &processor::af_)
        .def_readwrite("bc_", &processor::bc_)
        .def_readwrite("de_", &processor::de_)
        .def_readwrite("hl_", &processor::hl_)
        .def_readwrite("ix", &processor::ix)
        .def_readwrite("iy", &processor::iy)
        .def_readwrite("i", &processor::i)
        .def_readwrite("r", &processor::r)
        .def_readwrite("r7", &processor::r7)
        .def_readwrite("sp", &processor::sp)
        .def_readwrite("pc", &processor::pc)
        .def_readwrite("memptr", &processor::memptr)
        .def_readwrite("iff2_read", &processor::iff2_read)
        .def_readwrite("iff1", &processor::iff1)
        .def_readwrite("iff2", &processor::iff2)
        .def_readwrite("im", &processor::im)
        .def_readwrite("halted", &processor::halted)
        .def_readwrite("clockl", &processor::clockl)
        .def_readwrite("clockh", &processor::clockh)
        ;

    // from settings.h, but we could autogenerate it, obviously
    // or better make it dynamic, not static properties
    // and make it via config file reading code
    py::class_<settings_info>(m, "Settings")
        .def(py::init<>())
        .def_readwrite("accelerate_loader", &settings_info::accelerate_loader)
        .def_readwrite("auto_load", &settings_info::auto_load)
        .def_readwrite("detect_loader", &settings_info::detect_loader)
        .def_readwrite("emulation_speed", &settings_info::emulation_speed)
        .def_readwrite("fastload", &settings_info::fastload)
        .def_readwrite("frame_rate", &settings_info::frame_rate)
        .def_readwrite("gdbserver_enable", &settings_info::gdbserver_enable)
        .def_readwrite("gdbserver_port", &settings_info::gdbserver_port)
        .def_readwrite("interface1", &settings_info::interface1)
        .def_readwrite("interface2", &settings_info::interface2)
        .def_readwrite("issue2", &settings_info::issue2)
        .def_readwrite("joy_kempston", &settings_info::joy_kempston)
        .def_readwrite("joy_prompt", &settings_info::joy_prompt)
        .def_readwrite("slt_traps", &settings_info::slt_traps)
        .def_readwrite("sound", &settings_info::sound)
        .def_readwrite("tape_traps", &settings_info::tape_traps)
        .def("__repr__", [](const settings_info &a) {
            return "<Settings frame_rate=" + std::to_string(a.frame_rate)
                    + " emulation_speed=" + std::to_string(a.emulation_speed)
                    + " accelerate_loader=" + std::to_string(a.accelerate_loader)
                    + " fastload=" + std::to_string(a.fastload)
                    + " sound=" + std::to_string(a.sound)
                    + " tape_traps=" + std::to_string(a.tape_traps)
                    + ">";
        })
        ;

    py::class_<Fuzx>(m, "Fuzx")
        .def_static("machine", &Fuzx::Instance, "Get Fuzx instance",
                    py::return_value_policy::reference)
        .def_static("destroy", &Fuzx::Destroy, "Destroy Fuzx instance")
        .def("select_machine", &Fuzx::SelectMachine, "Select machine type")
        .def("run_unittests", &Fuzx::RunUnittests, "Run unit tests")
        .def("reset", &Fuzx::Reset, "Reset machine", py::arg("hard_reset") = 1)
        .def("do_opcodes", &Fuzx::DoOpcodes, "Run Z80 opcodes until next event")
        .def("do_events", &Fuzx::DoEvents, "Handle all events which have passed")
        .def("get_info", &Fuzx::GetMachineInfo, "Get machine info", py::return_value_policy::reference)
        .def_property_readonly("processor", &Fuzx::GetProcessor, "Get Z80 processor", py::return_value_policy::reference)
        .def("ram_page", &Fuzx::GetRAMPage, "Get RAM page", py::return_value_policy::reference)
        .def_property_readonly("screen_page_num", &Fuzx::GetScreenPageNum, "Get screen page")
        .def_property_readonly("screen_data", &Fuzx::GetScreenData, "Get screen data", py::return_value_policy::reference)
        .def("load_tape", &Fuzx::LoadTape, "Load tape", py::arg("filename"), py::arg("autoload") = 1)
        .def("load_tape_wait", &Fuzx::LoadTapeWait, "Load tape and wait for fast loading", py::arg("filename"))
        .def_property_readonly("settings", &Fuzx::GetSettings, "Get settings", py::return_value_policy::reference)
        .def_property_readonly("is_exiting", &Fuzx::IsExiting, "Is FUSE exiting")
        .def_property_readonly("is_fast_loading_active", &Fuzx::IsFastLoadingActive, "Is fast loading active")
        .def("wait_fast_loading", &Fuzx::WaitFastLoading, "Wait for fast loading to finish", py::arg("max_frames") = 100 * 60 * 50)
        ;

}
