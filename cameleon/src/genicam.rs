//! This module contains a unified C API that can be shared between Cameleon and Aravis.

use libc::{c_double, c_int, c_void};
use std::ptr;
use crate::u3v;
use crate::u3v::ControlHandle;
use crate::u3v::StreamHandle;
use crate::camera::Camera;

/// Genicam opaque camera descriptor.
#[repr(C)]
pub struct GenicamCamera {
    camera: Camera<ControlHandle, StreamHandle>,
}

/// Try to pick up a Genicam camera best matching the provided specification.
#[no_mangle]
pub unsafe extern "C" fn genicam_new(vid: *const c_int, pid: *const c_int, width: *const c_int, height: *const c_int, fps: *const c_double) -> *mut c_void {
    if vid.is_null() || pid.is_null() || width.is_null() || height.is_null() || fps.is_null() {
        return ptr::null_mut();
    }

    // Logic to find and initialize a Genicam camera matching the specification
    let cameras = u3v::enumerate_cameras().unwrap_or_default();
    let camera = cameras.into_iter().find(|c| {
        (vid.is_null() || i32::from(c.info.vid) == *vid) &&
        (pid.is_null() || i32::from(c.info.pid) == *pid) //&&
        //(width.is_null() || c.width == *width) &&
        //(height.is_null() || c.height == *height) &&
        //(fps.is_null() || c.fps == *fps)
    });

    if let Some(camera) = camera {
        let genicam_camera = Box::new(GenicamCamera { camera });
        Box::into_raw(genicam_camera) as *mut c_void
    } else {
        ptr::null_mut()
    }
}

/// Try to pick up the first available Genicam camera.
#[no_mangle]
pub extern "C" fn genicam_new_any() -> *mut c_void {
    let cameras = u3v::enumerate_cameras().unwrap_or_default();
    if let Some(camera) = cameras.into_iter().next() {
        let genicam_camera = Box::new(GenicamCamera { camera });
        Box::into_raw(genicam_camera) as *mut c_void
    } else {
        ptr::null_mut()
    }
}

/// Release the Genicam camera.
#[no_mangle]
pub extern "C" fn genicam_release(genicam_camera: *mut c_void) {
    if genicam_camera.is_null() {
        return;
    }
    unsafe {
        drop(Box::from_raw(genicam_camera as *mut GenicamCamera));
    }
}
