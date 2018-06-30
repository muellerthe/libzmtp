#include "net/af.h"
#include "net/gnrc/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initializes the Transmission Control Block (TCB)
 *
 * @param[in,out] tcb      TCB that should be initialized.
 */
void socket(gnrc_tcp_tcb_t *tcb);


/**
 * @brief Opens a connection actively.
 *
 * @note Blocks until a connection has been established or an error occured.
 *
 * @param[in,out] tcb              TCB holding the connection information.
 * @param[in]     target_addr      Pointer to the target address.
 * @param[in]     target_port      Target port number.
 *
 * @returns   Zero on success.
 *            1 if an error occured.
 */
int connect (gnrc_tcp_tcb_t *tcb, char *target_addr, uint16_t target_port);


/**
 * @brief Opens a connection passively by waiting for an incoming request.
 *
 * @note Blocks until a connection has been established or an error occured.
 *
 * @param[in,out] tcb              TCB holding the connection information.
 * @param[in]     local_port       Port number to listen on.
 *
 * @returns   Zero on success.
 *            1 if an error occured.
 */
int bind_listen_accept(gnrc_tcp_tcb_t *tcb, uint16_t local_port);


#ifdef __cplusplus
}
#endif