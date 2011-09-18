
#ifndef SWGANH_CONNECTION_PING_SERVER_H_
#define SWGANH_CONNECTION_PING_SERVER_H_

#include <cstdint>
#include <vector>

#include <boost/asio.hpp>

namespace swganh {
namespace connection {

class PingServer
{
public:
    /*! \brief Start a ping service listening on all addresses for a specified port.
     *
     * @param io_service The instance of io_service that drives this server.
     * @param port The port to listen for ping requests on.
     */
    PingServer(boost::asio::io_service& io_service, uint16_t port);

    /*! Default destructor */
    ~PingServer();

    /*! @returns Returns the bytes received since the Ping Server was started */
    uint64_t BytesReceived() const;

    /*! @returns Returns the bytes sent since the Ping Server was started */
    uint64_t BytesSent() const;

private:
    /* Disable the default constructor, copy constructor and assignment operators */
    PingServer();
    PingServer(const PingServer&);
    PingServer& operator=(const PingServer&);

    void AsyncReceive();
    void HandleReceive(const boost::system::error_code& error, size_t bytesReceived);
    void HandleSend(const boost::system::error_code& error, size_t bytesSent);

    boost::asio::ip::udp::endpoint remote_endpoint_;   //Storage for Current Client End Point (Thread Safe)
    boost::asio::ip::udp::socket socket_;            //Server Socket

    std::vector<uint8_t> receive_buffer_;

    uint64_t bytes_received_;
    uint64_t bytes_sent_;
};

}}  // namespace swganh::connection


#endif  // #define SWGANH_CONNECTION_PING_SERVER_H_

