#ifndef PAYLOAD_STREAM_H
#define PAYLOAD_STREAM_H

namespace u3v {

class PayloadStream {
public:
    // Methods for opening and closing the payload stream handle
    virtual void open() = 0;
    virtual void close() = 0;

    // Method for starting the streaming loop
    virtual void startStreamingLoop(PayloadSender sender, DeviceControl* ctrl) = 0;

    // Method for stopping the streaming loop
    virtual void stopStreamingLoop() = 0;

    // Method for checking if the streaming loop is running
    virtual bool isLoopRunning() const = 0;
};

} // namespace u3v

#endif // PAYLOAD_STREAM_H

