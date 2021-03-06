/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2015  Andre Puschmann, Francisco Paisana, Justin Tallon
 *
 * \section LICENSE
 *
 * This file is part of dyspanradio.
 *
 * dyspanradio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * dyspanradio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef OFDMTRANSCEIVER_H
#define OFDMTRANSCEIVER_H

// This class is based on liquid-usrp's ofdmtxrx class.

#include <complex>
#include <boost/ptr_container/ptr_deque.hpp>
#include <liquid/liquid.h>
#include "dyspanradio.h"
#include "Buffer.h"
#include "channels.h"
#include "EnergyDetector2.h"
#include "EnergyDetector.h"

class OfdmTransceiver : public DyspanRadio
{
public:
    // default constructor   
    OfdmTransceiver(const RadioParameter params);
    // destructor
   virtual ~OfdmTransceiver();

    //
    // transmitter methods
    //
    void set_tx_freq(float _tx_freq);
    void set_tx_rate(float _tx_rate);
    void set_tx_gain_soft(float gain);
    void set_tx_gain_uhd(float gain);
    void set_tx_antenna(char * _tx_antenna);

    void start();
    void stop();
    void reset_tx();

    // update payload data on a particular channel
    void transmit_packet();

    //
    // receiver methods
    //
    void set_rx_freq(float _rx_freq);
    void set_rx_rate(float _rx_rate);
    void set_rx_gain_uhd(float _rx_gain_uhd);
    void set_rx_antenna(char * _rx_antenna);
    void reset_rx();
    void start_rx();
    void stop_rx();

private:
    ofdmflexframegenprops_s fgprops;// frame generator properties
    unsigned int  current_channel;
    unsigned int next_channel;
    bool sensing_calibration;
    bool txrx_calibration;
    // transmitter objects
    ofdmflexframegen fg;            // frame generator object
    std::complex<float> * fgbuffer; // frame generator output buffer [size: M + cp_len x 1]
    //boost::scoped_ptr<CplxFVec> fgbuffer; // TODO: Convert to smart ptr
    spectrum* tx_;                  // handle to the challenge database
    int payload_len_;               // the actual payload length used for transmission

    unsigned int fgbuffer_len;      // length of frame generator buffer (is the size of a single OFDM symbol)
    float tx_gain;                  // soft transmit gain (linear)
    uint32_t seq_no_;

    //boost::ptr_deque<CplxFVec> frame_buffer;
    Buffer<boost::shared_ptr<CplxFVec> > frame_buffer;

    // receiver objects
    EnergyDetector2 e_detec;
    std::pair<double,bool> DwellEst(DwellTimeEstimator &Dwell, double &previous_dwelltime, int &dwell_counter, int steady_state, double steady_state_Th);
    bool learning;
    uhd::time_spec_t timestamp_;

    // member functions
    void transmit_function();
    void random_transmit_function(); // this is just a test function which randomly transmits on every available channel
    void modulation_function();
    void receive_function();
    void reconfigure_usrp(const int num);
    std::pair<bool,double> dwelltimer();
    void process_sensing(std::vector<float> ChPowers);
    // RF objects and properties
    uhd::usrp::multi_usrp::sptr usrp_tx;
    uhd::usrp::multi_usrp::sptr usrp_rx;
    uhd::tx_metadata_t          metadata_tx;
};

#endif // OFDMTRANSCEIVER_H
