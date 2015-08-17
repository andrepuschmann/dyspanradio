#ifndef MULTICHANNELRX_H
#define MULTICHANNELRX_H

// This class is based on liquid-usrp's multichannelrx class

#include "dyspanradio.h"
#include "Buffer.h"
#include "buffer_manager.h" // TODO: rename file
#include <vector>
#include <map>
#include <liquid/liquid.h>

#define SKIP_MIXING 1

#define MULTITHREAD 1
#define THREAD_BUFFER_SIZE 20

class multichannelrx : public DyspanRadio {
public:
    // default constructor
    multichannelrx(const std::string args,
                   const int num_channels,
                   const double f_center,
                   const double channel_bandwidth,
                   const double channel_rate,
                   const float rx_gain_uhd,
                   unsigned int    M,            // OFDM: number of subcarriers
                   unsigned int    cp_len,       // OFDM: cyclic prefix length
                   unsigned int    taper_len,    // OFDM: taper prefix length
                   unsigned char * p,            // OFDM: subcarrier allocation
                   bool debug);

    // destructor
    ~multichannelrx();

    void start();
    void stop();

    // reset multi-channel receiver
    void Reset();

    // accessor methods
    unsigned int GetNumChannels() { return num_channels_; }

    // push samples into base station receiver


private:
    // ...
    void receive_thread();
    void mixdown_thread();
    void channelizer_thread();
    void synchronizer_thread(Buffer<BufferElement> &buffer, const int channel_index);

    void mix_down(std::complex<float> * _x, unsigned int _num_samples);
    void channelize(std::complex<float> * _y, unsigned int counter);
    void sychronize(std::complex<float> * _x, const int len, const int channel_index);

    // finite impulse response polyphase filterbank channelizer
    firpfbch_crcf channelizer;      // channelizer size is 2*num_channels
    unsigned int buffer_index;      // input index

    size_t num_sampled_chans_;
    size_t max_spp_;

    BufferFactory buffer_factory_;
    Buffer<BufferElement> rx_to_mix_buffer_;
    Buffer<BufferElement> mix_to_chan_buffer_;
    boost::ptr_vector<Buffer<BufferElement> > chan_to_sync_buffers_;

    // objects
    ofdmflexframesync * framesync;  // array of frame generator objects
    void ** userdata;               // array of userdata pointers
    framesync_callback * callbacks; // array of callback functions
    nco_crcf nco;                   // frequency-centering NCO

    uhd::usrp::multi_usrp::sptr usrp_rx;
};

#endif // MULTICHANNELRX_H

