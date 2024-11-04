/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

//! This example is a streaming FPS rate benchmark:
//! 1. Run the camera image capture infinitely
//! 2. Calculate the frame rate
//! 3. Report the frame rate to stdout every 1 second

use cameleon::u3v::enumerate_cameras;
use std::time::{Duration, Instant};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;

fn main() {
    let running = Arc::new(AtomicBool::new(true));
    let r = running.clone();

    ctrlc::set_handler(move || {
        r.store(false, Ordering::SeqCst);
    }).expect("Error setting Ctrl-C handler");

    let mut cameras = enumerate_cameras().unwrap();
    if cameras.is_empty() {
        println!("no camera found!");
        return;
    }

    let mut camera = cameras.pop().unwrap();
    camera.open().unwrap();
    camera.load_context().unwrap();

    let payload_rx = camera.start_streaming(3).unwrap();
    let mut frame_count = 0;
    let mut start_time = Instant::now();

    loop {
        if !running.load(Ordering::SeqCst) {
            camera.close().ok();
            break;
        }

        match payload_rx.recv_blocking() {
            Ok(payload) => {
                frame_count += 1;
                payload_rx.send_back(payload);
            }
            Err(e) => {
                println!("payload receive error: {e}");
            }
        }

        if start_time.elapsed() >= Duration::from_secs(1) {
            let elapsed = start_time.elapsed();
            println!("FPS: {}", frame_count as f64 / elapsed.as_secs_f64());
            frame_count = 0;
            start_time = Instant::now();
        }
    }
}
