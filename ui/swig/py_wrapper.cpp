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

#include <pybind11/pybind11.h>

// Do not include FUSE headers, they can conflict with pybind11 dependencies headers
// Just declare the functions we want to expose and let the linker find them
extern "C" {

#include "libspectrum.h"

extern int fuse_exiting;		/* Shall we exit now? */

int machine_select( libspectrum_machine type );
int fuse_init(int argc, char **argv);
int fuse_end(void);
int unittests_run(void);
int machine_reset(int hard_reset);
void z80_do_opcodes(void);
int event_do_events(void);
}

// wrapper

void init() {
    // make program name for char **argv
    char prog[] = "fuse\0";
    char *argv[] = { prog };
    fuse_init(1, argv);
}

// TODO it seems we should make singleton

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

    m.def("init", &init, "Initialize FUSE");
    m.def("deinit", &fuse_end, "Deinitialize FUSE");
    m.def("select_machine", &machine_select, "Select machine type");
    m.def("run_unittests", &unittests_run, "Run unit tests");
    m.def("reset", &machine_reset, "Reset machine");
    m.def("do_opcodes", &z80_do_opcodes, "Run Z80 opcodes until next event");
    m.def("do_events", &event_do_events, "Handle all events which have passed");
}
