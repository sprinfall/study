/*
 * Copyright (c) 2009-2010, Dennis Wang (dennis.wangkk@gmail.com)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef WIN32
#   define _SCL_SECURE_NO_WARNINGS
#   define _WIN32_WINNT 0x0501
#endif

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#define NE_CLIENT_VERSION "0.1.0"

using namespace boost::program_options;
using boost::asio::ip::tcp;

enum {
    max_write_buf_length = 1024,
    max_read_buf_length  = 102400
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
          std::cerr << "Usage: console <host> <port>\n";
          return 1;
        }

        std::cout << "Copyright (c) 2009-2010, Dennis Wang (dennis.wangkk@gmail.com)" << std::endl;
        std::cout << "All rights reserved. " << "Noodle Engine Client Version " << NE_CLIENT_VERSION << ", See \"help\"" << std::endl;

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect (*iterator);

        using namespace std; // For strlen.
        char request[max_write_buf_length];

        std::cout << "ne>";
        while (std::cin.getline(request, max_write_buf_length)) {
            if (!strcmp (request, "exit")) {
                return 0;
            }

            size_t request_length = strlen(request);

            if (request_length < max_write_buf_length - 2) {
                request[request_length] = '\r';
                request[request_length + 1] = '\n';
                request_length += 2;
            }

            boost::asio::write(s, boost::asio::buffer(request, request_length));

            boost::system::error_code error;
            boost::asio::streambuf buffer (max_read_buf_length);
            size_t reply_length = boost::asio::read_until (
                s,
                buffer,
                std::string ("\r\n"),
                error);
            
            if (error) {
                std::cout << error.message () << std::endl;
                continue;
            }

            buffer.commit(reply_length);

            std::cout.write(boost::asio::buffer_cast<const char*>(buffer.data ()), reply_length - 2);
            std::cout << "\n";
            std::cout << "ne>";
        }
    } catch (std::exception& e) {
        std::cerr << "lost connection:" << e.what () << std::endl;
    }

    return 0;
}
