#ifndef EZSRV_NET_CLIENT_STATE_MACHINE_H
#define EZSRV_NET_CLIENT_STATE_MACHINE_H

#include "boost/system/error_code.hpp"

namespace ezsrv::net {
    enum class client_state {
        idle,
        authing,
        reading_header,
        reading_body,
        closing
    };

    namespace details {
        using boost::system::error_code;

        class client_state_machine {
          public:
            virtual void on_authing(std::string_view msg)        = 0;
            virtual void on_reading_header(std::string_view msg) = 0;
            virtual void on_reading_body(std::string_view msg)   = 0;
            virtual void on_error(const error_code &err)         = 0;

            void handle_read(std::size_t       nread,
                             std::string_view  msg,
                             const error_code &err) {
                if (nread == 0) {
                    return;
                } else if (err) {
                    on_error(err);
                    return;
                }

                switch (current_state_) {
                case client_state::authing:
                    on_authing(msg);
                    break;

                case client_state::reading_header:
                    on_reading_header(msg);
                    break;

                case client_state::reading_body:
                    on_reading_body(msg);
                    break;

                default:
                    break;
                }
            }

            inline client_state state() const noexcept {
                return current_state_;
            }

            inline void state(client_state state) noexcept {
                current_state_ = state;
            }

          private:
            client_state current_state_;
        };
    } // namespace details

    using details::client_state_machine;
} // namespace ezsrv::net

#endif /* EZSRV_NET_CLIENT_STATE_MACHINE_H */