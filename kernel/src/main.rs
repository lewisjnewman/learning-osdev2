#![no_std]
#![no_main]
#![feature(asm)]

use core::panic::PanicInfo;

use uart_16550::SerialPort;

const IOPORTNUM: u16 = 0x3F8;

static HELLO_WORLD: &[u8] = b"Hello World From The Kernel";

#[no_mangle]
pub extern "C" fn kmain() -> ! {

    let mut serial_port = unsafe { SerialPort::new(IOPORTNUM) };
    serial_port.init();

    for byte in HELLO_WORLD {
        serial_port.send(*byte);
    }

    let vga_buffer = 0xb8000 as *mut u8;

    for (i, &byte) in HELLO_WORLD.iter().enumerate() {
        unsafe {
            *vga_buffer.offset(i as isize * 2) = byte;
            *vga_buffer.offset(i as isize * 2 + 1) = 0x1F;
        }
    }

    loop {
        unsafe {asm!("hlt")};
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}