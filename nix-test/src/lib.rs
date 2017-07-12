extern crate libc;

use std::fmt;
use std::ffi::CString;
use libc::{c_int, c_char, c_void};

mod ffi {
    use libc::{c_int, c_char, size_t, c_void};

    #[link(name = "nixtest", kind = "static")]
    extern {
        pub fn get_int_const(errno: *const c_char) -> c_int;
        pub fn size_of(ty: *const c_char) -> size_t;
        pub fn cmsg_space(size: size_t) -> size_t;
        pub fn cmsg_len(size: size_t) -> size_t;
        pub fn cmsg_data_offset() -> size_t;
        pub fn cmsg_init(cmsghdr: *mut c_void, fds: usize, cmsgs: usize);
        pub fn cmsg_valid(cmsghdr: *const c_void, len: usize) -> bool;
    }
}

pub fn assert_const_eq<T: GetConst>(name: &str, actual: T) {
    unsafe {
        let cstr = CString::new(name).unwrap();
        let expect = GetConst::get_const(cstr.as_ptr());

        if actual != expect {
            panic!("incorrect value for errno {}; expect={}; actual={}",
                   name, expect, actual);
        }
    }
}

pub fn assert_size_of<T>(name: &str) {
    use std::mem;

    unsafe {
        let cstr = CString::new(name).unwrap();
        let expect = ffi::size_of(cstr.as_ptr()) as usize;

        assert!(expect > 0, "undefined type {}", name);

        if mem::size_of::<T>() != expect {
            panic!("incorrectly sized type; type={} expect={}; actual={}",
                   name, expect, mem::size_of::<T>());
        }
    }
}

pub fn assert_cmsg_space_of<T>(size: usize) {
    use std::mem;

    unsafe {
        let expect = ffi::cmsg_space(mem::size_of::<T>());
        if size != expect {
            panic!("incorrect cmsg space calculation; expect={} actual={}",
                   expect, size);
        }
    }
}

pub fn assert_cmsg_len_of<T>(len: usize) {
    use std::mem;

    unsafe {
        let expect = ffi::cmsg_len(mem::size_of::<T>());
        if len != expect {
            panic!("incorrect cmsg len calculation; expect={} actual={}",
                   expect, len);
        }
    }
}

pub fn assert_cmsg_data_offset(off: usize) {
    unsafe {
        let expect = ffi::cmsg_data_offset();
        if off != expect {
            panic!("incorrect cmsg_data offset calculation; expect={} actual={}",
                   expect, off);
        }
    }
}

pub fn cmsg_init(cmsghdr: *mut c_void, fds: usize, cmsgs: usize) {
    unsafe {
        ffi::cmsg_init(cmsghdr, fds, cmsgs);
    }
}

pub fn assert_cmsg_valid(cmsghdr: *const c_void, len: usize) {
    unsafe {
        if !ffi::cmsg_valid(cmsghdr, len) {
            panic!("invalid cmsg");
        }
    }
}

pub use ffi::get_int_const;

pub trait GetConst : PartialEq<Self> + fmt::Display {
    unsafe fn get_const(name: *const c_char) -> Self;
}

impl GetConst for c_int {
    unsafe fn get_const(name: *const c_char) -> c_int {
        ffi::get_int_const(name)
    }
}

impl GetConst for u32 {
    unsafe fn get_const(name: *const c_char) -> u32 {
        ffi::get_int_const(name) as u32
    }
}
